#ifndef CPALGOQSUM_CXX
#define CPALGOQSUM_CXX

#include "CPAlgoQSum.hh"

namespace cmtool {

  //----------------------------------------------------------
  CPAlgoQSum::CPAlgoQSum() : CPriorityAlgoBase()
  //----------------------------------------------------------
  {
    _qsum_cut = 0;
  }

  //------------------------------------------------------------------------------
  float CPAlgoQSum::Priority(const ::cluster::ClusterParamsAlg &cluster)
  //------------------------------------------------------------------------------
  {
    if(cluster.GetParams().sum_charge < _qsum_cut) return -1;

    return cluster.GetParams().sum_charge;
  }
  
    
}
#endif