import time
from machine import Pin, Timer

from . import config


class Button:

    def __init__(self, pin_number, press_handler):

        pin = Pin(pin_number, Pin.IN, Pin.PULL_UP)
        pin.irq(
            trigger=Pin.IRQ_FALLING | Pin.IRQ_RISING, handler=self._pin_interupt_handler
        )
        self._pin = pin
        self._last_state = pin.value()

        self._timer = Timer(-1)

        self.press_handler = press_handler

    def _pin_interupt_handler(self, pin):
        # (re)start a timer to wait out level changed
        self._timer.deinit()
        self._timer.init(
            mode=Timer.ONE_SHOT,
            period=config.clock["debounce_time_ms"],
            callback=self._timer_interupt_handler,
        )

    def _timer_interupt_handler(self, t):
        # If here button has been pressed and stable for a little while

        state = self._pin.value()

        if state != self._last_state:

            # State has changed, record it
            self._last_state = state

            # If state has changed to low call handler
            if state == False:
                self.press_handler()
