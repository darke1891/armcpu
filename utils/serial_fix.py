import  serial
import os


PARITY_NONE = serial.PARITY_NONE

def Serial(*args, **kargs):
    DEVICE = '/dev/ttyUSB0'
    if os.getenv('DEVICE'):
        DEVICE = os.getenv('DEVICE')
    ser = serial.Serial(DEVICE, 115200,
        stopbits=2, parity=serial.PARITY_NONE)#, timeout=1
    ser.setStopbits(1)
    ser.setStopbits(2) # wired problem, net reset serial port
    return ser
