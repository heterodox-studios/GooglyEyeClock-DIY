import time, gc

from machine import Pin


from . import config
from .motor import PupilMotor, GlintMotor


class Hand:

    _minimum_angle_delta = 360 / 12 / 60 / 2  # 30 seconds on hour hand

    _target_angle = 0

    @property
    def target_angle(self):
        return self._target_angle

    @target_angle.setter
    def target_angle(self, target):
        target = target % 360

        # If the angle is too small then don't set it
        angle_delta_cw = (target - self.current_angle) % 360
        if (
            angle_delta_cw < self._minimum_angle_delta
            or angle_delta_cw > 360 - self._minimum_angle_delta
        ):
            print(
                "ignoring target_angle {1} because of small delta small delta {0}".format(
                    angle_delta_cw, target
                )
            )
            return

        self._target_angle = target
        self.motor.reset_target_start_point()

    _minimum_position_delta_for_isr = 500

    _need_to_update_after_entry_isr = False
    _enter_position = 0
    _previous_enter_position = 0

    _need_to_update_after_exit_isr = False
    _exit_position = 0
    _previous_exit_position = 0

    _steps_per_rotation = 0
    _steps_across_home = 0
    _last_confirmed_noon_position = 0
    _position_has_been_determined = False

    def __init__(self, name, motor, sensor_pin, sensor_rising_is_enter):

        # name can be useful when debugging
        self._name = name

        # setup related bits
        self.motor = motor

        # setup input pins and attach interupt
        self._sensor_rising_is_enter = sensor_rising_is_enter
        self._sensor_pin = Pin(sensor_pin, mode=Pin.IN, pull=Pin.PULL_UP)
        self._sensor_pin.irq(self.isr_cb, Pin.IRQ_FALLING | Pin.IRQ_RISING)

    @property
    def current_angle(self):
        angle = self.full_angle_at_position(
            self.motor.position, self._last_confirmed_noon_position
        )
        return angle % 360

    def full_angle_at_position(self, position, noon_position):
        own_component = 0

        # get the angle component resulting from our own rotation
        steps_per_rot = self._steps_per_rotation
        try:
            delta_to_noon = position - noon_position
            own_component = delta_to_noon / steps_per_rot * 360
        except ZeroDivisionError:
            raise Exception("Cannot calculate current_angle as have not calibrated yet")

        return own_component

    @property
    def degrees_per_step(self):
        return 360 / self._steps_per_rotation

    @property
    def steps_cw_to_target(self):
        angle_delta_cw = (-self.current_angle + self.target_angle) % 360
        steps_delta = angle_delta_cw / self.degrees_per_step
        return int(steps_delta)

    @property
    def is_moving(self):
        return self.steps_cw_to_target > 0

    def move_to_target(self):
        steps_delta = self.steps_cw_to_target
        self.motor.set_target(steps_delta)
        self.motor.step()
        self.update_after_isr()

        if self.motor.position % 100 == 0:
            gc.collect()

    def run_to(self, target):

        self.wake()
        self.target_angle = target

        # print("run_to", target, "from", self.current_angle)

        while self.is_moving:
            self.move_to_target()

        self.sleep()

    def wake(self):
        self.motor.wake()
        self.update_after_isr()

    def sleep(self):
        self.motor.sleep()

    ##############

    def reset_calibration(self):
        # reset values

        # !! remember to reset the child as well, relies on these values

        self._need_to_update_after_entry_isr = False
        self._enter_position = 0
        self._previous_enter_position = 0

        self._need_to_update_after_exit_isr = False
        self._exit_position = 0
        self._previous_exit_position = 0

        self._steps_per_rotation = 0
        self._steps_across_home = 0

        self._last_confirmed_noon_position = 0
        self._position_has_been_determined = False
        self._drfit_has_been_determined = False

        self.motor.position = 0

    def fast_calibration(self, steps_per_rotation, steps_across_home):
        self.reset_calibration()
        self.move_until_calibrated(-1000)
        self.reset_calibration()
        self._steps_per_rotation = steps_per_rotation
        self._steps_across_home = steps_across_home

        self.move_until_calibrated()

    def calibrate(self):
        """Spin the motor until we have calibrated, and then stop at noon."""

        self.reset_calibration()

        self.move_until_calibrated()

    def move_until_calibrated(self, target=1_000_000):
        # Keep stepping until we are calibrated
        self.motor.set_target(target)
        while not self.has_been_calibrated and self.motor.is_moving:
            self.motor.step()
            self.update_after_isr()
            if self.motor.position % 100 == 0:
                gc.collect()

        # If we are calibrated move to noon
        if self.has_been_calibrated:
            self.run_to(0)

    @property
    def has_been_calibrated(self):
        return (
            self._steps_per_rotation
            and self._steps_across_home
            and self._position_has_been_determined
        )

    @property
    def drift_has_been_calibrated(self):
        return self._drfit_has_been_determined

    #############################

    def is_in_home_zone(self):
        return self._sensor_pin.value() == self._sensor_rising_is_enter

    def isr_cb(self, pin):

        # print("isr", self._name, pin.value(), self.motor.position)

        if pin.value() == self._sensor_rising_is_enter:
            self._need_to_update_after_entry_isr = True
            self._enter_position = self.motor.position
        else:
            self._need_to_update_after_exit_isr = True
            self._exit_position = self.motor.position

    def update_after_isr(self):
        if self._need_to_update_after_entry_isr:
            self.update_after_isr_entry()
            self._need_to_update_after_entry_isr = False

        if self._need_to_update_after_exit_isr:
            self.update_after_isr_exit()
            self._need_to_update_after_exit_isr = False

    def update_after_isr_entry(self):

        # Get the change in steps, reset last entry point to here
        previous_enter_pos = self._previous_enter_position
        self._previous_enter_position = self._enter_position

        # what is the delta? return if too small (means jitter, or running backwards)
        entry_delta = self._enter_position - previous_enter_pos
        if entry_delta < self._minimum_position_delta_for_isr:
            print(self._name, "ignoring small entry delta", entry_delta)
            return

        # entry to entry (full rotation)
        if previous_enter_pos:
            steps_per_rotation = self._enter_position - previous_enter_pos
            self._steps_per_rotation = steps_per_rotation

        # print(
        #     "{2} enter: {0} ({1})".format(
        #         self._enter_position, self._steps_per_rotation, self._name
        #     )
        # )

        self.update_after_isr_noon_position()

    def update_after_isr_noon_position(self):
        # If we also have _steps_across_home we can determine our position
        if self._steps_across_home:
            # Now calibrated, set angle correctly
            # Currently we have just entered home
            steps_to_home_center = self._steps_across_home / 2
            new_noon_position = int(self._enter_position + steps_to_home_center)

            # capture for logging below
            angle_previously_determined = self._position_has_been_determined
            angle_before_noon_update = (
                self.current_angle if angle_previously_determined else "???"
            )

            # store what we need to calculate angle
            self._last_confirmed_noon_position = new_noon_position
            self._position_has_been_determined = True

            if angle_previously_determined:
                print(
                    "{0} position adjusted: angle was {1}, now {2}".format(
                        self._name, angle_before_noon_update, self.current_angle
                    )
                )
            else:
                print(
                    "{0} position determined: angle now {1}".format(
                        self._name, self.current_angle
                    )
                )

    def update_after_isr_exit(self):
        # entry_to_exit

        # Get the change in steps, reset last entry point to here
        previous_exit_pos = self._previous_exit_position
        self._previous_exit_position = self._exit_position

        # what is the delta? return if too small (means jitter, or running backwards)
        entry_delta = self._exit_position - previous_exit_pos
        if entry_delta < self._minimum_position_delta_for_isr:
            print(self._name, "ignoring small entry delta", entry_delta)
            return

        if self._previous_enter_position:
            self._steps_across_home = (
                self._exit_position - self._previous_enter_position
            )

        # print(
        #     "{2} exit: {0} ({1})".format(
        #         self._exit_position, self._steps_across_home, self._name
        #     )
        # )


class PupilHand(Hand):
    def __init__(self):
        super().__init__(
            "pupil",
            PupilMotor(),
            config.pupil_stepper["sensor_pin"],
            config.pupil_stepper["sensor_rising_is_enter"],
        )


class GlintHand(Hand):

    def __init__(self):
        super().__init__(
            "glint",
            GlintMotor(),
            config.glint_stepper["sensor_pin"],
            config.glint_stepper["sensor_rising_is_enter"],
        )
