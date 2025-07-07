from motor import Motor
from time import sleep_us

glint_motor = Motor("glint", 6, 7, 8, 9)
pupil_motor = Motor("pupil", 10, 11, 12, 13)

while True:
    glint_motor.step()
    pupil_motor.step()
    sleep_us(1000)
