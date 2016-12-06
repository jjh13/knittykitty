import sys
import os
import os.path
import string

from array import *
import struct

import serial
    #
    # KK_OP_NOP = 0,
    # KK_OP_RESET,
    # KK_OP_TEST_SENSOR,
    # KK_OP_SOL_TEST1,
    # KK_OP_SOL_TEST2,
    # KK_OP_LC_TEST,
    #
class UpdatePacket:
    def __init__(self, serial):
        self.size = 16
        self.ser = serial

    def read(self):
        sig = 0
        self.ser.flush()
        self.ser.flushInput()
        while sig != 0x4D454F57:
            data = self.ser.read(1)[0]
            if ord(data) != 0x57:
                continue
            data = self.ser.read(3)
            sig = struct.unpack("<I", "\0" + data)[0] | 0x57
        data = self.ser.read(self.size - 4)

        self.carriage_position     = struct.unpack("<h", data[0:2])[0]
        self.carriage_type         = ord(struct.unpack("<c", data[2:3])[0])
        self.carriage_direction    = ord(struct.unpack("<c", data[3:4])[0])
        self.belt_shift            = ord(struct.unpack("<c", data[4:5])[0])
        self.state                 = ord(struct.unpack("<c", data[5:6])[0])
        self.eol_left              = struct.unpack("<H", data[6:8])[0]
        self.eol_right             = struct.unpack("<H", data[8:10])[0]
        self.ver_maj               = ord(struct.unpack("<c", data[10:11])[0])
        self.ver_min               = ord(struct.unpack("<c", data[11:12])[0])


        # print self.carriage_position
        # print self.carriage_type
        # print self.carriage_direction
        # print self.belt_shift
        # print self.state
        # print self.eol_left
        # print self.eol_right
        # print self.ver_maj
        # print self.ver_min

    def get_version(self):
        return (self.ver_maj, self.ver_min)

    def get_sensor_values(self):
        return (self.eol_left, self.eol_right)

    def get_carriage_position(self):
        return self.carriate_position

class KnittyKittyLL:
    def __init__(self, serial_port):
        self.ser = serial.Serial(port=serial_port, baudrate=115200, stopbits=1, timeout=1, xonxoff=0, rtscts=0, dsrdtr=0)
#            self.ser.setRTS(True)
        if self.ser == None:
            print 'Unable to open serial device %s' % serial_port
            raise IOError

        self.upack = UpdatePacket(self.ser)

    def get_update(self):
        self.upack.read()
        return self.upack

    def enter_sensor_test(self):
        self.upack.read()
        self.ser.write(bytearray([2]))

    def enter_sol_test1(self):
        self.upack.read()
        self.ser.write(bytearray([3]))

    def enter_sol_test2(self):
        self.upack.read()
        self.ser.write(bytearray([4]))

    def knitline(self, start, end, l, r):
        self.ser.write(bytearray([6] + [50] + [150] + [0xF0]*((400/8)) ))
        self.ser.read()

    def knitlaceline(self, start, end, l, r):
        self.ser.write(bytearray([7] + [50] + [150] + [0xF0]*((400/8)) ))
        self.ser.read()

    def send_preamble(self):
        pass

    def reset(self):
        self.upack.read()
        self.ser.write(bytearray([1]))
        pass
        
    def write(self, data):
        self.ser.write(data)

    def close(self):
        self.ser.close()
        pass
