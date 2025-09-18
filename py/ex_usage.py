
import subprocess
import sys
import time 

from cnc_math import *
from cnc_pt_op import *





pop3 = pop3d()





def run_pulser(ptlist):
    #inputFileName = sys.argv[1];
    #outputFileName = sys.argv[2];


    #debug not done yet 
    #runcmd = '../home_axes/homer')
    #subprocess.call(runcmd , shell=True)


    for i,pt in enumerate(ptlist):
        if i>0:
            f = ptlist[i-1]
            s = ptlist[i]
            
            #time.sleep(.01)

            runcmd = '../pulser/pulser %s %s %s %s %s %s %s'%(f[0],f[1],f[2], s[0],s[1],s[2], 50)
            subprocess.call(runcmd , shell=True)




pts = pop3.calc_circle( pos=(0,0,0), rot=(0,0,0), dia=1, axis='z', periodic=True, spokes=20, start=0, end=360)
run_pulser( pts )


