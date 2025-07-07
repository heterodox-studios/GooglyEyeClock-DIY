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
)

pupil_motor = Motor(
    "pupil",
    config.pupil_stepper["pin1"],
    config.pupil_stepper["pin2"],
    config.pupil_stepper["pin3"],
    config.pupil_stepper["pin4"],
)

pupil_sensor_pin = Pin(config.pupil_light_gate_pin, mode=Pin.IN, pull=Pin.PULL_UP)

pupil_enter_position = 0
last_pupil_enter_position = pupil_enter_position

pupil_exit_position = 0
last_pupil_exit_position = pupil_exit_position


def pupil_cb(pin):

    global pupil_enter_position
    global pupil_exit_position

    if pin.value():
        pupil_enter_position = pupil_motor.position
    else:
        pupil_exit_position = pupil_motor.position


pupil_sensor_pin.irq(pupil_cb, Pin.IRQ_FALLING | Pin.IRQ_RISING)

while True:
    pupil_motor.step()
    sleep_us(1000)

    if pupil_enter_position != last_pupil_enter_position:
        delta = pupil_enter_position - last_pupil_enter_position
        last_pupil_enter_position = pupil_enter_position
        print("enter: {0} ({1})".format(pupil_enter_position, delta))

    if pupil_exit_position != last_pupil_exit_position:
        delta = pupil_exit_position - last_pupil_exit_position
        last_pupil_exit_position = pupil_exit_position
        print("exit: {0} ({1})".format(pupil_exit_position, delta))
