from motor import PupilMotor, GlintMotor

pupil_motor = PupilMotor()
glint_motor = GlintMotor(pupil_motor)

# speed things up at start during dev
if True:

    glint_motor._steps_per_rotation = 14825
    glint_motor._steps_across_home = 367
    glint_motor._degrees_per_step = 360 / glint_motor._steps_per_rotation

    glint_motor.take_steps(-1000)
    glint_motor._previous_enter_position = 0
    glint_motor.position = 0

    pupil_motor._steps_per_rotation = 14272
    pupil_motor._steps_across_home = 580
    pupil_motor._degrees_per_step = 360 / pupil_motor._steps_per_rotation

    pupil_motor.take_steps(-1000)
    pupil_motor._previous_enter_position = 0
    pupil_motor.position = 0


# glint_motor.calibrate()
# glint_motor.goto_noon()
glint_motor.calibrate_drift()

# glint_motor.goto_noon()
# while True:
#     glint_motor.step_to_target_angle(0)
#     glint_motor.step_to_target_angle(180)

pupil_motor.sleep()
glint_motor.sleep()
