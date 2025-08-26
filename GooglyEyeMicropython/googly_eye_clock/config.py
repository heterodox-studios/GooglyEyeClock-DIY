# pupil motor pin definitions
pupil_stepper = {
    "pin1": 10,
    "pin2": 11,
    "pin3": 12,
    "pin4": 13,
    "sensor_pin": 14,
    "sensor_rising_is_enter": True,
    "steps_per_rotation": 22_205,
    "steps_across_home": 608,
}

# glint motor pin definitions
glint_stepper = {
    "pin1": 9,
    "pin2": 8,
    "pin3": 7,
    "pin4": 6,
    "sensor_pin": 15,
    "sensor_rising_is_enter": True,
    "steps_per_rotation": 11_426,
    "steps_across_home": 300,
}

# RTC pin definitions
clock = {
    "hour_increment_pin": 21,
    "minute_increment_pin": 20,
    "debounce_time_ms": 20,
}
