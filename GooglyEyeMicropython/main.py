from motor import PupilMotor, GlintMotor

pupil_motor = PupilMotor()
glint_motor = GlintMotor(pupil_motor)

# pupil_motor.calibrate()
glint_motor.calibrate()
