#!/usr/bin/python


import lcm
import sys
import time
import math

LCMROOT='/home/auv/git/acfr-lcm'

sys.path.append('{}/build/lib/python2.7/dist-packages/perls/lcmtypes'.format(LCMROOT))

from acfrlcm import auv_control_t

lc = lcm.LCM();

msg = auv_control_t()
msg.depth_mode = auv_control_t.DEPTH_MODE
msg.run_mode = auv_control_t.RUN

# initial 'zero' settings
msg.altitude = float(0.0)
msg.pitch = float(0.0)
msg.vx = float(2.0)
msg.heading = float(5)

# send a null depth (so the plots later don't have a sawtooth)
msg.utime = int(time.time() * 1000000)
msg.depth = float(0.0)
time.sleep(0.5)
lc.publish('NGA.AUV_CONTROL', msg.encode())

for i in xrange(120):
    msg.depth = float(0.0)

    msg.utime = int(time.time() * 1000000)
    time.sleep(0.5)

    lc.publish('NGA.AUV_CONTROL', msg.encode())
    print "Sent dive command"


msg.utime = int(time.time() * 1000000)
msg.run_mode = auv_control_t.STOP
lc.publish('AUV_CONTROL', msg.encode())
print "Sent STOP command"
