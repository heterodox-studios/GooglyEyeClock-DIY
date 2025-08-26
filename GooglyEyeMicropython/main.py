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
    pupil_hand.fast_calibration()
    glint_hand.fast_calibration()

while True:
    hh, mm, ss = clock.hms()
    display.show_hms(hh, mm, 0)
    time.sleep(1)
