
""" 

    REQUIRES:
        shapely 
        trimesh 
        networkx 
        rtree 

    TODO: 
        experiment: 
            cheapo waterline - get points near a plane (with dist threshold)  - then flatten them 
              - works better with dense data 
              - lidar, point cloud, 3d printer slicer data 
            

        from pyoctree import pyoctree as ot
        tree = ot.PyOctree(pointCoords,connectivity)


        MIS  - machine information system 
             - analyze GCODE and edit it afterb its generated 
               - extract by connected operations
               - TRS 
               - adjust laser pwm
               - slow SFM speed 
               - add/remove operation (coolant,etc)  
               - extract by generic Mcode 
               - find and replace generic 
               
               - extract by AXIS (height)
               - extract by geomtype 
               - extract by vector/line direction 
               - extract by proximity 
               - extract by tool 



        ADD DIRECTION VECTOR TO THE PATH (BETWEEN POINTS)
        ALA ENGRAVING AND BRUSH STROKE 


        TO FIX STUBBORN IMAGES:
            use the bmpinvert option 
            
            add a buffer of white around the image data?
            this makes sure polygons dont run into egdes
        
        add a script engine
           json files are "feed"
           json stroke info - hand drawn or machine learning , or whatever 


        establish end orientation and scale of vectors related to CNC 

"""



import numpy as np
from pathlib import Path


from pygfx.dag_ops import *
from pygfx.milling_ops import *
from pygfx.vector_ops import *




import rtree 
import networkx 


if GEOJSON_IS_LOADED:
    from geojson import dump 

    from geojson import Point as gjpt
    from geojson import Polygon as gjply
    from geojson import Feature as gjftr
    from geojson import LineString as gjln
    from geojson import FeatureCollection as gjfc
    from geojson import load as gjload

if SHAPELY_IS_LOADED:
    from shapely import buffer, BufferCapStyle, BufferJoinStyle
    from shapely import Point as shp_pt
    from shapely import Polygon as shp_ply
    from shapely import MultiPolygon as shp_mltply
    from shapely import LineString as shp_ln


##------------------------
# TOOLS 


from tools.imagecam import *
from tools.raytrace import *
from tools.milling import * 

from examples.obj_to_cnc import *
from examples.geometry import *
from examples.objects_2d import *
from examples.vector import *
from examples.render import *

from pygfx.doc_ops import *

#from unit_tests.ut_matrix import *


GLOBAL_PROJ = os.getcwd()


##---------------------------------


#make_rect(4, 4, GLOBAL_PROJ, 'quad')

#pcb_to_tesselated(GLOBAL_PROJ, 'foo.kicad_pcb')

#def tesselate_json(scale, numx, numy, numrots, seedpts, folder, injson, outjson):

#seedpts = [(-.1,.3,0), (.3,1,0), (.1,-.3,0), (-.1,-.3,0)]
#tesselate_json(1, 1, 1, 4, seedpts, GLOBAL_PROJ, '', 'outtessl')




"""
start  = (2 ,  0, 0)
ctrl1  = (.5,  .5, 0)
ctrl2  = (3, .5, 0)
end    = (1 ,  1, 0)
curve1 = [  start, ctrl1, ctrl2, end ]

obj = object3d() 
curvepts = obj.cubic_bezier(10, start, ctrl1, ctrl2, end )
obj.vec_connect_pts(pts=curvepts, draw_obj='rect_2d', axis='x', width=.01)
#lathe(GLOBAL_PROJ, 'foo.obj')
"""


#matrix_extrusion()

#iso_flat_render(GLOBAL_PROJ, [0,0,0], 'spacetime.obj', 'warpy')

#scale_fit_test(GLOBAL_PROJ, '4.json', 'astro')



# move_json(GLOBAL_PROJ, 'spacewarp')



#optimize_gcode(.3, GLOBAL_PROJ, '%s/images/2.json'%(GLOBAL_PROJ), outname='processed')



# test_plotter(GLOBAL_PROJ, '/images/0.json', 'erich0')



#test_streamline(GLOBAL_PROJ, 6,'spring.jpg' , dopass='all') 



#tesselator(GLOBAL_PROJ, 'phuk.json')



##---------------------------------

def np_testmult():
    pt1 = vec3(3,0,0)
    pt2 = vec3(-5,-1,2)

    m44 = matrix44()
    m44 = m44.align_vectors([0, 0, 1], pt2)
    #m44.from_euler([0,0,-90]) 

    #multiply matrix by vec3
    newpt = np.dot(m44[:3, :3], pt1.as_np)
    print(newpt)

    o = object3d()
    o.prim_arrow(axis='x')
    o.prim_arrow(axis='y')
    o.prim_arrow(axis='z')    
    o.prim_cube(size=.1, pos=pt1)
    o*m44
    #o.move(pt1[0], pt1[1], pt1[2])


    o.prim_cube(size=.1, pos=pt2, pivot='world')

    o.save('vek.obj')


#np_testmult()







##------------------------------------##






"""
cdc = cam_diskcache()
#cdc.load_cache(PROJPATH, 'head_cache.txt')
bbox = cdc.extent_fr_normal(PROJPATH, '3d_obj/head.obj')
print(bbox)
"""









##------------------------------------##

"""
mesh = trimesh.load_mesh('obj1.obj')
mesh.kdtree
mesh.points.kmeans
"""

#unique_edges = mesh.edges[trimesh.grouping.group_rows(mesh.edges_sorted, require_count=1)]







#toymesh(GLOBAL_PROJ, '/convex.obj')
#toymesh(GLOBAL_PROJ, '/obj1.obj')

#toymesh(GLOBAL_PROJ, '/3d_obj/sphere.obj')







##------------------------------------##
#https://blog.krum.io/k-d-trees/
class Node(object):
    def __init__(self):
        self.left = None
        self.right = None
        self.dim = None
        self.val = None

def build_kdtree(vectors, node, dimension, max_dimension):
    vals = sorted(vectors, key=lambda v: v[dimension])
    if len(vals) == 1:
        node.val = vals[0]
        return node
    if len(vals) == 2:
        node.val = vals[1]
        node.left = Node()
        node.left.val = vals[0]
        return node
    mid = int((len(vals) - 1) / 2)
    node.val = vals[mid]
    node.left = build_kdtree(vals[:mid], Node(), (dimension+1) % max_dimension, max_dimension)
    node.right = build_kdtree(vals[mid+1:], Node(), (dimension+1) % max_dimension, max_dimension)
    return node


##------------------------------------##
def test_scipy():
    #https://stackoverflow.com/questions/54114728/finding-nearest-neighbor-for-python-numpy-ndarray-in-3d-space
    #https://stackoverflow.com/questions/48312205/find-the-k-nearest-neighbours-of-a-point-in-3d-space-with-python-numpy

    from scipy.spatial import KDTree

    #build a KD tree to use an optimized search of neighbor points
    #data= np.random.rand(5,3)
    #sample= np.random.rand(3,3)

    obj = object3d()
    obj.prim_cube()

    obj2 = object3d()
    obj2.prim_triangle()
    #obj2.rotate(0,0,0)

    data  = np.array(obj.points)
    kdtree=KDTree(data)

    sample= np.array(obj2.points)

    dist,points=kdtree.query(sample)

    for p in dist:
        print(p)




"""

buffer(Point(10, 10), 2, quad_segs=1)
<POLYGON ((12 10, 10 8, 8 10, 10 12, 12 10))>

buffer(Point(10, 10), 2, quad_segs=2)
<POLYGON ((12 10, 11.414 8.586, 10 8, 8.586 8.586, 8 10, 8.5...>

buffer(Point(10, 10), -2, quad_segs=1)
<POLYGON EMPTY>

line = LineString([(10, 10), (20, 10)])

buffer(line, 2, cap_style="square")
<POLYGON ((20 12, 22 12, 22 8, 10 8, 8 8, 8 12, 20 12))>

buffer(line, 2, cap_style="flat")
<POLYGON ((20 12, 20 8, 10 8, 10 12, 20 12))>

buffer(line, 2, single_sided=True, cap_style="flat")
<POLYGON ((20 10, 10 10, 10 12, 20 12, 20 10))>

line2 = LineString([(10, 10), (20, 10), (20, 20)])

buffer(line2, 2, cap_style="flat", join_style="bevel")
<POLYGON ((18 12, 18 20, 22 20, 22 10, 20 8, 10 8, 10 12, 18 12))>

buffer(line2, 2, cap_style="flat", join_style="mitre")
<POLYGON ((18 12, 18 20, 22 20, 22 8, 10 8, 10 12, 18 12))>

buffer(line2, 2, cap_style="flat", join_style="mitre", mitre_limit=1)
<POLYGON ((18 12, 18 20, 22 20, 22 9.172, 20.828 8, 10 8, 10 12, 18 12))>

square = Polygon([(0, 0), (10, 0), (10, 10), (0, 10), (0, 0)])

buffer(square, 2, join_style="mitre")
<POLYGON ((-2 -2, -2 12, 12 12, 12 -2, -2 -2))>

buffer(square, -2, join_style="mitre")
<POLYGON ((2 2, 2 8, 8 8, 8 2, 2 2))>

buffer(square, -5, join_style="mitre")
<POLYGON EMPTY>

buffer(line, float("nan")) is None
True
"""

#cam.export_geojson_polygon(GLOBAL_PROJ, 'head.json')
#cam.export_geojson_lines(GLOBAL_PROJ, 'head.json')


#cam.triangulate()


#cam.cvt_obj3d_grpoly()
#cam.export_geojson_polygon(GLOBAL_PROJ, 'quad.json')














def tesselation_to_strokes():
    #regualr grid patterns can be used for "chuck close" brush strokes motion
    pass 



#load_gcode(GLOBAL_PROJ, '/sneaker_net/OLD/sprok', 'sprok_style')
#load_gcode(GLOBAL_PROJ, '/sneaker_net/OLD/sprok', 'face75')
#load_gcode(GLOBAL_PROJ, '/sneaker_net/OLD/sprok', 'sprok_teeth')
#load_gcode(GLOBAL_PROJ, '/sneaker_net/OLD/sprok', 'sprok_bolts')



#break_into_many_ngc('XXXX')



#test_streamline(GLOBAL_PROJ, 3,'girl.png' , dopass='all') 


##-------------------------##
def test_slicer():
    kam = cam_op()
    kam.load('../pulser/3d_obj/head_vn.obj')
    #kam.show()
    p = kam.slice_example('../pulser/3d_obj/', 'head.obj', [0,1])
    print(p)

# def slice_example( self, path, infile, heights, scan_axis ='z', reindex=True):
# def tm_meshplane_test(self, origin, normal, path, infile):
# def batch_slicer( self, path , infile, heights, scan_axis ='z'):

##-------------------------##

def test_slicer2():
    kam = cam_op()
    #kam.load('../pulser/3d_obj/head_vn.obj')
    #kam.show()
    #kam.batch_slicer( path , infile, heights, scan_axis ='z'):
    p = kam.batch_slicer( '../pulser/3d_obj/', 'head_vn.obj', [0,1], scan_axis ='z')





##-------------------------##

def its_alive(): 
    vflo = vectorflow()
    #vflo.load_geojson('images/out/4.json')
    vflo.load_geojson('new.json')
    #vflo.gl_move_center()
    #vflo.export_geojson_polygon(GLOBAL_PROJ,'centered')

    vflo.cvt_grpoly_obj3d()
    vflo.rotate_pts(rot=[90,0,0])
    vflo.save('l0.obj', as_lines=True)

 

its_alive()



# vflo = vectorflow()
# vflo.load('objects/monkey.obj')
# vflo.save('flatball.obj', as_lines=True)
# #vflo.export_geojson_lines('foo.json')




#vflo.export_machine_size(GLOBAL_PROJ, 'cuttylaser') 


#gn_dir_type(vectorflow)


#json_to_ngc('XXX', 'astro',.5, 4)



########################

# d = normalize(d);
# double yaw = Math.Atan2(d.X, d.Y);
# double pitch = Math.Atan2(d.Z, Math.Sqrt((d.X * d.X) + (d.Y * d.Y)));
# p.rotateXYZ(pitch, yaw, 0); //Roll == 0



#look into - TOPMOD  , intanced self connecting geom 

def escher_ball(): 
    o = object3d()
    pts= o.calc_icosahedron(pos=(0,0,0), rot=(0,0,0), size=1) 

    o2 = object3d()  
    o2.prim_circle(pos=(0,0,1),axis='z', dia=.5,spokes=50)
    #o2.points.append((0,0,0))

    #ptgrp = o2.get_pt_grp()  

    cntr = vec3(0,0,0)
    up = vec3(1,0,0)

    for pt in pts:
        v2 = vec3(pt[0],pt[1],pt[2])

        v2 = v2.normal
        
        #yaw = math.atan2(v2.x, v2.y);
        #pitch = math.atan2(v2.z, math.sqrt((v2.x * v2.x) + (v2.y * v2.y)));
        #rotated_m33 = matrix33()
        #m = rotated_m33.np_rotate( up , (yaw,pitch,0)) 

        #--
        rotated_m33 = matrix33()
        
        print( v2.to_euler() ) 
        
        #m = rotated_m33.np_rotate( up ,  ) 


        #out = cntr-v2
        #o.one_vec_to_obj(out, pos=pt)
        
        # apply the matrix to the points in the model 
        #rotated_points = o.apply_matrix_ptgrp(ptgrp, m33=m) 
        #tmp = o.apply_matrix_pts(o2.points, m33=m)

        #rotated_points =[]
        #for xx in tmp:
        #    npt= ( xx[0]+pt[0], xx[1]+pt[1], xx[2]+pt[2] ) 
        #    rotated_points.append(npt) 




        
        #THIS IS THE DAMN POINT GROUP CURSE - ZERO INDEX BLOWING UP 
        #o.insert_pt_grp(rotated_points)
        
        #o.points.extend(tmp)


    o.save('ballz.obj')










##---------------------------------

def circle_cutter(outfilename, radius):
    vflo = vectorflow()
    cutdepth = False

    # holes = [ [dia, spokes, (x,y,z)/(x,y) ] ] 
    holes = [ (1  , 12, (0,0,0)),
              (.7 , 8, (1,1)),
              (.1 , 3, (3,3)),
              (.1 , 3, (4,4,0))              
             ]
    
    vflo.hole_cutter_2d(.5, holes=holes)

    if cutdepth: 
        vflo.rh = 4    
        vflo.cdpi = .05 
        vflo._make_outline(5)

    #vflo._calculate_paths3d(do_retracts=False)
    #vflo._calculate_paths3d(do_retracts=True)
    #vflo.gl_translate(1,1,1)

    #def export_ngc(self, rh, ch, cdpi, cmax, filename, do3d=False):
    vflo.export_ngc(1, 0, .1, 2, '%s/%s.ngc'%(GLOBAL_PROJ, outfilename) , do3d=True, do_retracts=True)







 



def kmeans(): 
    from sklearn.cluster import KMeans
    from sklearn.datasets import make_blobs
    import matplotlib.pyplot as plt
    #from itertools import cycle
    
    filepath = 'PUT/PATH/HERE'

    #from sklearn.cluster import AffinityPropagation
    #from sklearn import metrics

    vflo = vectorflow()
    vflo.load_geojson('%s/out/0.json')%(filepath)
    # vflo.load_geojson('%s/out/1.json')%(filepath)
    # vflo.load_geojson('%s/out/2.json')%(filepath)
    # vflo.load_geojson('%s/out/3.json')%(filepath)
    # vflo.load_geojson('%s/out/4.json')%(filepath)

    vflo.gr_sort = vflo.filter_by_bbox(2, 2, underover='smaller')
    vflo.gl_scale(.2)
    #vflo.gl_rotate((0,0,45))
    #vflo.export_geojson_lines(GLOBAL_PROJ, 'filtered.json', periodic=False)
    #vflo.export_ngc(1, 0, .1, 2, '%s/%s.ngc'%(GLOBAL_PROJ, 'escher'), do3d=False, do_retracts=False, do_laser=True, laserpwm=350)

    centers = []
    for ply in vflo.gr_sort:
        centers.append(ply[1]) 

    X,Y = make_blobs(
        n_samples=300, centers=centers, cluster_std=100.5, random_state=0
    )

    #plt.close("all")
    plt.figure(0)
    plt.grid(True)
    print(X)
    print(Y)
    
    #plt.scatter(X[:,0], Y[:,1])
    #plt.clf()

    #for pt in centers:
    #    plt.plot([pt[0], pt[1]])

    plt.show()









 


##----------------------------------------------------
#math examples
##----------------------------------------------------

##----------------------------------------------------
#vector examples
##----------------------------------------------------

##----------------------------------------------------
#2d object examples
##----------------------------------------------------


##----------------------------------------------------
#3d object examples
##----------------------------------------------------


##----------------------------------------------------
#polygon selection examples
##----------------------------------------------------

#extract_by_copy_hack("3d_obj/output.obj")

#slice_extract_and_makenew("3d_obj/output.obj")

#test_subsel_point_transform("3d_obj/output.obj")

#select_polygons_spatially( "3d_obj/output.obj", (2, 1, -4) , 4.5)



##----------------------------------------------------
#render selection examples
##----------------------------------------------------

"""

def fill_polygon_3d(pointgen)
    pass 

def fill_polygon_2d(pointgen)
    pass 

"""






def loadkicad(infilepath, outfilename):
    kip = vectorflow()
    
    #kip.load_kicadpcb(infilepath)
    #kip._make_periodic()
    
    #def load_geojson(self, inputfile, zaxis, getfids=None, getids=None):
    kip.load_geojson('%s/images/out/%s.json'%(GLOBAL_PROJ, infilepath)   )
 
    # _set_cam_properties( rh, ch, cdpi, cmax):
    #kip._set_cam_properties(2, 1, .1, 5)
    
    #kip.show()

    #kip._make_outline(7)

    #kip.gl_move_center()   
    #kip.gl_rotate( (45,0,0) )

    #kip._calculate_paths3d(do_retracts=False)
    kip._calculate_paths3d(do_retracts=True)

    #def export_ngc(self, rh, ch, cdpi, cmax, filename, do3d=False):
    kip.export_ngc(1, 0, .1, 2, '%s/images/out/%s.ngc'%(GLOBAL_PROJ, outfilename) , do3d=True)


#loadkicad('XXXX', 'kicadgrply')




def test_gears(outfilename): 
    gearz = gear_generator()
    pts =  gearz.build(1, 2,.5, 12) 
    gearz.linegeom_fr_points(pts, color=(100,0,100), periodic=False )
    gearz.save( '%s/images/out/%s.obj'%(GLOBAL_PROJ, outfilename) )







##----------------------------------------------------
##   /usr/local/opt/python@3.10/bin/python3.10 ./imagecam.py  

"""
recipe for finding gradients 
run firstpassbw -> get commonbands -> these are your regions 
run firstpass -> common bands on each region in color  


"""



##----------------------------------------------------
##----------------------------------------------------
## (iteration , blur , contrast, bright, scaling(divs) , in, out )

#color version 
#firstpass(5, .5, 1.1, 1.1, 250, "images/forest.jpg", "images/out", "output")


#black and white version 
#firstpass_bw(10, 1.2, 1.5, 1, 250, "images/mount.jpg", "images/out", "output")

#no bright 
#firstpass(4, 1.2, 1.2, None, 250, "images/in/image.png", "images/out", "output")
#with bright
#firstpass(4, 1.2, 1.2, 1.1, 250, "images/in/deer.jpg", "images/out", "output")


##----------------------------------------------------
##----------------------------------------------------
##   /usr/local/opt/python@3.10/bin/python3.10 ./imagecam.py  

#secondpass("images/out/output_0.bmp", "images/out" , 6, False)

#secondpass("images/in/foo.jpg", "images/out" , 4, False)
#secondpass("images/out/output_0.bmp", "images/out" , 10, False)
#secondpass("images/in/robot.png", "images/out" , 12, False)

##----------------------------------------------------
##----------------------------------------------------
#set the RGB values from last tool and run this 
#thirdpass( "images/out/commonbands.png", "images/out", "dxf" )


#thirdpass( "images/out/commonbands.png",  "images/out" , "geojson", bmpinvert=True, po_invert=False)
#thirdpass( "images/out/commonbands.png",  "images/out" , "geojson", bmpinvert=False, po_invert=False)


#thirdpass( "images/out/commonbands.png",  "images/out" , "geojson", po_invert=False)


#GEOJSON
#thirdpass( "images/out/commonbands.png",  "images/out" , "geojson", True, False, False)

#DXF
#thirdpass( "images/out/commonbands.png",  "images/out" , "dxf", True, False, False)

#NO VECTORS (COMIC STYLE IS ALL RASTER) 
#thirdpass( "images/out/commonbands.png",  "images/out" , None, True, False, False)

##----------------------------------------------------
##----------------------------------------------------
 
#comic_pass( infolder, names, outfolder, colorbg_file, lineweight, bluramt, brightamt, blacklines=True ):

#comic_pass( "images/out", ['0','1','2','3','4'], 'images/out', 'commonbands.png', 1, 2, None, True)


#comic_pass( "images/out", ['0','1','2','3','4','5','6','7','8'], 'images/out', 'commonbands.png', 10, 1.1, 1.1, True)

#comic_pass( "images/out", ['0','1','2','3','4'], 'images/out', 'commonbands.png', 10, None, 1, True)

##---

# def redraw(  outfolder, colorbg_file, lineweight, bluramt, brightamt, blacklines=True ):
#redraw( 'images/out', 'output_1.bmp', 1, 1, 1.1, True)

#redraw( 'images/out', 'commonbands.png', 1, 1, 1.3, True)


##----------------------------------------------------
##----------------------------------------------------

##----------------------------------------------------
##----------------------------------------------------



#scan_line_tool( 20, 'trapz.obj', GLOBAL_PROJ, 'scanz')

#vflo.load_geojson('%s/images/out/0.json'%GLOBAL_PROJ,getfids=getids)

#vflo.export_geojson_lines(GLOBAL_PROJ,'polyfoo.json')

#vflo.cvt_grpoly_obj3d()
#vflo.save('polyfoo.obj')


 



#prim_example(GLOBAL_PROJ, 'foo3d')

#spline_to_cnc(GLOBAL_PROJ, 'foo3d')

#scratch_obj_to_cnc()



##----------------------------------------------------












