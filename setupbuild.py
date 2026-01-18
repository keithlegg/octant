#!/usr/bin/env python

import sys 
import os
import shutil


#  pass path from make? 
#base_dir = sys.argv[1]

src_dir    = 'src'
header_dir = 'inc'

holding_dir = 'keep_me'

files_to_process = ['gl_cmds','gl_gui','gl_render','gl_setup','gl_util']

base_dir = "%s/pulser"%os.getcwd()

print(" -> build process in python - begin in dir %s"%base_dir )



#assemble a list of all the source code files we want to work with
def getfiles(ext):
    paths = []
    for f in files_to_process:
        paths.append( '%s%s'%(f, ext)) 
    return paths


#move all the files to a place outside of makes path (GUI disabled)
def takeaway_gui_src():
    print(" -> build process - hide GUI related files")
    for p in getfiles('.h'):
        cmd1 = "%s/%s/%s"%(base_dir,header_dir,p) 
        cmd2 = "%s/%s/%s"%(base_dir,holding_dir,p)
        #print(cmd1, cmd2)
        shutil.move(cmd1, cmd2)

    # for p in getfiles('.cpp'):
    #     command = "%s/%s"%(src_dir,p), "%s/%s"%(holding_dir,p)
    #     #os.rename()
    #     print(command)


#put all the files back to where they live (GUI enabled)
def putback_gui_src():
    print(" -> build process - return GUI related files")

    for p in getfiles('.h'):
        cmd1 = "%s/%s/%s"%(base_dir,holding_dir,p)
        cmd2 = "%s/%s/%s"%(base_dir,header_dir,p) 
        #print(cmd1, cmd2)
        shutil.move(cmd1, cmd2)

    # for p in getfiles('.cpp'):
    #     command = "%s/%s"%(src_dir,p), "%s/%s"%(base_dir,p)
    #     #os.rename()
    #     print(command)





if __name__=="__main__":

    if len(sys.argv)>1:
        arg2 = sys.argv[1] #.encode('utf-8', 'replace').decode()
        
        print("ARG IS " , arg2)  
        
        if arg2 == 'gui': 
            putback_gui_src()

        if arg2 == 'nogui': 
            takeaway_gui_src()

 
print(" -> build process in python - setup complete")