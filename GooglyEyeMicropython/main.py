import time, gc
from hand import PupilHand, GlintHand
from display import Display


pupil_hand = PupilHand()
glint_hand = GlintHand()
display = Display(pupil_hand, glint_hand)

if False:
    pupil_hand.calibrate()
    glint_hand.calibrate()
else:
    pupil_hand.fast_calibration(steps_per_rotation=22_205, steps_across_home=608)
    glint_hand.fast_calibration(steps_per_rotation=11_426, steps_across_home=300)

for m in range(0, 121, 1):
    display.show(m / 60, m)
    time.sleep(1)
