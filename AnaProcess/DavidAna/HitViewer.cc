#ifndef HITVIEWER_CC
#define HITVIEWER_CC

#include "HitViewer.hh"

namespace larlight {


  //********************************
  HitViewer::HitViewer(): ana_base(), _hHits_U(), _hHits_V(), _hHits_Z()
  //********************************
  {
    //Class Name
    _name = "HitViewer";
    //set initialization for pointers
    _hHits_U = 0;
    _hHits_V = 0;
    _hHits_Z = 0;
  }

  //********************************
  bool HitViewer::initialize()
  //********************************
  {

    return true;
  }
  

  //**********************************************
  bool HitViewer::analyze(storage_manager* storage)
  //**********************************************
  {

    std::cout << "Analyzing..." << std::endl;

    //clean up histograms if they already exist (from previous event)
    if (_hHits_U) {delete _hHits_U; _hHits_U = 0;};  
    if (_hHits_V) {delete _hHits_V; _hHits_V = 0;};  
    if (_hHits_Z) {delete _hHits_Z; _hHits_Z = 0;};  


    //Get Hits
    const event_hit *hits = (event_hit*)(storage->get_data(DATA::FFTHit));

    std::cout << "Just got hits! " << std::endl;
    std::cout << "Number of Hits in event: " << hits->size() << std::endl;

    //Define axis ranges
    std::vector<double> chmax, chmin, wiremax, wiremin, timemax, timemin;
    //    chmax = wiremax = timemax = 999999.;
    //    chmin = wiremin = timemin = -1;
    //Find axis boundary
    hits->get_axis_range(chmax, chmin, wiremax, wiremin, timemax, timemin);
    //proceed only if values actually reset
    if ( wiremax[0] <= -1 )
      {
	print(MSG::WARNING,__FUNCTION__,
	      "Did not find any reconstructed hits in view 0. Skipping this event...");
	return true;
      }
    
    //if all ok, plot wire vs. time for hits
    _hHits_U = Prepare2DHisto("HitHistU", wiremin[0], wiremax[0], timemin[0], timemax[0]);
    _hHits_V = Prepare2DHisto("HitHistV", wiremin[1], wiremax[1], timemin[1], timemax[1]);
    _hHits_Z = Prepare2DHisto("HitHistZ", wiremin[2], wiremax[2], timemin[2], timemax[2]);

    //loop over hits
    for (size_t i=0; i<hits->size(); i++)
      {
	const hit *this_hit = (&(hits->at(i)));
	//place in right plane
	if ( this_hit->View()==0 )
	  _hHits_U->Fill( this_hit->Wire(), this_hit->PeakTime(), this_hit->Charge() );
	if ( this_hit->View()==1 )
	  _hHits_V->Fill( this_hit->Wire(), this_hit->PeakTime(), this_hit->Charge() );
	if ( this_hit->View()==2 )
	  _hHits_Z->Fill( this_hit->Wire(), this_hit->PeakTime(), this_hit->Charge() );
      }//end loop over hits
    
    
    return true;
  }

  //****************************************************************
  TH2D* HitViewer::Prepare2DHisto(std::string name, 
				      double wiremin, double wiremax,
				      double timemin, double timemax)
  //****************************************************************
  {
    
    TH2D* h=0;
    if(h) delete h;
    
    double mywiremin=0.9*wiremin;
    double mywiremax=1.1*wiremax;
    double mytimemin=0.9*timemin;
    double mytimemax=1.1*timemax;

    int wirewidth = (int)mywiremax-mywiremin;
    int timewidth = (int)mytimemax-mytimemin; 

    h = new TH2D(name.c_str(),"2D Viewer; Wire; Time;",
		 50,  mywiremin, mywiremax,
		 50,  mytimemin, mytimemax);
    
    return h;
  }

  bool HitViewer::finalize() {
  
    return true;
  }
}
#endif
