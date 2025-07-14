import time
from motor import PupilMotor, GlintMotor

pupil_motor = PupilMotor()
glint_motor = GlintMotor()

if True:
    pupil_motor.move(15000 / 60 / 12)
    glint_motor.move(15000 / 60)
    # pupil_motor.move(15000)
    # glint_motor.move(15000)

    while pupil_motor.is_moving or glint_motor.is_moving:
        pupil_motor.step()
        glint_motor.step()

    # time.sleep(1)

pupil_motor.sleep()
glint_motor.sleep()
