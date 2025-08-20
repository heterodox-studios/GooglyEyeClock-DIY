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


def goto(hand, angle):
    time.sleep(1)
    print("goto", angle, "from", hand.current_angle)
    hand.target_angle = angle

    hand.wake()

    while hand.is_moving:
        hand.move_to_target()

    hand.sleep()


for _ in (90, 180, 270, 360):
    goto(pupil_hand, _)
    goto(glint_hand, _)
