#ifndef TRACKVIEWER_CC
#define TRACKVIEWER_CC

#include "TrackViewer.hh"

namespace larlight {
  
  //################################################################
  TrackViewer::TrackViewer() : ana_base(), _hRecoTrack_v(), _track_v()
  //################################################################
  {
    // Class name
    _name = "TrackViewer";
    // Set initialization values for pointers
    _fout     = 0;
    _hRecoSPS = 0;
    _hMCStep  = 0;
    fDataType = DATA::Kalman3DSPS;
  }
  
  //################################################################
  bool TrackViewer::initialize()
  //################################################################
  {
    
    return true;
  }
  
  //################################################################
  bool TrackViewer::analyze(storage_manager* storage)
  //################################################################
  {
    
    // Clean up histograms if they already exist (from previous event)
    if(_hMCStep)  {delete _hMCStep;  _hMCStep  = 0;};
    if(_hRecoSPS) {delete _hRecoSPS; _hRecoSPS = 0;};
    for(auto h : _hRecoTrack_v) {delete h; h=0;};
    _hRecoTrack_v.clear();
    _track_v.clear();
    
    //
    // Obtain event-wise data object pointers
    //
    event_sps*    ev_sps    = (event_sps*)    ( storage->get_data(DATA::SpacePoint)  );
    event_track*  ev_track  = (event_track*)  ( storage->get_data(fDataType)         );
    event_mcpart* ev_mcpart = (event_mcpart*) ( storage->get_data(DATA::MCParticle)  );
    
    // Define utility variables to hold max/min of each axis range
    double xmax, ymax, zmax;
    xmax = ymax = zmax = -1.;
    double xmin, ymin, zmin;
    xmin = ymin = zmin = 999999.;
    
    // Find max/min boundary for all axis (spacepoint and track)
    if(ev_sps)
      
      ev_sps->get_axis_range(xmax, xmin, ymax, ymin, zmax, zmin);
    
    if(ev_track)
      
      ev_track->get_axis_range(xmax, xmin, ymax, ymin, zmax, zmin);
    
    //    if(ev_mcpart)
      
    //ev_mcpart->get_axis_range(xmax, xmin, ymax, ymin, zmax, zmin);
    
    // Proceed only if minimum/maximum are set to some values other than the defaults
    if(xmax == -1) {
      
      print(MSG::WARNING,__FUNCTION__,
	    "Did not find any reconstructed spacepoint or track. Skipping this event...");
      
      return true;
    }
    
    //
    // Make & fill vertex histograms
    //
    
    // Spacepoints
    if(ev_sps) {
      
      /// Note: for a plotting style, it is convenient to have a (x, y, z) labeled as (z, x, y).
      _hRecoSPS = Prepare3DHisto("_hRecoSPS", zmin, zmax, xmin, xmax, ymin, ymax);
      
      for(auto sps : *ev_sps)
	
	_hRecoSPS->Fill(sps.XYZ()[2], sps.XYZ()[0], sps.XYZ()[1]);
      
      _hRecoSPS->SetMarkerStyle(23);
      _hRecoSPS->SetMarkerColor(kBlue);
    }
    
    // Tracks
    if(ev_track) {
      
      for(auto const& trk : *ev_track){
	
	TH3D* h=0;
	h=Prepare3DHisto(Form("_hRecoTrack_%03d",(int)(_hRecoTrack_v.size())),
			 zmin,zmax,xmin,xmax,ymin,ymax);
	
	for(size_t i=0; i<trk.n_point(); i++) {
	  
	  const TVector3 vtx = trk.vertex_at(i);
	  
	  h->Fill(vtx[2],vtx[0],vtx[1]);
	  
	}
	
	h->SetMarkerStyle(22);
	h->SetMarkerColor(kRed);
	_hRecoTrack_v.push_back(h);
	
	_track_v.push_back(track(trk));
	
      }
      
    }
    
    // MC trajectory points
    if(ev_mcpart){

      for(auto const part : *ev_mcpart){

	// Only care about a primary particle
	if(part.TrackId() != 1)
	  
	  continue;
	
	_hMCStep = Prepare3DHisto("_hMCStep",
				  zmin, zmax, xmin, xmax, ymin, ymax);

	for(auto const step : part.Trajectory()) 

	  _hMCStep->Fill(step.Z(), step.X(), step.Y());
	
	_hMCStep->SetMarkerStyle(20);
	_hMCStep->SetMarkerColor(kCyan);
	break; // Only make 1 histogram
      }
    }
    
    return true;
  };
  
  //################################################################
  TH3D* TrackViewer::Prepare3DHisto(std::string name, 
				    double xmin, double xmax,
				    double ymin, double ymax,
				    double zmin, double zmax)
  //################################################################
  {
    
    TH3D* h=0;
    if(h) delete h;
    
    h = new TH3D(name.c_str(),"3D Viewer; Z; X; Y",
		 50,  xmin, xmax,
		 50,  ymin, ymax,
		 300, zmin, zmax);
    
    return h;
  }
  
  bool TrackViewer::finalize() {
    
    // This function is called at the end of event loop.
    // Do all variable finalization you wish to do here.
    // If you need, you can store your ROOT class instance in the output
    // file. You have an access to the output file through "_fout" pointer.
    
    return true;
  }
}
#endif
  
