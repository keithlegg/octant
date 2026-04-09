#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import linuxcnc

try:
    s = linuxcnc.stat() # create a connection to the status channel
    s.poll() # get current values

except linuxcnc.error:
    sys.exit(1)

for x in dir(s):
    if not x.startswith("_"):
        print(x, getattr(s,x))



print("Joint 1 homed: ", s.joint[1]["homed"])
