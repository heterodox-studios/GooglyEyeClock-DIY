import config
from motor import Motor
from time import sleep_us

glint_motor = Motor(
    "glint",
    config.glint_stepper["pin1"],
    config.glint_stepper["pin2"],
    config.glint_stepper["pin3"],
    config.glint_stepper["pin4"],
)

pupil_motor = Motor(
    "pupil",
    config.pupil_stepper["pin1"],
    config.pupil_stepper["pin2"],
    config.pupil_stepper["pin3"],
    config.pupil_stepper["pin4"],
)

while True:
    glint_motor.step()
    pupil_motor.step()
    sleep_us(1000)
