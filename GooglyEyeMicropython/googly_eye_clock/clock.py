import math
import time

from machine import Pin, RTC, I2C

from ds3231_gen import DS3231

from . import config, Button


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
        # (2021, 1, 1, 4, 0, 0, 4, 0)

        self.rtc = rtc
        self.ds3231 = ds3231

        self.hour_button = Button(
            config.clock["hour_increment_pin"], self._increment_hour
        )
        self.minute_button = Button(
            config.clock["minute_increment_pin"], self._increment_minute
        )

    def set_rtc_from_ds3231(self):
        # print("before set_rtc_from_ds3231")
        # print("rtc   ", self.hms_from_rtc())
        # print("ds3231", self.hms_from_ds3231())

        YY, MM, DD, hh, mm, ss, wday, _ = self.ds3231.get_time()
        self.rtc.datetime((YY, MM, DD, wday, hh, mm, ss, 0))

        # print("after set_rtc_from_ds3231")
        # print("rtc   ", self.hms_from_rtc())
        # print("ds3231", self.hms_from_ds3231())

    def set_ds3231_from_rtc(self):
        YY, MM, DD, wday, hh, mm, ss, subseconds = self.rtc.datetime()
        self.ds3231.set_time((YY, MM, DD, hh, mm, ss, wday, subseconds))

    def hms(self) -> tuple[int, int, int]:
        return self.hms_from_rtc()

    def hms_from_rtc(self) -> tuple[int, int, int]:
        YY, MM, DD, wday, hh, mm, ss, subseconds = self.rtc.datetime()
        return (hh, mm, ss)

    def hms_to_rtc(self, hh: int, mm: int, ss: int) -> None:
        # (YY, MM, DD, wday, hh, mm, ss, subsec)
        dt = (2025, 1, 1, 1, hh, mm, ss, 0)
        self.rtc.datetime(dt)

    def hms_from_ds3231(self) -> tuple[int, int, int]:
        YY, MM, DD, hh, mm, ss, wday, _ = self.ds3231.get_time()
        return (hh, mm, ss)

    def _increment_hour(self):
        print("Hour increment pin pressed")
        self._increment_time(hour=1)

    def _increment_minute(self):
        print("Minute increment pin pressed")
        self._increment_time(minute=1)

    def _increment_time(self, hour=0, minute=0):
        # Increment the time by the specified hours and minutes

        hh, mm, ss = self.hms_from_rtc()

        # increase as needed
        new_hour = hh + hour
        new_minute = mm + minute

        # handle overflow of minutes
        new_hour = new_hour + math.floor(new_minute / 60)

        # update values, ignoring overflows
        hh = new_hour % 12
        mm = new_minute % 60
        ss = 0  # zero seconds

        # print("new time", hh, mm, ss)

        # update the RTC with the new time
        self.hms_to_rtc(hh, mm, ss)
        self.set_ds3231_from_rtc()
        print("Time updated:")
        print("rtc   ", self.hms_from_rtc())
        print("ds3231", self.hms_from_ds3231())
