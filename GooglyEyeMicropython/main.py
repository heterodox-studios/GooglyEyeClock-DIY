import time, gc
from hand import PupilHand, GlintHand


pupil_hand = PupilHand()
glint_hand = GlintHand()

if False:
    pupil_hand.calibrate()
    glint_hand.calibrate()
else:
    pupil_hand.fast_calibration(steps_per_rotation=22_205, steps_across_home=608)
    glint_hand.fast_calibration(steps_per_rotation=11_426, steps_across_home=300)


def goto(p_angle, g_angle):

    time.sleep(1)

    pupil_hand.target_angle = p_angle
    glint_hand.target_angle = g_angle

    pupil_hand.wake()
    glint_hand.wake()

    while pupil_hand.is_moving or glint_hand.is_moving:
        pupil_hand.move_to_target()
        glint_hand.move_to_target()

    pupil_hand.sleep()
    glint_hand.sleep()


for _ in (90, 180, 270, 360):
    goto(_, _)
