#!/usr/bin/env python3

import os 
import shutil
import subprocess






CMD_PATH = '/home/keith/linuxcnc/nc_files'

CMD_FILES = ['testport','rotate_head','home_axis']



print('installing binaries to : %s'%CMD_PATH )

##---------------
# remove old files if they exist
for file in CMD_FILES:
    file_path = '%s/%s'%(CMD_PATH,file)
    if os.path.exists(file_path):
        os.remove(file_path)
        print(f"{file_path} has been deleted.")
    else:
        print(f"{file_path} does not exist.")
    

##---------------
for cmd in CMD_FILES:
    print('installing %s'%cmd)
    shutil.move('%s/%s'%(cmd,cmd), CMD_PATH)

    #set the capability operation (so you dont need sudo to access parport)
    result = subprocess.run(['setcap','cap_sys_rawio+ep', '%s/%s'%(CMD_PATH,cmd)])



