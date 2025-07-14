from motor import PupilMotor, GlintMotor

pupil_motor = PupilMotor()
# glint_motor = GlintMotor()

target = 2000

while pupil_motor.position < target:
    pupil_motor.step(target - pupil_motor.position)
