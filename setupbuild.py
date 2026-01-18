
import sys 
import os
import shutil


src_dir    = 'src'
header_dir = 'inc'

files_to_hide = ['gl_cmds','gl_gui','gl_render','gl_setup','gl_util']


print(" -> build process in python - begin")


def makepth(prefix, name, suffx):
    return prefix+'/'+name+suffx 


def takeaway_gui_src():
    print(" -> build process - hide GUI related files")
    for f in files_to_hide:
        print( makepth(src_dir   , f, '.cpp') ) 
        print( makepth(header_dir, f, '.h'  ) ) 

    #os.rename("path/to/current/file.foo", "path/to/new/destination/for/file.foo")
    #os.replace("path/to/current/file.foo", "path/to/new/destination/for/file.foo")
    #shutil.move("path/to/current/file.foo", "path/to/new/destination/for/file.foo")
    pass 

def putback_gui_src():
    print(" -> build process - return GUI related files")

    #os.rename("path/to/current/file.foo", "path/to/new/destination/for/file.foo")
    #os.replace("path/to/current/file.foo", "path/to/new/destination/for/file.foo")
    #shutil.move("path/to/current/file.foo", "path/to/new/destination/for/file.foo")
    pass 


takeaway_gui_src()

if __name__=="__main__":

    if len(sys.argv)>=3:
        arg2 = sys.argv[1] #.encode('utf-8', 'replace').decode()
        arg3 = sys.argv[2] #.encode('utf-8', 'replace').decode()



 
print(" -> build process in python - setup complete")