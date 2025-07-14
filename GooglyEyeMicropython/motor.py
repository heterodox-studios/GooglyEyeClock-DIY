import time

from machine import Pin


import config


class Motor:

    # never reset. This may be a bad idea, can cross that bridge when we come to it...
    position = 0
    target = 0
    start_from = 0

    # track when we can take next step
    next_step_us = 0

    # speed and accelerations
    max_speed = 800  # steps per second
    min_speed = 10
    acceleration_coefficient = 0.5
    acceptable_sleep_time_us = 400

    @property
    def max_interval_between_steps_us(self):
        return int(1 / self.min_speed * 1_000_000)

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

    def set_target(self, steps):
        self.target = self.position + int(steps)
        self.start_from = self.position

    @property
    def is_moving(self):
        return self.target != self.position

    def step(self, soft=True):

        steps = self.target - self.position
        if steps == 0:
            return False

        # Calculate the speed
        velocity = self.calculate_velocity(steps) if soft else self.max_speed
        speed = abs(velocity)
        dir = 1 if velocity > 0 else -1

        # are we too soon?
        now_us = time.ticks_us()
        us_until_next_step = time.ticks_diff(self.next_step_us, now_us)

        if us_until_next_step < 0:
            # we're due to run
            pass
        elif us_until_next_step > self.max_interval_between_steps_us:
            # timer is unreasonably long so we've looped
            pass
        elif us_until_next_step > self.acceptable_sleep_time_us:
            # too long to sleep, let other code run
            return False
        elif us_until_next_step > 0:
            # short enough interval - might as well just wait here
            time.sleep_us(us_until_next_step)

        # prep step (reset if needed) and take it
        self._step_index += dir
        self._step_index = self._step_index % self._step_count
        self.writeStepToPins()

        # At our current speed store when we should take our next step
        min_interval_us = int(1 / speed * 1_000_000)
        self.next_step_us = time.ticks_add(now_us, min_interval_us)

        # update our tracking of where we are
        self.position += dir
        return True

    def calculate_velocity(self, distance):
        dir = 1 if distance > 0 else -1

        # are we closer to start or end?
        shortest_dist = min(abs(self.position - self.start_from), abs(distance))

        # poor mans acceleration equations
        speed = shortest_dist / self.acceleration_coefficient

        # check that speed is within allowed norms
        speed = min(speed, self.max_speed)
        speed = max(speed, self.min_speed)

        return dir * speed

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
    def __init__(self):
        super().__init__(
            "glint",
            config.glint_stepper["pin1"],
            config.glint_stepper["pin2"],
            config.glint_stepper["pin3"],
            config.glint_stepper["pin4"],
        )
