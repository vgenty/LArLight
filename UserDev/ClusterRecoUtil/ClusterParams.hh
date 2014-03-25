#ifndef UTIL_CLUSTERPARAMS_H
#define UTIL_CLUSTERPARAMS_H


namespace cluster{

  class cluster_params
  {
  public:
    cluster_params(){
      mean_charge                       = -999.999 ;
      mean_x                            = -999.999 ;
      mean_y                            = -999.999 ;
      rms_x                             = -999.999 ;
      rms_y                             = -999.999 ;
      charge_wgt_x                      = -999.999 ;
      charge_wgt_y                      = -999.999 ;
      eigenvalue_principal              = -999.999 ;
      eigenvalue_secondary              = -999.999 ;
      multi_hit_wires                   = -999.999 ;
      N_Wires                           = -999.999 ;
      verticalness                      = -999.999 ;
      opening_angle                     = -999.999 ;
      opening_angle_highcharge          = -999.999 ;
      closing_angle                     = -999.999 ;
      closing_angle_highcharge          = -999.999 ;
      offaxis_hits                      = -999.999 ;
      modified_hit_density              = -999.999 ;    
      length                            = -999.999 ;
      width                             = -999.999 ;
      hit_density_1D                    = -999.999 ;
      hit_density_2D                    = -999.999 ;
      angle_2d                          = -999.999 ;
      cluster_angle_2d		        = -999.999 ;
      showerness                        = -999.999 ;
      trackness                         = -999.999 ;
    } 

    ~cluster_params(){};

    double mean_charge;                // Mean (average) charge of hits in ADC
    double mean_x;                     // Mean of hits along x, peaks only
    double mean_y;                     // Mean of hits along y, peaks only
    double rms_x;                      // rms of hits along x (wires)
    double rms_y;                      // rms of hits along y, (time)
    double charge_wgt_x;               // Mean of hits along x, charge weighted
    double charge_wgt_y;               // Mean of hits along y, charge weighted
    double cluster_angle_2d;           // Linear best fit to high-charge hits in the cluster
    double angle_2d;                   // Angle of axis in wire/hit view
    double opening_angle;              // Width of angular distubtion wrt vertx
    double opening_angle_highcharge;   // Same for high charge 
    double closing_angle;              // Width of angular distubtion wrt endpoint
    double closing_angle_highcharge;   // Same for high charge 
    double eigenvalue_principal;       // the principal eigenvalue from PCA
    double eigenvalue_secondary;       // the secondary eigenvalue from PCA
    double verticalness;               // ???

    double length;
    double width;

    double hit_density_1D;
    double hit_density_2D;
    double multi_hit_wires;
    double N_Wires;
    double modified_hit_density;
    double N_Hits;


    int    direction;                  // +1 means shower is "forward - start wire < end wire"
    // -1 is backwards, 0 is undecided
    double showerness;                 // got heart
    double trackness;                  // got soul                                       
    double offaxis_hits;
  };
}

#endif