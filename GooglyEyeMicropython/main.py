import time, gc
from hand import PupilHand


pupil_hand = PupilHand()

pupil_hand.fake_calibration(steps_per_rotation=14_260, steps_across_home=590)


def goto(angle):
    pupil_hand.target_angle = angle
    while pupil_hand.is_moving:
        pupil_hand.move_to_target()
    time.sleep(1)


goto(90)
goto(10)
