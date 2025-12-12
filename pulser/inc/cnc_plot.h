#ifndef CNC_PLOT_H    
#define CNC_PLOT_H

#include <vector>
using std::vector;  


#include "Vectors.h"



class cnc_plot
{
    public:
        cnc_plot()
        {
            running  = false;
            finished = true;
            quill_pos  = Vector3(0,0,0);
            
            prg_origin = Vector3(0,0,0);
            prg_end    = Vector3(0,0,0);

            //vec1.reserve(30);  // Allocate space for 30 items, but vec1 is still empty.
            //vec2.resize(30);  // Allocate space for 30 items, and vec2 now contains 30 items.

        };
        
        ~cnc_plot(){};

        void show(void);
        void pause(void);
        void stop(void);
        void run(void);

        void rapid_move(void);
        void update_cache(void);
        void loadpath( vector<Vector3>* pt_drawvecs, int numdivs);

        void gen_pules(std::vector<int>*, int, int);
        void calc_3d_pulses(std::vector<Vector3>*,
                            Vector3, 
                            Vector3,
                            int);
        
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
        vector<Vector3> rapidmove_vecs;    
        vector<Vector3> program_vecs; 
        vector<Vector3> pathcache_vecs;


    private:
        void show_vecs(vector<Vector3> * pt_vec);



    //void gen_3d_pules(std::vector<Vector3>*, int, int);
 

};



#endif

