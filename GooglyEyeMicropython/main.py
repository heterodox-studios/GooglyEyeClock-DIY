import time, gc
from hand import PupilHand


pupil_hand = PupilHand()

pupil_hand.fake_calibration(steps_per_rotation=14_260, steps_across_home=590)


def goto(angle):
    time.sleep(1)
    print("goto", angle, "from", pupil_hand.current_angle)
    pupil_hand.target_angle = angle

    pupil_hand.motor.wake()

    while pupil_hand.is_moving:
        pupil_hand.move_to_target()

    pupil_hand.motor.sleep()


goto(15)
goto(15.1)
goto(14.9)

goto(3 * 360 / 12 + 0 * 360 / 12 / 60)  # 03:00
goto(3 * 360 / 12 + 1 * 360 / 12 / 60)  # 03:01

for _ in [x / 10.0 for x in range(3520, 3540, 1)]:
    goto(_)
