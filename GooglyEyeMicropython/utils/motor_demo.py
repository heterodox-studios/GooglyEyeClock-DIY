import time, gc, sys

sys.path.append("..")

from googly_eye_clock.motor import PupilMotor, GlintMotor

pupil_motor = PupilMotor()
glint_motor = GlintMotor()

if True:
    pupil_motor.set_target(15000 / 60 / 12)
    glint_motor.set_target(15000 / 60)

    # pupil_motor.set_target(15000)
    # glint_motor.set_target(15000)

    counter = 0

    while pupil_motor.is_moving or glint_motor.is_moving:
        pupil_motor.step(soft=False)
        glint_motor.step(soft=False)

        # Need to run the garbage collector often to prevent memory bog downs
        if counter % 100 == 0:
            gc.collect()
            pass
        counter += 1


pupil_motor.sleep()
glint_motor.sleep()
