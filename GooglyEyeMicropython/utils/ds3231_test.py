# Example of SirFrico to get the right time in the Pi Pico, derived from a DS3231
# Firstly print the time on pico startup, then get and print the time from the ds3231
# April 28 2025

from time import sleep, localtime
from machine import RTC, I2C, Pin
from ds3231_gen import DS3231

rtc = RTC()

# print time on pico startup
# local time format (year,month,day,hour,min,sec,weekday,yearday)
print("ds3231_test.py")
print(localtime())

# time from ds3231 via peter hinch class
# first set the pico i2c pins and i2c bus that have been used to connect to the DS3231
i2c = I2C(0)

# instanciate the DS3231 class (as found in ds3231_gen.py)
drtc = DS3231(i2c)

# get the time from the ds3231
tm = drtc.get_time()
# rtc.datetime args are (year, month, day, weekday, hours, minutes, seconds, subseconds)
# set the time on the pico
rtc.datetime((tm[0], tm[1], tm[2], tm[6] + 1, tm[3], tm[4], tm[5], 0))

# print time after setting pico RTC
print(localtime())
