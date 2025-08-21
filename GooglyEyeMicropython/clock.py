from machine import Pin, RTC
import config
import math
import time
from button import Button


class Clock:

    _debounce_last_seen = {
        "hour": 0,
        "minute": 0,
    }

    def __init__(self) -> None:
        # set up the RTC
        rtc = RTC()

        # when raspberry pi is powered on the datetime tuple appears to be
        (2021, 1, 1, 4, 0, 0, 4, 0)

        print(rtc.datetime())
        self.rtc = rtc

        self.hour_button = Button(
            config.clock["hour_increment_pin"], self._increment_hour
        )
        self.minute_button = Button(
            config.clock["minute_increment_pin"], self._increment_minute
        )

    def now(self):
        return self.rtc.datetime()

    def _increment_hour(self):
        print("Hour increment pin pressed")
        self._increment_time(hour=1)

    def _increment_minute(self):
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
