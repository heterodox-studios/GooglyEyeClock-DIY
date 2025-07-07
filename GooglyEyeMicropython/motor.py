import machine


class Motor:

    position = 0

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

        # setup pins
        self._pin1 = machine.Pin(pin1, machine.Pin.OUT)
        self._pin2 = machine.Pin(pin2, machine.Pin.OUT)
        self._pin3 = machine.Pin(pin3, machine.Pin.OUT)
        self._pin4 = machine.Pin(pin4, machine.Pin.OUT)

    def step(self):

        # Always take a forwards step
        dir = 1

        self._step_index += dir

        if self._step_index >= self._step_count:
            self._step_index = 0

        self.writeStepToPins()
        self.position += dir

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
