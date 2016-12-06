#!/usr/bin/python
#
# Urwid BigText example program
#    Copyright (C) 2004-2009  Ian Ward
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# Urwid web site: http://excess.org/urwid/

"""
Urwid example demonstrating use of the BigText widget.
"""

import sys
from knittykitty.knittykitty import KnittyKittyLL
from knittykitty.knitcode import *

import select

#
serial_tty = sys.argv[1]
drv = KnittyKittyLL(serial_tty)
#drv.reset()

#
direction = [
    "Unknown",
    "Left",
    "Right"
]

ctype = [
    "unknown",
    "Knit",
    "Lace"
]

belt_shift = [
    "Unknown",
    "Regular",
    "Shifted"
]
cstate = [
    "KK_INIT",
    "KK_IDLE",
    "KK_SEND_SYNC",
    "KK_RECV_SYNC",
    "KK_TEST_SENSORS",
    "KK_TEST_SOL1",
    "KK_TEST_SOL2",
    "KK_TEST_LC",
    "KK_CALIBRATE",
    "KK_KNIT",
    "KK_KNIT_L",
    "KK_KNIT_R",
    "KK_KNITLACE",
    "KK_KNITLACE_L",
    "KK_KNITLACE_R"
]
##127

def print_there(x, y, text):
     sys.stdout.write("\x1b7\x1b[%d;%df%s\x1b8" % (x, y, " "*80))
     sys.stdout.write("\x1b7\x1b[%d;%df%s\x1b8" % (x, y, text))
     sys.stdout.flush()
for x in xrange(100):
    print_there(x,0, " ")

def pollkb():
    i,o,e = select.select([sys.stdin],[],[],0.0001)
    for s in i:
        if s == sys.stdin:
            input = sys.stdin.readline()
            return input
    return None

#drv.enter_sol_test2()

kc_file = open(sys.argv[2])
content = kc_file.readlines()
#print content
code, warnings, errors = compile_knitcode(''.join(content))

code = list(reversed(code))

while 1:
    u = drv.get_update()
    print_there(0,0, "Carriage position: %d" % (u.carriage_position))
    print_there(2,0, "Direction: %s" % direction[u.carriage_direction]) #[u.carriage_direction])
    print_there(3,0, "Carriage Type: %s" % ctype[u.carriage_type]) #[u.carriage_direction])
    print_there(4,0, "Belt Shift: %s" % belt_shift[u.belt_shift]) #[u.carriage_direction])
    print_there(5,0, "State: %s" % cstate[u.state]) #[u.carriage_direction])
    print_there(7,0, "Left: %s" % u.eol_left) #[u.carriage_direction])
    print_there(8,0, "Right: %s" % u.eol_right) #[u.carriage_direction])


    schange = pollkb()
    if schange is None:
        pass
    elif schange[0] == "1":
        print "Changing to Sensor Test"
        drv.enter_sensor_test()
    elif schange[0] == "2":
        print "Changing to Sol Test 1"
        drv.enter_sol_test1()
    elif schange[0] == "3":
        print "Changing to Sol Test 2"
        drv.enter_sol_test2()
    elif schange[0] == "4":
        print "Changing to Sol Test 2"
        drv.knitline(50, 150, [],[])
    elif schange[0] == "5":
        print "Changing to Sol Test 2"
        drv.knitlaceline(50, 150, [],[])

    elif schange[0] == "r":
        print "Resetting "
        drv.reset()
    elif len(code) > 0 and cstate[u.state] == "KK_IDLE" and schange[0] == 'n':
            _, k = code.pop()
            drv.write(k.bytes())
            print "swapping"



drv.close()
