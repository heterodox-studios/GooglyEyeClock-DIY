from machine import Pin, RTC
import config
import math


class Clock:

    def __init__(self) -> None:
        # set up the RTC
        rtc = RTC()
        rtc.datetime(
            # year, month, day, weekday, hours, minutes, seconds, subseconds
            (2020, 1, 1, 0, 6, 30, 0, 0)
        )
        print(rtc.datetime())
        self.rtc = rtc

        # setup interupts
        self._setup_interupts()

    def now(self):
        return self.rtc.datetime()

    def _debug_pins(self):
        # debug the pins
        print(self.hour_increment_pin.value(), self.minute_increment_pin.value())

    def _setup_interupts(self):
        # setup the hour increment pin
        self.hour_increment_pin = Pin(
            config.rtc["hour_increment_pin"], Pin.IN, Pin.PULL_UP
        )
        self.hour_increment_pin.irq(
            trigger=Pin.IRQ_FALLING, handler=self._increment_hour
        )

        # setup the minute increment pin
        self.minute_increment_pin = Pin(
            config.rtc["minute_increment_pin"], Pin.IN, Pin.PULL_UP
        )
        self.minute_increment_pin.irq(
            trigger=Pin.IRQ_FALLING, handler=self._increment_minute
        )

    def _increment_hour(self, pin):
        print("Hour increment pin pressed")
        self._increment_time(hour=1)

    def _increment_minute(self, pin):
        print("Minute increment pin pressed")
        self._increment_time(minute=1)

    def _increment_time(self, hour=0, minute=0):
        # Increment the time by the specified hours and minutes
        dt = self.rtc.datetime()
        dt = list(dt)

        # increase as needed
        new_hour = dt[4] + hour
        new_minute = dt[5] + minute

        # handle overflow of minutes
        new_hour = new_hour + math.floor(new_minute / 60)

        # update values, ignoring overflows
        dt[4] = new_hour % 12
        dt[5] = new_minute % 60
        dt[6] = 0  # zero seconds

        # update the RTC with the new time
        self.rtc.datetime(tuple(dt))
        print("Time updated:", self.rtc.datetime())
