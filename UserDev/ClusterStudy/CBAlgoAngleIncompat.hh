/**
 * \file CBAlgoAngleIncompat.hh
 *
 * \ingroup ClusterStudy
 * 
 * \brief Class def header for a class CBAlgoAngleIncompat
 *
 * @author davidkaleko_NAME
 */

/** \addtogroup ClusterStudy

    @{*/
#ifndef CBALGOANGLEINCOMPAT_HH
#define CBALGOANGLEINCOMPAT_HH

#include <iostream>
#include "CBoolAlgoBase.hh"

namespace cmtool {
  /**
     \class CBAlgoAngleIncompat
     User implementation for CBoolAlgoBase class
     doxygen documentation!
  */
  class CBAlgoAngleIncompat : public CBoolAlgoBase {
    
  public:
    
    /// Default constructor
    CBAlgoAngleIncompat();
    
    /// Default destructor
    virtual ~CBAlgoAngleIncompat(){};

    /**
       Core function: given the CPAN input, return whether a cluster should be
       merged or not.
    */
    virtual bool Bool(const ::cluster::ClusterParamsAlgNew &cluster1,
		      const ::cluster::ClusterParamsAlgNew &cluster2);

    void SetAllow180Ambig(bool on) { _allow_180_ambig = on; }

    void SetAngleCut(double angle) { _max_allowed_2D_angle_diff = angle; }

    void SetUseOpeningAngle(bool on) { _use_opening_angle = on; }

    void SetMinHits(int n) { _minHits = n; }

    void SetMinLength(double l) { _min_length = l; }

    void SetDebug(bool on) { _debug = on; }

  protected:
   
    bool _debug;
    bool _allow_180_ambig;
    double _max_allowed_2D_angle_diff;
    double _min_length;
    bool _use_opening_angle;
    size_t _minHits;
  };
}
#endif
/** @} */ // end of doxygen group 

