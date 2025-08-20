import time, gc
from hand import PupilHand, GlintHand


pupil_hand = PupilHand()
glint_hand = GlintHand(parent=pupil_hand)

pupil_hand.fast_calibration(steps_per_rotation=14_260, steps_across_home=590)

glint_hand.calibrate()

# glint_hand.fast_calibration(
#     steps_per_rotation=14_780, steps_across_home=370, parent_drift_factor=1.4
# )

anch cha
def goto(hand, angle):
    time.sleep(1)
    print("goto", angle, "from", hand.current_angle)
    hand.target_angle = angle

    hand.wake()

    while hand.is_moving:
        hand.move_to_target()

    hand.sleep()


# goto(glint_hand, 180)
# goto(glint_hand, 90)


# for _ in (glint_hand, pupil_hand):
#     print(_._name, _.current_angle)

# goto(pupil_hand, 90)
# for _ in (glint_hand, pupil_hand):
#     print(_._name, _.current_angle)

# goto(glint_hand, 180)
# for _ in (glint_hand, pupil_hand):
#     print(_._name, _.current_angle)

# # set up for next test run
# time.sleep(10)
# goto(pupil_hand, 350)
# goto(glint_hand, 350)
# for _ in (glint_hand, pupil_hand):
#     print(_._name, _.current_angle)


# goto(pupil_hand, 15)
# goto(pupil_hand, 15.1)
# goto(pupil_hand, 14.9)

# goto(pupil_hand, 3 * 360 / 12 + 0 * 360 / 12 / 60)  # 03:00
# goto(pupil_hand, 3 * 360 / 12 + 1 * 360 / 12 / 60)  # 03:01

# for _ in [x / 10.0 for x in range(3520, 3540, 1)]:
#     goto(pupil_hand, _)
