import math


class Display:

    def __init__(self, hour_hand, minute_hand):
        self.hour_hand = hour_hand
        self.minute_hand = minute_hand

    def show_datetime(self, dt):
        # year, month, day, weekday, hours, minutes, seconds, subseconds
        print(dt)
        return self.show_hms(dt[4], dt[5])

    def show_hms(self, hh=0, mm=0, ss=0):

        # check we are within bounds
        hh = int(hh) % 12
        mm = int(mm) % 60
        ss = int(ss) % 60

        print(hh, mm, ss)

        # calculate fractional times
        frac_mm = mm + ss / 60
        frac_hh = hh + frac_mm / 60

        print(frac_hh, frac_mm)

        # calculate the angle for each hand
        second_angle = 0
        minute_angle = 360 / 60 * frac_mm
        hour_angle = 360 / 12 * frac_hh

        self.hour_hand.target_angle = hour_angle
        self.minute_hand.target_angle = minute_angle

        self.hour_hand.wake()
        self.minute_hand.wake()

        while self.hour_hand.is_moving or self.minute_hand.is_moving:
            self.hour_hand.move_to_target()
            self.minute_hand.move_to_target()

        self.hour_hand.sleep()
        self.minute_hand.sleep()
