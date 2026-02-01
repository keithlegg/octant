#ifndef CNC_PLOT_H    
#define CNC_PLOT_H

#include <vector>

#include "globals.h"
#include "Vectors.h"

#define MAX_NUM_PLY 100000
#define MAX_MOTION_NUM 100000
#define MAX_DISP_VECTORS 100000

/*
    WIP - OVERVIEW OF DATA SCTRUCTUREMAX_MOTION_NUM


    
    PROGRAM = ( NAME -> ORIGIN -> LINKTYPE (rapid,file,etc) PRELINK , POSTLINK )

    

    // cache of toolpath component vectors 
    std::vector<Vector3> rapidmove_vecs;    
    std::vector<Vector3> program_vecs;  

    // original vectors loaded from disk  
    std::vector<Vector3> loaded_file_vecs;


    // the final "baked" path that gets run
    std::vector<Vector3> toolpath_vecs;
    //tool path indeces
    std::vector<uint> tp_idxs[MAX_NUM_PLY];

    // data for the actual pulsing out the parport 
    std::vector<Vector3> pulsetrain;
    //std::vector<uint> pt_idxs[MAX_NUM_PLY];

*/



//----------------------------//

void pulse_thread(double f_x,
                  double f_y,
                  double f_z,
                  double s_x,
                  double s_y,
                  double s_z,
                 uint x_divs, 
                 uint y_divs,                                   
                 uint z_divs);  

//----------------------------//
void run_cncplot(double f_x,
                 double f_y,
                 double f_z,
                 double s_x,
                 double s_y,
                 double s_z,
                 uint x_divs, 
                 uint y_divs,                                   
                 uint z_divs);  


//----------------------------//
//gen_pulses() intentionally left out of class
//historicaly it processed pulsetrain data not stored in object itself 
void gen_pulses(std::vector<int>* pt_pulsetrain, int size, int num);

//----------------------------//

/*
    object to define an atomic unit of motion 
    links to paths - kind of like a foreign key in databases 
*/

class motion_idx
{

    public:
        motion_idx(){}
        motion_idx(std::string nam, std::string typ, 
                   int rapidin, int progid, int rapidout)
        {
            name      = nam;
            type      = typ;
            prog_id   = progid;
            rapid_in  = rapidin;
            rapid_out = rapidout;
        }


        ~motion_idx(){};

        Vector3 origin;

        std::string name;
        std::string type;

        //index to objects 
        //this is silly but I have to use "int" instead of "uint"
        //-1 is a flag no index exists , its zero indexed - doh! 
        int rapid_in;
        int rapid_out;
        int prog_id;

};


//----------------------------//

class cnc_plot
{
    public:
        cnc_plot()
        {
            timediv    = 1; //speed == simtime divisions == 1 second/divs
            vec_idx    = 0;
            num_rpd_plys   = 0;

            running    = false;
            finished   = true;
            
            quill_pos  = Vector3(0,0,0);
            prg_origin = Vector3(0,0,0);
            prg_end    = Vector3(0,0,0);

            //vec1.reserve(30);  //  how does reserve work ??
            //vec2.resize(30);   // 

        };
        
        ~cnc_plot(){};

        //--------------------------// 
        //debugging tools 

        void show_mpath(void);
        void show_mpath_info(void);
        void show_path(void);
        void show_path_info(void);

        void showgeom(void);
        void showpthids(void);
        void showply(uint vec_idx);
        void show_pt(void);

        //--------------------------// 
        // simulation control 

        void pause(void);
        void stop(void);
        void reset(void);        
        void run_sim(void);
        void update_sim(void);
        void timer_init(void);
        
        //##########################//
        // experimental tools

        void prim_shape(uint shape, uint axis, float size);
        //void copy_prg_to_dispvec(void);
        void import_path_from_obj(std::string filepath);

        //##########################//

        // motion path interface backend
        void copy_prg_to_toolpath(void);
    

        //add single vector to NON INDEXED 
        void add_file_vec(Vector3* nv);

        //add single vector TO INDEXED (you need to update idx)
        void add_disp_vec(Vector3* nv);
        void add_prg_vec(Vector3* nv); 
        void add_rapid_vec(Vector3* nv);

        //this gets called post file load - but its not done  
        void init_paths( std::vector<Vector3>* pt_drawvecs);   
        //void mov_fv_to_pv(void); // move file vec to program vec , clear file vec


        void add_motion(std::string name, std::string type, 
                        int prog_id, int rapid_in, int rapid_out); 

        void add_prgvec_ply(void);
        void add_dispvec_ply(void);
   

        //debug these are wip 
        void update_toolpaths(void); //old update tp
        void bake_motion(void);      //new WIP update tp 

        void clear_motionidx(void);
        void clear_toolpaths(void);
        // del_motion   (std::string name, Vector3 start, Vector3 end);
        // link_motions (std::string name, Vector3 start, Vector3 end);

        // end motion interface 

        void toolpath_extents(void);

        //--------------------------//   
        // file operations  NOT DONE 

        void save_motionfile(void);
        void load_motionfile(void);


        //--------------------------//  
        // simulation backend 

        void process_vec(uint vec_idx);

        void calc_3d_pulses(Vector3 fr_pt, 
                            Vector3 to_pt,
                            uint numdivx,
                            uint numdivy,
                            uint numdivz);
        
        double localsimtime;

        // index to the current vector processed while running 
        uint vec_idx;
        uint num_simvecs;

        double timediv;
        
        // calculated values - length of travel for vectors
        float rapid_dist;
        float program_dist;     

        //--------------------------// 
        Vector3 quill_pos;
        Vector3 prg_origin;
        Vector3 prg_end;

        //--------------------------//     
        bool running;
        bool finished;

        //motion buffers
        float trav_dist  ;
        float num_vecs   ;
        float trav_speed ; //linear unit per sec 

        float retract_height;
        float work_height;

        //--------------------------// 
        // data for the actual pulsing out the parport 
        std::vector<Vector3> pulsetrain;
        //pulse train indeces (experiment not used?) 
        //std::vector<uint> pt_idxs[MAX_NUM_PLY];

        //--------------------------//         
        // cache of toolpath component vectors 
        std::vector<Vector3> rapidmove_vecs;    
        std::vector<uint> rpd_idxs[MAX_NUM_PLY]; // index to rapidmove vecs
        uint num_rpd_plys;

        //--------------------------//    
        std::vector<Vector3> program_vecs;          
        std::vector<uint> prg_idxs[MAX_NUM_PLY]; // index to program vecs
        uint num_prg_plys;
        //--------------------------//     
        std::vector<motion_idx> motion_prg[MAX_MOTION_NUM];  // "fk" style links to motion vecs

        //--------------------------//    
        // toolpaths probably dont need to be indexed , but I did anyway 
        // I mistakenly wrote it in so I left it     
        std::vector<Vector3> toolpath_vecs;     // toolpath data (dynamically constructed)
        std::vector<uint> tp_idxs[MAX_NUM_PLY]; // index to toolpath vecs
        uint num_toolpath_ids;

        //--------------------------//    
        // indexed static display geom - just for viewing (and possible storing) vectors      
        std::vector<Vector3> disp_vecs;     
        std::vector<uint> disp_idxs[MAX_DISP_VECTORS];  
        uint num_disp_ids;

        //-----
        // temp buffer for incoming vector data , typically from a file but not always 
        std::vector<Vector3> loaded_file_vecs;  

    private:
        void show_vecs(std::vector<Vector3> * pt_vec);


};



#endif

