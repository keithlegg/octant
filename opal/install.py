#!/usr/bin/env python3


import os 
import shutil
import subprocess

from pathlib import Path



#source path for shell/python files to link to the executables 
MCODE_SOURCE = 'cmd_init'
#name of the scripts to install (copy)
MCODE_FILES = ['M101', 'M102']

#source path for binaries to move over 
CMD_FILES = ['set_byte', 'set_pin', 'flash_pin' ]
#name of the binaries to install (copy) 
CMD_PATH = '/home/keith/linuxcnc/nc_files'







 
##---------------
# Check if destination exists AND is a directory

dest_path = Path(CMD_PATH)

if dest_path.is_dir():
    pass
else:
    print("Install location '%s' does not exist."%CMD_PATH)
    exit()

print('installing binaries to : %s'%CMD_PATH )



##---------------
# remove old files if they exist
for file in CMD_FILES:
    file_path = '%s/%s'%(CMD_PATH,file)
    if os.path.exists(file_path):
        os.remove(file_path)
        print(f" ... {file_path} has been deleted.")
    else:
        print(f" ... {file_path} does not exist.")
    



##---------------
#copy the binaries to the path 

for cmd in CMD_FILES:
    print(' ... installing %s binary '%cmd)
    shutil.copy('%s/%s'%(cmd,cmd), CMD_PATH)

    # set the capability operation (so you dont need sudo to access parport)
    result = subprocess.run(['setcap','cap_sys_rawio+ep', '%s/%s'%(CMD_PATH,cmd)])

 


##---------------
#now install the shell scripts 

print('installing mcode files to : %s'%CMD_PATH )

for mf in MCODE_FILES:
    src_file = '%s/%s'%(MCODE_SOURCE,mf)
    print(' ... installing %s mcode file'%src_file)

    shutil.copy(src_file, CMD_PATH)



