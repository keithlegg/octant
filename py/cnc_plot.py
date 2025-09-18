


from cnc_math import *

#from cnc_pt_op import *








class cnc_plot(object):
    def __init__(self):
        self.head_pos     = [0,0,0]
        self.next_pt      = [0,0,0]

        self.linear_unit  = 'inch'
        self.angular_unit = 'rad'

        #pulses per linear unit 
        self.pp_lux       = 100
        self.pp_luy       = 100
        self.pp_luz       = 100
        #self.pp_lui      = 10
        #self.pp_luj      = 10        

    ##-------------------------------------------##
    def locate_pt_along3d(self, fpos, spos, num):
        """
            given two 3D points, return a series of N number connecting points in 3D 

            usage:

        """
        pts_created = []
         
        for n in range(num):
            npos = [3]

            npos[0]     = spos[0]+(((fpos[0]-spos[0])/(num+1))*(n+1))
            npos.append(  spos[1]+(((fpos[1]-spos[1])/(num+1))*(n+1))  )
            npos.append(  spos[2]+(((fpos[2]-spos[2])/(num+1))*(n+1))  )

            pts_created.append( vec3(npos[0], npos[1], npos[2]) )
        return pts_created

    ##-------------------------------------------##
    def calc_3d_pulses(self, fr_pt, to_pt):
        """ 
            No idea is this is right, and probably not the best way to do this. 
            This converts a 3D vector into a series of pulses to drive the 3 (or more?) axis servos 
        """

        mag = ( fr_pt.between(to_pt).length )

        delta_x = fr_pt[0]-to_pt[0]
        delta_y = fr_pt[1]-to_pt[1]
        delta_z = fr_pt[2]-to_pt[2]
        #print(delta_x, delta_y, delta_z)
      
        num_pul_x = abs(delta_x)*self.pp_lux
        num_pul_y = abs(delta_y)*self.pp_luy
        num_pul_z = abs(delta_z)*self.pp_luz

        #print(num_pul_x, num_pul_y, num_pul_z)

        #get the highest number of pulses to calculate 
        tmp = [num_pul_x, num_pul_y, num_pul_z]
        sort_puls = sorted(tmp, reverse=True)
        most = sort_puls[0]
        
        #get the smallest division to use as granularity 
        if mag!=0 and most !=0:
            gran = (mag/most)
        else:
            gran = 0

        #calculate a series of points along vector for each axis 
        x_pts = self.locate_pt_along3d(to_pt, fr_pt, int(num_pul_x))
        y_pts = self.locate_pt_along3d(to_pt, fr_pt, int(num_pul_y))
        z_pts = self.locate_pt_along3d(to_pt, fr_pt, int(num_pul_z))


        DEBUGMODE = 0

        pulsetrain = []
        # build a sampleset of all points along the vector - then iterate and match each axis to those points
        # converting the spatial points into a pulse train 
        if most!=0 and gran!=0:
            if DEBUGMODE:
                print('### most possible samples', most)
                print(mag, most, gran, tmp)

            thresh = gran/2 
            samples = self.locate_pt_along3d(to_pt, fr_pt, int(most))
            for spt in samples:
                xp=0;yp=0;zp=0;

                for xpt in x_pts:
                    if( (xpt-spt).length<thresh):
                        #print((xpt-spt).length)
                        xp=1;break
                for ypt in y_pts:
                    if( (ypt-spt).length<thresh ):
                        yp=1;break
                for zpt in z_pts:
                    if( (zpt-spt).length<thresh ):
                        zp=1;break

                pulsetrain.append([xp,yp,zp])
                if not DEBUGMODE:
                    pulsetrain.append([0 ,0 ,0])
        
        if DEBUGMODE:
            xct=0;yct=0;zct=0
            for p in pulsetrain:
                if p[0]==1:
                    xct+=1
                if p[1]==1:
                    yct+=1
                if p[2]==1:
                    zct+=1                                        
            print(xct, yct, zct)


        if not DEBUGMODE:
            for p in pulsetrain:
                print(p)

#---------------------------------#
"""
plot = cnc_plot()
x = vec3(0 ,0 ,0  )
y = vec3(7 ,.1 ,15  )

#test the function 
pts = plot.locate_pt_along3d(x,y,5)
for pt in pts:
    print(pt)

#p = plot.calc_3d_pulses(x,y)
"""





 