from machine import Pin, RTC, I2C
from ds3231_gen import DS3231
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
        ds3231 = DS3231(I2C(0))

        # when raspberry pi is powered on the datetime tuple appears to be
        (2021, 1, 1, 4, 0, 0, 4, 0)

        print(rtc.datetime())
        print(ds3231.get_time())

        self.rtc = rtc
        self.ds3231 = ds3231
        self.set_rtc_from_ds3231()

        self.hour_button = Button(
            config.clock["hour_increment_pin"], self._increment_hour
        )
        self.minute_button = Button(
            config.clock["minute_increment_pin"], self._increment_minute
        )

    def set_rtc_from_ds3231(self):
        YY, MM, DD, hh, mm, ss, wday, _ = self.ds3231.get_time()
        self.rtc.datetime((YY, MM, DD, wday, hh, mm, ss, 0))

    def set_ds3231_from_rtc(self):
        dt = self.rtc.datetime()
        dt_tuple = time.localtime(time.mktime(dt))  # Populate weekday field
        self.ds3231.set_time(dt_tuple)

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
        self.set_ds3231_from_rtc()
        print("Time updated:")
        print("rtc   ", self.rtc.datetime())
        print("ds3231", self.ds3231.get_time())
