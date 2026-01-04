import numpy as np


from chopper import GLOBAL_PROJ

from chopper.pygfx.dag_ops import *
from chopper.pygfx.milling_ops import *
from chopper.pygfx.vector_ops import *

##------------------------
# CORE MODULES

"""

from chopper.pygfx.grid_ops import *
from chopper.pygfx.kicad_ops import *
from chopper.pygfx.math_ops import *
from chopper.pygfx.neighbors import *
from chopper.pygfx.obj2d import *
from chopper.pygfx.obj3d import *
from chopper.pygfx.point_ops import *
from chopper.pygfx.point_ops_2d import *
from chopper.pygfx.raster_ops import *
from chopper.pygfx.render import *
"""


##------------------------
# EXAMPLES  

"""
from chopper.examples.objects_2d import *
from chopper.examples.milling import *
from chopper.examples.raster import *
from chopper.examples.selection import *

from chopper.examples.wip import *
from chopper.examples.dag import *
from chopper.examples.dag import *
from chopper.examples.objects_3d import *
"""

##------------------------
# TOOLS 

"""

from chopper.tools.raytrace import *
"""

from chopper.tools.imagecam import *
from chopper.tools.raytrace import *

from chopper.examples.geometry import *
from chopper.examples.objects_2d import *
from chopper.examples.vector import *
from chopper.examples.render import *

##---------------------------------

from chopper.unit_tests.ut_matrix import *

#test_2d_intersect() 





#rotate_to_vector()

#model_geom_from_scratch()

#model_obj_from_scratch('whoknows.obj')

#model_geom_from_scratch_calc_normals()



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



class magnet_sand_trail(object):
    """drag a ball bearing around in the paint from underneath
    """
    def __init__(self):
        self.duration = .1

class air_puff(object):
    """blast of compressed air"""
    def __init__(self):
        self.angle_x = 0
        self.angle_y = 0
        self.angle_z = 0                 
        self.duration = .1 
        self.pressure = .1 

class brush_stroke(object):
    """good old brush (how to clean?) """
    def __init__(self):
        self.angle_x = 0
        self.angle_y = 0
        self.angle_z = 0           
        self.pressure = .1 
        
class circle_stroke(brush_stroke):
    def __init__(self):
        pass 

class paint_blob(object):
    def __init__(self):
        pass


class andy_worfarts(object):

    def __init__(self):
        self.units = 'mm'
        self.extents = []
        self.brush_strokes =[] 

    def load_from_json(self):
        pass 
