import time

from machine import Pin

import config


class Motor:

    # never reset. This may be a bad idea, can cross that bridge when we come to it...
    position = 0

    # track when we took the last step
    previous_step_us = 0

    # speed and accelerations
    max_speed = 800  # steps per second

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

    def __init__(self, name, pin1, pin2, pin3, pin4):

        # name can be useful when debugging
        self._name = name

        # setup output pins
        self._pin1 = Pin(pin1, Pin.OUT)
        self._pin2 = Pin(pin2, Pin.OUT)
        self._pin3 = Pin(pin3, Pin.OUT)
        self._pin4 = Pin(pin4, Pin.OUT)

    def step(self, steps):

        if steps == 0:
            return False

        # which way to go
        dir = 1 if steps > 0 else -1

        # print("steps {0}".format(steps))

        # speed
        speed = self.max_speed

        # when is the earliest we should take next step?
        min_interval_us = 1 / speed * 1_000_000
        step_diff = time.ticks_diff(time.ticks_us(), self.previous_step_us)
        if step_diff < min_interval_us:
            # print("too soon: {0} < {1}".format(step_diff, min_interval_us))
            return False  # too soon to take the step

        # prep step (reset if needed) and take it
        self._step_index += dir
        self._step_index = self._step_index % self._step_count
        self.writeStepToPins()

        # update our tracking of where we are
        self.previous_step_us = time.ticks_us()
        self.position += dir
        return True

    def sleep(self):
        self.writeToPins(0, 0, 0, 0)

    def wake(self):
        self.writeStepToPins()

    def writeStepToPins(self):
        step = self._step_sequence[self._step_index]
        self.writeToPins(step[0], step[1], step[2], step[3])

    def writeToPins(self, p1, p2, p3, p4):
        self._pin1.value(p1)
        self._pin2.value(p2)
        self._pin3.value(p3)
        self._pin4.value(p4)


class PupilMotor(Motor):
    def __init__(self):
        super().__init__(
            "pupil",
            config.pupil_stepper["pin1"],
            config.pupil_stepper["pin2"],
            config.pupil_stepper["pin3"],
            config.pupil_stepper["pin4"],
        )


class GlintMotor(Motor):
    def __init__(self, pupil_motor):
        super().__init__(
            "glint",
            config.glint_stepper["pin1"],
            config.glint_stepper["pin2"],
            config.glint_stepper["pin3"],
            config.glint_stepper["pin4"],
        )
