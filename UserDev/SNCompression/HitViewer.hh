/**
 * \file HitViewer.hh
 *
 * \ingroup Analysis
 * 
 * \brief Class def header for a class HitViewer
 *
 * @author David Caratelli
 */

/** \addtogroup Analysis

    @{*/

#ifndef HITVIEWER_HH
#define HITVIEWER_HH

#include "ana_base.hh"
#include "GeometryUtilities.hh"
#include <TH2I.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TPad.h>
#include <string.h>

namespace larlight {
  /**
     \class HitViewer
     User custom analysis class made by SHELL_USER_NAME
  */
  class HitViewer : public ana_base{

  public:
    
    /// Default constructor
    HitViewer();
    
    /// Default destructor
    virtual ~HitViewer(){};

    /** IMPLEMENT in HitViewer.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in HitViewer.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in HitViewer.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

    /// A utility function to (re)create Th3D histogram of a specified boundary & name
    TH2I* Prepare2DHisto(std::string name, 
			 double wiremin, double wiremax,
			 double timemin, double timemax);
    
    TGraph* PrepareGraph();
    
    /// Getter for hit TH2I histo, weighted by charge
    const TH2I*  GetHisto_Hits (int view) const {
      if(view==0)
	return _hHits_U;
      else if(view==1)
	return _hHits_V;
      else if(view==2)
	return _hHits_Y;
      else {
std::cout<<"*******************you screwed something up. view should be 0 1 or 2!"<<std::endl;
	std::cout<<"returning _hHits_U because i don't know what else to return"<<std::endl;
	return _hHits_U;
      }
    };
    
  protected:

    /// Counter for event Number
    int _evtNum;

    /// Main canvas
    TCanvas* _c1;
    /// Main pad
    TPad*    _p1;
    
    /// Hit histograms to sit next to cluster ones,
    TH2I* _hHits_U;   
    TH2I* _hHits_V;    
    TH2I* _hHits_Y;
    
    GEO::View_t iview;
    std::vector<hit> ihit_v;
    
    double _w2cm;
    double _t2cm;

  };
}
#endif

/** @} */ // end of doxygen group 
