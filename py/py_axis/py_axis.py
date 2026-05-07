#!/usr/bin/python3


"""
Inspired by this:

https://github.com/kentavv/linuxcnc_vxi11
"""

import sys
import time
import datetime

 
import linuxcnc

from os import listdir
from os.path import isfile, join

showcmd = True

TIMEOUT = 200

##--##--##--##--##--##--##--## 
##--##--##--##--##--##--##--## 



# to calculate homed, we iterate over the axes, finding those that are present on this machine,
# and logically combining their homed state (for LinuxCNC 2.7)
def ok_for_mdi27():
  cnc_s.poll()
  homed = True

  # #THIS NEEDS WORK - LOOK UP PROPER KEYS 
  # for axis in cnc_s.axis:
  #     #homed = homed and ((not axis['enabled']) or (axis['homed'] != 0))
  #     print(axis)

  return not cnc_s.estop and cnc_s.enabled and homed and (cnc_s.interp_state == linuxcnc.INTERP_IDLE)


def verify_ok_for_mdi():
  if not ok_for_mdi27():
    print('Not ready for MDI commands')
    sys.exit(1)



def clean_num(instr):
    out = instr 
    out = out.replace('(','')
    out = out.replace(')','')
    out = out.replace(' ','')
    out = out.replace(',','')

    return float(out)


##--##--##--##--##--##--##--## 
##--##--##--##--##--##--##--## 
def _runcmd(cmd):
    verify_ok_for_mdi()

    cnc_c.mdi(cmd)
    rv = cnc_c.wait_complete(TIMEOUT)
    if rv != 1:
      print('MDI command timed out')
      sys.exit(1)




def rpd_move_to(x, y, z):
    cmd = 'G0 G54 X{0:f} Y{1:f} Z{2:f} f5'.format(x, y, z)
    if showcmd:
        print('CMD: %s'%cmd)
    _runcmd(cmd)

def move_to(x, y, z):
    cmd = 'G1 G54 X{0:f} Y{1:f} Z{2:f} f5'.format(x, y, z)
    if showcmd:
        print('CMD: %s'%cmd)
    _runcmd(cmd)


def digi_out(pin, val):
    if val==0:
        cmd = 'M65 P%s'%pin
        #cmd = 'M63 P%s'%pin
    if val==1:
        cmd = 'M64 P%s'%pin
        #cmd = 'M62 P%s'%pin

    if showcmd:
        print('CMD: %s'%cmd)
    _runcmd(cmd)




##--##--##--##--##--##--##--## 
##--##--##--##--##--##--##--## 

"""
lets assume each polygon is a text file of coordinates
a "program" is a folder of polygons 
"""
def run_poly(filename):
    f = open( filename,"r", encoding='utf-8')
    contents = f.readlines()
    for i,x in enumerate(contents):
        tok = x.split(" ")
        xc = clean_num(tok[0]) 
        yc = clean_num(tok[1])
        zc = clean_num(tok[2])
        
        if i==0:
            digi_out(1,1)
            rpd_move_to(xc,yc,zc)
        else:  
            digi_out(1,0)
            move_to(xc,yc,zc)

def run_project(folder):
    polyfiles = [f for f in listdir(folder) if isfile(join(folder, f))]
     
    for i,f in enumerate(polyfiles):
        print("###### NEW POLYGON %s"%i)  
        run_poly("%s/%s"%(folder,f))



cnc_s = linuxcnc.stat()
cnc_c = linuxcnc.command()

verify_ok_for_mdi()
cnc_c.mode(linuxcnc.MODE_MDI) 
cnc_c.wait_complete()


def test_digi(iters):
  for a in range(iters):
    digi_out(0, 0)
    digi_out(1, 0)
    digi_out(2, 0)
    digi_out(3, 0)
    digi_out(0, 1)
    digi_out(1, 1)
    digi_out(2, 1)
    digi_out(3, 1)


###########################

run_poly("../new_proj/extents.txt")
 
#run_project('../new_proj')



