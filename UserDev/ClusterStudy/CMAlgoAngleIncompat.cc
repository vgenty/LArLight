#ifndef CMALGOANGLEINCOMPAT_CC
#define CMALGOANGLEINCOMPAT_CC

#include "CMAlgoAngleIncompat.hh"

namespace cluster {

  //-------------------------------------------------------
  CMAlgoAngleIncompat::CMAlgoAngleIncompat() : CBoolAlgoBase()
  //-------------------------------------------------------
  {
    SetAngleCut(30.);

    SetMinHits(0);

    SetAllow180Ambig(false);

    SetUseOpeningAngle(false);

    SetMinLength(20);

    SetDebug(false);
  }

  //-----------------------------
  void CMAlgoAngleIncompat::Reset()
  //-----------------------------
  {

  }

  //------------------------------------------------------------------------------------------
  void CMAlgoAngleIncompat::EventBegin(const std::vector<cluster::ClusterParamsAlgNew> &clusters)
  //------------------------------------------------------------------------------------------
  {

  }

  //-------------------------------
  void CMAlgoAngleIncompat::EventEnd()
  //-------------------------------
  {

  }

  //-----------------------------------------------------------------------------------------------
  void CMAlgoAngleIncompat::IterationBegin(const std::vector<cluster::ClusterParamsAlgNew> &clusters)
  //-----------------------------------------------------------------------------------------------
  {

  }

  //------------------------------------
  void CMAlgoAngleIncompat::IterationEnd()
  //------------------------------------
  {

  }
  
  //----------------------------------------------------------------
  bool CMAlgoAngleIncompat::Bool(const ClusterParamsAlgNew &cluster1,
			       const ClusterParamsAlgNew &cluster2)
  //----------------------------------------------------------------
  {
    //if either cluster is too small, do not prohibit them
    if (cluster1.GetNHits() < _minHits || cluster2.GetNHits() < _minHits)
      return false;
    
    //if either cluster is too short, do not prohibit them
    if (cluster1.GetParams().length < _min_length ||
	cluster2.GetParams().length < _min_length)
      return false;

    
    double angle1 = cluster1.GetParams().angle_2d;
    double angle2 = cluster2.GetParams().angle_2d;

    if(angle1 < -998 || angle2 < -998)
      return false;

    bool compatible = false;
    
    double my_cut_value = _max_allowed_2D_angle_diff;

    //if using opening angle, have angle cutoff be the smaller of the two opening angles
    if(_use_opening_angle) 
      my_cut_value = std::min(cluster1.GetParams().opening_angle, cluster2.GetParams().opening_angle);
    
    //if you don't care if clusters have been reconstructed backwards
    if(_allow_180_ambig)
      compatible = ( abs(angle1-angle2) < _max_allowed_2D_angle_diff ||
		     abs(angle1-angle2-180) < _max_allowed_2D_angle_diff ||
		     abs(angle1-angle2+180) < _max_allowed_2D_angle_diff   );
    else
      compatible = ( abs(angle1-angle2)     < _max_allowed_2D_angle_diff );
    

    if(_debug){
      if(!compatible)
	
	std::cout<<"CMAlgoAngleIncompat is PROHIBITING clusters with angles "
		 <<angle1<<" and "<<angle2<<std::endl;

    }



    return !compatible;
  }

  //------------------------------
  void CMAlgoAngleIncompat::Report()
  //------------------------------
  {

  }
    
}
#endif
