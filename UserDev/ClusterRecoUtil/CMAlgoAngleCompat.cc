#ifndef CMALGOANGLECOMPAT_CC
#define CMALGOANGLECOMPAT_CC

#include "CMAlgoAngleCompat.hh"

namespace cluster {

  CMAlgoAngleCompat::CMAlgoAngleCompat() {

    //this just sets default values
    SetVerbose(true);

    SetAngleCut(30.); // in degrees
    
    SetAllow180Ambig(false);

    SetUseOpeningAngle(false);

  } //end constructor

  bool CMAlgoAngleCompat::Merge(const ClusterParamsAlgNew &cluster1,
				const ClusterParamsAlgNew &cluster2){
    
    //pretty sure we don't need conversion factors here. 
    //already in cm/cm units, degrees? need to check that
    double angle1 = cluster1.GetParams().angle_2d;// * _time_2_cm / _wire_2_cm;
    double angle2 = cluster2.GetParams().angle_2d;// * _time_2_cm / _wire_2_cm;
    
    bool compatible = false;
    
    double my_cut_value;
    //if using opening angle, have angle cutoff be the smaller of the two opening angles
    if(_use_opening_angle) 
      my_cut_value = std::min(cluster1.GetParams().opening_angle, cluster2.GetParams().opening_angle);
    
    else my_cut_value = _max_allowed_2D_angle_diff;
    
    //if you don't care if clusters have been reconstructed backwards
    if(_allow_180_ambig)
      compatible = ( abs(angle1-angle2)     < _max_allowed_2D_angle_diff ||
		     abs(angle1-angle2-180) < _max_allowed_2D_angle_diff ||
		     abs(angle1-angle2+180) < _max_allowed_2D_angle_diff   );
    else
      compatible = ( abs(angle1-angle2)     < _max_allowed_2D_angle_diff );
    
  
    if(_verbose) {
	if(compatible) std::cout<<"These two clusters are compatible in angle."<<std::endl;
	else std::cout<<"These two clusters are NOT compatible in angle."<<std::endl;
    }
    
    return compatible;
    
  } // end Merge function 
  


}//end namespace cluster
#endif
