import time, gc
from hand import PupilHand


pupil_hand = PupilHand()

pupil_hand.fake_calibration(steps_per_rotation=14257, steps_across_home=586)


time.sleep(1)

pupil_hand.target_angle = 90
while pupil_hand.is_moving:
    pupil_hand.move_to_target()
