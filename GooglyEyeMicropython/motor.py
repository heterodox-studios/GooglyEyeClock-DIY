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
    min_speed = 2
    max_interval_between_steps_us = int(1 * 1_000_000)
    # acceleration = 100  # steps per sec per sec
    # stopping_distance = 3200  # FIXME - calculate from equation d = v^2/(2*a)

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

    def move(self, steps):
        self.target = self.position + int(steps)
        self.start_from = self.position

    @property
    def is_moving(self):
        return self.target != self.position

    def step(self):

        steps = self.target - self.position
        if steps == 0:  # FIXME - buggy for decel to reverse direction
            return False

        # are we too soon?
        now_us = time.ticks_us()
        us_until_next_step = time.ticks_diff(self.next_step_us, now_us)
        if (
            us_until_next_step > 0
            and us_until_next_step < self.max_interval_between_steps_us
        ):
            # print(
            #     "too soon: {0} > 0  from  time.ticks_diff({1}, {2})".format(
            #         us_until_next_step, self.next_step_us, now_us
            #     )
            # )
            # return False  # too soon to take the step
            print("sleeping for {0} us".format(us_until_next_step))
            time.sleep_us(us_until_next_step)

        # speed
        velocity = self.calculate_velocity(steps)
        speed = abs(velocity)
        dir = 1 if velocity > 0 else -1

        # prep step (reset if needed) and take it
        self._step_index += dir
        self._step_index = self._step_index % self._step_count
        self.writeStepToPins()

        # update our tracking of where we are
        min_interval_us = int(1 / speed * 1_000_000)
        self.next_step_us = time.ticks_add(now_us, min_interval_us)
        self.position += dir
        return True

    def calculate_velocity(self, distance):
        dir = 1 if distance > 0 else -1

        # poor mans acceleration equations
        time_factor = 0.5
        dist_from_start = abs(self.position - self.start_from)
        speed_from_start = dist_from_start / time_factor
        speed_to_end = abs(distance) / time_factor

        # check that speed is within allowed norms
        speed = min(speed_from_start, speed_to_end, self.max_speed)
        speed = max(speed, self.min_speed)

        # print(distance, speed)

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
