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
            running = false;
        };
        
        ~cnc_plot(){};


    void stop(void);
    void run(void);

    //void rapid_move(Vector3* output, Vector3 from, Vector3 to, double speed);

    void rapid_move(float rh, float wh, 
                    Vector3 from, Vector3 to, 
                    double speed);


    void calc_precache( vector<Vector3>* pt_drawvecs, int numdivs);

    void gen_pules(std::vector<int>*, int, int);
    void calc_3d_pulses(std::vector<Vector3>*,
                        Vector3, 
                        Vector3,
                        int);
    
    //-----
    
    bool running;

    vector<Vector3> rapidmove_vecs;    
    vector<Vector3> program_vecs; 

    vector<Vector3> pathcache_vecs;

    double trav_dist  ;
    double num_vecs   ;
    double trav_speed ; //linear unit per sec 


    //void gen_3d_pules(std::vector<Vector3>*, int, int);
 

};



#endif

