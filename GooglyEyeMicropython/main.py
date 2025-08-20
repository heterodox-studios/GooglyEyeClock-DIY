import time, gc
from hand import PupilHand, GlintHand
from display import Display
from clock import Clock

pupil_hand = PupilHand()
glint_hand = GlintHand()
display = Display(pupil_hand, glint_hand)
clock = Clock()

if True:
    pupil_hand.calibrate()
    glint_hand.calibrate()
else:
    pupil_hand.fast_calibration(steps_per_rotation=22_205, steps_across_home=608)
    glint_hand.fast_calibration(steps_per_rotation=11_426, steps_across_home=300)

while True:
    display.show_datetime(clock.now())
    time.sleep(60)
