import time

from machine import Pin

import config


class Motor:

    # two values that are never reset. This may be a bad idea, can cross that bridge when we come to it...
    position = 0
    lifetime_angle = 0

    # values representing where we are and want to be. These are >= 0 and < 360 (ie 360 -> 0).
    # current_angle is frequently reset by deliberate and drive by calibration.
    current_angle = 0

    _minimum_angle_delta = 0.2

    _minimum_position_delta_for_isr = 500

    _minimum_interval_between_steps_us = 1000

    _need_to_update_after_entry_isr = False
    _enter_position = 0
    _previous_enter_position = 0
    _need_to_update_after_exit_isr = False
    _exit_position = 0
    _previous_exit_position = 0

    _steps_per_rotation = 0
    _steps_across_home = 0
    _degrees_per_step = 0
    _position_has_been_determined = False

    # motor sequence related
    _step_index = 0
    _step_count = 8
    _step_sequence = (
        (1, 0, 0, 0),
        (1, 1, 0, 0),
        (0, 1, 0, 0),
        (0, 1, 1, 0),
        (0, 0, 1, 0),
        (0, 0, 1, 1),
        (0, 0, 0, 1),
        (1, 0, 0, 1),
    )

    def __init__(
        self, name, pin1, pin2, pin3, pin4, sensor_pin, sensor_rising_is_enter
    ):

        # name can be useful when debugging
        self._name = name

        # setup output pins
        self._pin1 = Pin(pin1, Pin.OUT)
        self._pin2 = Pin(pin2, Pin.OUT)
        self._pin3 = Pin(pin3, Pin.OUT)
        self._pin4 = Pin(pin4, Pin.OUT)

        # setup input pins and attach interupt
        self._sensor_rising_is_enter = sensor_rising_is_enter
        self._sensor_pin = Pin(sensor_pin, mode=Pin.IN, pull=Pin.PULL_UP)
        self._sensor_pin.irq(self.isr_cb, Pin.IRQ_FALLING | Pin.IRQ_RISING)

    def calibrate(self):
        """Spin the motor until we have calibrated, and then stop at noon."""

        # reset values
        self._steps_per_rotation = 0
        self._steps_across_home = 0
        self._enter_position = 0
        self._previous_enter_position = 0
        self._exit_position = 0
        self._previous_exit_position = 0

        # Keep stepping until we have values needed
        while not self.has_been_calibrated:
            self.step()
            time.sleep_us(self._minimum_interval_between_steps_us)

        # Go to noon (0 degrees)
        self.step_to_target_angle(0)

    @property
    def has_been_calibrated(self):
        return self._steps_per_rotation != 0 and self._steps_across_home != 0

    def forget_position(self):
        self._position_has_been_determined = False

    def goto_noon(self):
        """Goto noon. Determine position and/or run calibration if needed"""
        if self._position_has_been_determined:
            self.step_to_target_angle(0)
        elif self.has_been_calibrated:
            while not self._position_has_been_determined:
                self.step()
                time.sleep_us(self._minimum_interval_between_steps_us)
            self.step_to_target_angle(0)
        else:
            self.calibrate()

    def step_to_target_angle(self, target):

        while True:
            cw_delta = (target - self.current_angle) % 360

            if cw_delta < self._minimum_angle_delta:
                break

            self.step()
            time.sleep_us(self._minimum_interval_between_steps_us)

    def take_steps(self, num):
        dir = 1 if num > 0 else -1
        while num != 0:
            self.step(dir)
            time.sleep_us(self._minimum_interval_between_steps_us)

            num -= dir

    def step(self, dir=1):

        # prep step (reset if needed) and take it
        self._step_index += dir
        self._step_index = self._step_index % self._step_count
        self.writeStepToPins()

        # update our tracking of where we are
        self.position += dir
        self.current_angle += self._degrees_per_step % 360
        self.lifetime_angle += self._degrees_per_step

        self.update_after_isr()

    def sleep(self):
        self.writeToPins(0, 0, 0, 0)

    def wake(self):
        self.writeStepToPins()

    def writeStepToPins(self):
        self.writeToPins(
            self._step_sequence[self._step_index][0],
            self._step_sequence[self._step_index][1],
            self._step_sequence[self._step_index][2],
            self._step_sequence[self._step_index][3],
        )

    def writeToPins(self, p1, p2, p3, p4):
        self._pin1.value(p1)
        self._pin2.value(p2)
        self._pin3.value(p3)
        self._pin4.value(p4)

    #############################

    def is_in_home_zone(self):
        return self._sensor_pin.value() == self._sensor_rising_is_enter

    def isr_cb(self, pin):
        if pin.value() == self._sensor_rising_is_enter:
            self._need_to_update_after_entry_isr = True
            self._enter_position = self.position
        else:
            self._need_to_update_after_exit_isr = True
            self._exit_position = self.position

    def update_after_isr(self):
        if self._need_to_update_after_entry_isr:
            self.update_after_isr_entry_to_entry()
            self._need_to_update_after_entry_isr = False

        if self._need_to_update_after_exit_isr:
            self.update_after_isr_entry_to_exit()
            self._need_to_update_after_exit_isr = False

    def update_after_isr_entry_to_entry(self):

        # Get the change in steps, reset last entry point to here
        previous_enter_pos = self._previous_enter_position
        self._previous_enter_position = self._enter_position

        # what is the delta? return if too small (means jitter, or running backwards)
        entry_delta = self._enter_position - previous_enter_pos
        if entry_delta < self._minimum_position_delta_for_isr:
            return

        # entry to entry (full rotation)
        if previous_enter_pos:
            steps_per_rotation = self._enter_position - previous_enter_pos
            self._steps_per_rotation = steps_per_rotation
            self._degrees_per_step = 360 / steps_per_rotation
            print(
                "{2} enter: {0} ({1})".format(
                    self._enter_position, self._steps_per_rotation, self._name
                )
            )

        # If we also have _steps_across_home we can determine our position
        if self._steps_across_home:
            # Now calibrated, set angle correctly
            # Currently we have just entered home
            steps_to_home_center = self._steps_across_home / 2
            angle_to_home_center = steps_to_home_center * self._degrees_per_step
            new_current_angle = 360 - angle_to_home_center

            if True:
                delta = self.current_angle - new_current_angle
                if delta > 180:
                    delta -= 360
                print("{0} angle correction: {1}".format(self._name, delta))

            self.current_angle = new_current_angle
            self._position_has_been_determined = True

    def update_after_isr_entry_to_exit(self):
        # entry_to_exit

        # Get the change in steps, reset last entry point to here
        previous_exit_pos = self._previous_exit_position
        self._previous_exit_position = self._exit_position

        # what is the delta? return if too small (means jitter, or running backwards)
        entry_delta = self._exit_position - previous_exit_pos
        if entry_delta < self._minimum_position_delta_for_isr:
            return

        if self._previous_enter_position:
            self._steps_across_home = (
                self._exit_position - self._previous_enter_position
            )

        print(
            "{2} exit: {0} ({1})".format(
                self._exit_position, self._steps_across_home, self._name
            )
        )


class PupilMotor(Motor):
    def __init__(self):
        super().__init__(
            "pupil",
            config.pupil_stepper["pin1"],
            config.pupil_stepper["pin2"],
            config.pupil_stepper["pin3"],
            config.pupil_stepper["pin4"],
            config.pupil_stepper["sensor_pin"],
            config.pupil_stepper["sensor_rising_is_enter"],
        )


class GlintMotor(Motor):
    def __init__(self, pupil_motor):
        super().__init__(
            "glint",
            config.glint_stepper["pin1"],
            config.glint_stepper["pin2"],
            config.glint_stepper["pin3"],
            config.glint_stepper["pin4"],
            config.glint_stepper["sensor_pin"],
            config.glint_stepper["sensor_rising_is_enter"],
        )
        self._pupil_motor = pupil_motor

    def goto_noon(self):
        self._pupil_motor.goto_noon()
        return super().goto_noon()

    def calibrate(self):
        self._pupil_motor.calibrate()
        return super().calibrate()

    def calibrate_drift(self):
        self.goto_noon()

        # note down current positions
        pupil_start_pos = self._pupil_motor.position
        glint_start_pos = self.position
        print("start: pupil {0}, glint {1}".format(pupil_start_pos, glint_start_pos))

        # We now need to spin the pupil 360 degrees and measure the effect it has on us
        self._pupil_motor.step_to_target_angle(180)
        self._pupil_motor.step_to_target_angle(0)
        self.forget_position()
        self.goto_noon()

        # note down end positions
        pupil_end_pos = self._pupil_motor.position
        glint_end_pos = self.position
        print("  end: pupil {0}, glint {1}".format(pupil_end_pos, glint_end_pos))

        print(
            "delta: pupil {0}, glint {1}".format(
                pupil_end_pos - pupil_start_pos, glint_end_pos - glint_start_pos
            )
        )

    def isr_cb(self, pin):
        # Magnet may trigger switch at odd times - so make sure pupil is in home zone
        if self._pupil_motor.is_in_home_zone():
            super().isr_cb(pin)
