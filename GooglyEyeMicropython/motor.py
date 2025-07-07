import time

from machine import Pin


class Motor:

    position = 0
    current_angle = 0
    target_angle = 0

    _minimum_angle_delta = 0.2
    _degrees_per_step = 0

    _minimum_interval_between_steps_us = 1000

    _enter_position = 0
    _last_enter_position = 0
    _exit_position = 0
    _last_exit_position = 0

    _steps_per_rotation = 0
    _steps_across_home = 0

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
        self._last_enter_position = 0
        self._exit_position = 0
        self._last_exit_position = 0

        # Keep stepping until we have values needed
        while self._steps_per_rotation == 0 or self._steps_across_home == 0:
            self.step()
            time.sleep_us(self._minimum_interval_between_steps_us)

        # Now calibrated, set angle correctly
        # Currently we have just entered home
        steps_to_home_center = self._steps_across_home / 2
        angle_to_home_center = steps_to_home_center / self._steps_per_rotation * 360
        corrected_angle = 360 - angle_to_home_center
        self.current_angle = corrected_angle

        # Go to noon (0degrees)
        self.step_to_target_angle(0)

    def step_to_target_angle(self, target):
        self.target_angle = target
        while (
            self.cw_delta_from_current_to_target() > 0
            and 360 - self.cw_delta_from_current_to_target() > self._minimum_angle_delta
        ):
            self.step()
            time.sleep_us(self._minimum_interval_between_steps_us)

    def cw_delta_from_current_to_target(self):
        """returns cw angle to travel to get from current to target"""
        cw_delta = self.target_angle - self.current_angle
        return cw_delta % 360

    def step(self):

        # Always take a forwards step
        dir = 1

        self._step_index += dir

        if self._step_index >= self._step_count:
            self._step_index = 0

        self.writeStepToPins()
        self.position += dir
        self.current_angle += self._degrees_per_step % 360

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

    def isr_cb(self, pin):
        if pin.value() == self._sensor_rising_is_enter:
            self.isr_enter()
        else:
            self.isr_exit()

    def isr_enter(self):
        self._enter_position = self.position

    def isr_exit(self):
        self._exit_position = self.position

    def update_after_isr(self):
        # entry to entry (full rotation)
        if self._enter_position != self._last_enter_position:
            if self._last_enter_position:
                steps_per_rotation = self._enter_position - self._last_enter_position
                self._steps_per_rotation = steps_per_rotation
                self._degrees_per_step = 360 / steps_per_rotation

            self._last_enter_position = self._enter_position

            print(
                "{2} enter: {0} ({1})".format(
                    self._enter_position, self._steps_per_rotation, self._name
                )
            )

        # entry_to_exit
        if self._exit_position != self._last_exit_position:
            if self._last_enter_position:
                self._steps_across_home = (
                    self._exit_position - self._last_enter_position
                )
            self._last_exit_position = self._exit_position

            print(
                "{2} exit: {0} ({1})".format(
                    self._exit_position, self._steps_across_home, self._name
                )
            )
