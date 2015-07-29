import  serial

PARITY_NONE = serial.PARITY_NONE

def Serial(*args, **kargs):
    DEVICE = '/dev/ttyUSB0'
    ser = serial.Serial(DEVICE, 115200,
        stopbits=2, parity=serial.PARITY_NONE)#, timeout=1
    ser.setStopbits(1)
    ser.setStopbits(2) # wired problem, net reset serial port
    return ser
