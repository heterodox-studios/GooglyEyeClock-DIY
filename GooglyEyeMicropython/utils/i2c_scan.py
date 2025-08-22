import machine

# i2c = machine.I2C(0, scl=5, sda=4)
i2c = machine.I2C(0)

devices = i2c.scan()

if len(devices) != 0:
    print("Number of I2C devices found=", len(devices))
    for device in devices:
        print("Device Hexadecimel Address= ", hex(device))
else:
    print("No device found")
