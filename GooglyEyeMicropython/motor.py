from machine import Pin


class Motor:

    position = 0

    _enter_position = 0
    _last_enter_position = 0
    _exit_position = 0
    _last_exit_position = 0

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

    def __init__(self, name, pin1, pin2, pin3, pin4, sensor_pin):

        # name can be useful when debugging
        self._name = name

        # setup output pins
        self._pin1 = Pin(pin1, Pin.OUT)
        self._pin2 = Pin(pin2, Pin.OUT)
        self._pin3 = Pin(pin3, Pin.OUT)
        self._pin4 = Pin(pin4, Pin.OUT)

        # setup input pins and attach interupt
        self._sensor_pin = Pin(sensor_pin, mode=Pin.IN, pull=Pin.PULL_UP)
        self._sensor_pin.irq(self.isr_cb, Pin.IRQ_FALLING | Pin.IRQ_RISING)

    def step(self):

        # Always take a forwards step
        dir = 1

        self._step_index += dir

        if self._step_index >= self._step_count:
            self._step_index = 0

        self.writeStepToPins()
        self.position += dir

    def update_after_isr(self):
        if self._enter_position != self._last_enter_position:
            delta = self._enter_position - self._last_enter_position
            self._last_enter_position = self._enter_position
            print(
                "{2} enter: {0} ({1})".format(self._enter_position, delta, self._name)
            )

        if self._exit_position != self._last_exit_position:
            delta = self._exit_position - self._last_exit_position
            self._last_exit_position = self._exit_position
            print("{2} exit: {0} ({1})".format(self._exit_position, delta, self._name))

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

    def isr_cb(self, pin):
        if pin.value():
            self.isr_enter()
        else:
            self.isr_exit()

    def isr_enter(self):
        self._enter_position = self.position

    def isr_exit(self):
        self._exit_position = self.position
