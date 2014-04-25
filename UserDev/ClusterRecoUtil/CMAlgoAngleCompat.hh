/**
 * \file CMAlgoAngleCompat.hh
 *
 * \ingroup ClusterRecoUtil
 * 
 * \brief Class def header for a class CMAlgoAngleCompat
 *
 * @author davidkaleko
 */

/** \addtogroup ClusterRecoUtil

    @{*/
#ifndef CMALGOANGLECOMPAT_HH
#define CMALGOANGLECOMPAT_HH

#include <iostream>
#include "CMergeAlgoBase.hh"


namespace cluster {
  /**
     \class CMAlgoAngleCompat
     User defined class CMAlgoAngleCompat ... these comments are used to generate
     doxygen documentation!
  */
  class CMAlgoAngleCompat : public CMergeAlgoBase{
    
  public:
    
    /// Default constructor
    CMAlgoAngleCompat();
    
    /// Default destructor
    virtual ~CMAlgoAngleCompat(){};
        
    /// Overloaded (from CMergeAlgoBase) Merge function
    virtual bool Merge(const ClusterParamsAlgNew &cluster1,
		       const ClusterParamsAlgNew &cluster2);
    
    /// Method to set verbose mode
    void SetVerbose(bool on) { _verbose = on; }

    /// Method to set whether you allow angles to match with +/- 180 deg difference
    void SetAllow180Ambig(bool on) { _allow_180_ambig = on; }

    /// Method to set cut value in degrees for angle compatibility test
    void SetAngleCut(double angle) { _max_allowed_2D_angle_diff = angle; }

    /// Method to set angle cut value to be based on opening angle
    void SetUseOpeningAngle(bool on) { _use_opening_angle = on; }

  protected:

    bool _verbose;    /// bool to suppress lots of output if you want

    ///bool to allow "backwards" clusters (swapped start/end points)
    ///to still match in angle, even though they are 180 degrees apart
    ///only valid for _use_opening_angle==false
    bool _allow_180_ambig; 

    /// hard shower-axis angle cutoff (only valid for _use_opening_angle==false)
    double _max_allowed_2D_angle_diff; //in degrees

    /// bool set to true if you want to use opening angle as the cutoff
    /// angle instead of whatever you set with SetAngleCut
    bool _use_opening_angle;



  };
  
} // end namespace cluster

#endif
  /** @} */ // end of doxygen group 
  
