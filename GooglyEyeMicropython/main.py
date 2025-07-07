import config
from machine import Pin
from motor import Motor
from time import sleep_us

glint_motor = Motor(
    "glint",
    config.glint_stepper["pin1"],
    config.glint_stepper["pin2"],
    config.glint_stepper["pin3"],
    config.glint_stepper["pin4"],
    config.glint_hall_effect_pin,
)

pupil_motor = Motor(
    "pupil",
    config.pupil_stepper["pin1"],
    config.pupil_stepper["pin2"],
    config.pupil_stepper["pin3"],
    config.pupil_stepper["pin4"],
    config.pupil_light_gate_pin,
)


while True:
    pupil_motor.step()
    pupil_motor.update_after_isr()
    glint_motor.step()
    glint_motor.update_after_isr()
    sleep_us(1000)
