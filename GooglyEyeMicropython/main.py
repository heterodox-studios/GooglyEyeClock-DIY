import time, gc
from googly_eye_clock import PupilHand, GlintHand, Display, Clock

pupil_hand = PupilHand()
glint_hand = GlintHand()
display = Display(pupil_hand, glint_hand)

clock = Clock()
clock.set_rtc_from_ds3231()

if False:
    pupil_hand.calibrate()
    glint_hand.calibrate()
else:
    pupil_hand.fast_calibration(steps_per_rotation=22_205, steps_across_home=608)
    glint_hand.fast_calibration(steps_per_rotation=11_426, steps_across_home=300)

while True:
    hh, mm, ss = clock.hms()
    display.show_hms(hh, mm, 0)
    time.sleep(1)
