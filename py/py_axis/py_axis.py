#!/usr/bin/python3


"""
Inspired by this:

https://github.com/kentavv/linuxcnc_vxi11
"""

import sys
import time
import datetime

import linuxcnc


cnc_s = linuxcnc.stat()
cnc_c = linuxcnc.command()


# to calculate homed, we iterate over the axes, finding those that are present on this machine,
# and logically combining their homed state (for LinuxCNC 2.7)
def ok_for_mdi27():
  cnc_s.poll()
  homed = True

  #THIS NEEDS WORK - LOOK UP PROPER KEYS 
  for axis in cnc_s.axis:
      #homed = homed and ((not axis['enabled']) or (axis['homed'] != 0))
      print(axis)

  return not cnc_s.estop and cnc_s.enabled and homed and (cnc_s.interp_state == linuxcnc.INTERP_IDLE)


def verify_ok_for_mdi():
  if not ok_for_mdi27():
    print('Not ready for MDI commands')
    sys.exit(1)


verify_ok_for_mdi()


cnc_c.mode(linuxcnc.MODE_MDI) 
cnc_c.wait_complete()


def move_to(x, y, z):
  cmd = 'G1 G54 X{0:f} Y{1:f} Z{2:f} f5'.format(x, y, z)
  print('Command, %s'%cmd)
  verify_ok_for_mdi()

  cnc_c.mdi(cmd)
  rv = cnc_c.wait_complete(60)
  if rv != 1:
    print('MDI command timed out')
    sys.exit(1)



move_to(-1,1,1)
move_to(1,0,0)
move_to(0,1,0)


