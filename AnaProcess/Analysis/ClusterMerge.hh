/**
 * \file ClusterMerge.hh
 *
 * \ingroup Analysis
 * 
 * \brief Class def header for a class ClusterMerge
 *
 * @author kazuhiro
 */

/** \addtogroup Analysis

    @{*/

#ifndef CLUSTERMERGE_HH
#define CLUSTERMERGE_HH

#include "ana_base.hh"
#include "ClusterAnaPrep.hh"
#include "ClusterMergeAlgo.hh"

namespace larlight {
  /**
     \class ClusterMerge
     User custom analysis class made by kazuhiro
   */
  class ClusterMerge : public ana_base{
  
  public:

    /// Default constructor
    ClusterMerge(){ _name="ClusterMerge"; _fout=0;};

    /// Default destructor
    virtual ~ClusterMerge(){};

    /** IMPLEMENT in ClusterMerge.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in ClusterMerge.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in ClusterMerge.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

    protected:

  };
}
#endif

/** @} */ // end of doxygen group 
