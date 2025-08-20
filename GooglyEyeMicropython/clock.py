import machine


class Clock:

    def __init__(self) -> None:
        rtc = machine.RTC()
        rtc.datetime(
            # year, month, day, weekday, hours, minutes, seconds, subseconds
            (2020, 1, 1, 0, 6, 30, 0, 0)
        )
        print(rtc.datetime())
        self.rtc = rtc

    def now(self):
        return self.rtc.datetime()
