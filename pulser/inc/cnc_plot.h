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

            //vec1.reserve(30);  // Allocate space for 30 items, but vec1 is still empty.
            //vec2.resize(30);  // Allocate space for 30 items, and vec2 now contains 30 items.

        };
        
        ~cnc_plot(){};

    void pause(void);
    void stop(void);
    void run(void);

    //void rapid_move(Vector3* output, Vector3 from, Vector3 to, double speed);

    // void rapid_move(float rh, float wh, 
    //                 Vector3 from, Vector3 to, 
    //                 double speed);

    void rapid_move(void);
    void update_cache(void);
    void precache( vector<Vector3>* pt_drawvecs, int numdivs);

    void gen_pules(std::vector<int>*, int, int);
    void calc_3d_pulses(std::vector<Vector3>*,
                        Vector3, 
                        Vector3,
                        int);
    
    //-----
    Vector3 quill_pos;
    Vector3 prg_origin;

    //-----    
    bool running;
    bool finished;

    //motion buffers
    double trav_dist  ;
    double num_vecs   ;
    double trav_speed ; //linear unit per sec 

    double retract_height;
    double work_height;

    //-----
    vector<Vector3> rapidmove_vecs;    
    vector<Vector3> program_vecs; 
    vector<Vector3> pathcache_vecs;






    //void gen_3d_pules(std::vector<Vector3>*, int, int);
 

};



#endif

