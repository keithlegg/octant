#ifndef CNC_PLOT_H    
#define CNC_PLOT_H

#include <vector>

#include "globals.h"
#include "Vectors.h"

#define MAX_NUM_PLY 100000


//----------------------------//
//gen_pulses() intentionally left out of class
//historicaly it processed pulsetrain data not stored in object itself 
void gen_pulses(std::vector<int>* pt_pulsetrain, int size, int num);

//old experimental version that used true 3D data to build pulsetrain 
//void gen_3d_pules(std::vector<Vector3>*, int, int);

//----------------------------//

//old version which we removed from class and brought back to life
//this is so we can precache each section of pulsetrain without the hassle of 
//dealing with internal object.pulsetrain data container 

void precalc_3d_pulses(std::vector<Vector3>* pt_pulsetrain,
                              Vector3 fr_pt, 
                              Vector3 to_pt,
                              int numdivs);



/********************/


class cnc_plot
{
    public:
        cnc_plot()
        {
            timediv    = 1; //speed == simtime divisions == 1 second/divs
            pidx       = 0;
            num_plys   = 0;

            running    = false;
            finished   = true;
            
            quill_pos  = Vector3(0,0,0);
            prg_origin = Vector3(0,0,0);
            prg_end    = Vector3(0,0,0);

            //vec1.reserve(30);  // Allocate space for 30 items, but vec1 is still empty.
            //vec2.resize(30);  // Allocate space for 30 items, and vec2 now contains 30 items.

        };
        
        ~cnc_plot(){};

        void show(void);
        void showgeom(void);
        void showpthids(void);
        void showply(unsigned int pidx);
        void show_pt(void);


        void pause(void);
        void stop(void);
        
        void run_sim(void);
        void update_sim(void);
        void precache_sim(void);

        void timer_init(void);
        //void timer_reset(void)

        void add_prg_vec(Vector3* nv); 
        void add_file_vec(Vector3* nv);

        void add_new_polygon(int numply, int numids);
        
        //void old_rapid_move(void);
        void rapid_move(void);
        
        void update_toolpaths(void);
        void loadpath( std::vector<Vector3>* pt_drawvecs);
        
        void calc_3d_pulses(Vector3, 
                            Vector3,
                            unsigned int);

        void run_send_pulses(cncglobals* pt_cg,
                             float f_x, float f_y, float f_z,
                             float s_x, float s_y, float s_z,
                             int divs);          

        
        double localsimtime;

        // keep track of number of polygons 
        // a ploygon is an indexed array of path vectors 
        // similar to an .OBJ file face, etc
        unsigned int num_plys;

        // index to the current vector processed while running 
        unsigned int pidx;
        double timediv;
        
        // calculated values - length of travel for vectors
        float rapid_dist;
        float program_dist;     


        //-----
        Vector3 quill_pos;
        Vector3 prg_origin;
        Vector3 prg_end;

        //-----    
        bool running;
        bool finished;

        //motion buffers
        float trav_dist  ;
        float num_vecs   ;
        float trav_speed ; //linear unit per sec 

        float retract_height;
        float work_height;

        //-----
        // data for the actual pulsing out the parport 
        std::vector<Vector3> pulsetrain;
        //pulse train indeces 
        std::vector<unsigned int> pt_idxs[MAX_NUM_PLY];

        //-----        

        // cache of toolpath component vectors 
        std::vector<Vector3> rapidmove_vecs;    
        std::vector<Vector3> program_vecs;  
        
        //-----         
        // the final "baked" path that gets run
        std::vector<Vector3> toolpath_vecs;
        //tool path indeces
        std::vector<unsigned int> tp_idxs[MAX_NUM_PLY];
        //-----
        // original vectors loaded from disk  
        // these get copied to program_vecs and linebuffer1 vecs (for display)
        std::vector<Vector3> loaded_file_vecs;
        





    private:
        void show_vecs(std::vector<Vector3> * pt_vec);




 

};



#endif

