#!/usr/bin/env python3

import sys 
import shutil






CMD_PATH = '/home/keith/linuxcnc/nc_files'

CMD_FILES = ['testport','rotate_head','home_axis']



print('installing binaries to : %s'%CMD_PATH )


for cmd in CMD_FILES:
    print('installing %s'%cmd)
    shutil.move('%s/%s'%(cmd,cmd), CMD_PATH)


