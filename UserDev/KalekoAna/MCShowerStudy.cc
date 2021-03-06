#ifndef MCSHOWERSTUDY_CC
#define MCSHOWERSTUDY_CC

#include "MCShowerStudy.hh"

namespace larlight {
  
  bool MCShowerStudy::initialize() {
    
    //
    // This function is called in the beggining of event loop
    // Do all variable initialization you wish to do here.
    // If you have a histogram to fill in the event loop, for example,
    // here is a good place to create one on the heap (i.e. "new TH1D"). 
    //

    //temporary
    _hit_angles_forwards = 0;
    _hit_angles_backwards = 0;
    if(!_hit_angles_forwards)
      _hit_angles_forwards = new TH1F("_hit_angles_forwards","_hit_angles_forwards",100,-1.,1.);
    if(!_hit_angles_backwards)
      _hit_angles_backwards = new TH1F("_hit_angles_backwards","_hit_angles_backwards",100,-1.,1.);      
    //end temporary

    _mcss_ana_tree = 0;
    PrepareTTree();

    //setting variables that i should be getting from geometry service...
    _wire_2_cm = 0.3; 
    _time_2_cm = 0.0818566;

    return true;
  }
  
  bool MCShowerStudy::analyze(storage_manager* storage) {
    
    //
    // Do your event-by-event analysis here. This function is called for 
    // each event in the loop. You have "storage" pointer which contains 
    // event-wise data. To see what is available, check the "Manual.pdf":
    //
    // http://microboone-docdb.fnal.gov:8080/cgi-bin/ShowDocument?docid=3183


    //grab the reconstructed clusters
    larlight::event_cluster* ev_cluster = (event_cluster*)storage->get_data(DATA::ShowerAngleCluster);
    if(!ev_cluster) {
      print(larlight::MSG::ERROR,__FUNCTION__,Form("Did not find specified data product, ShowerAngleCluster!"));
      return false;
    }
    
    //grab the MC showers
    larlight::event_mcshower* ev_mcshower = (event_mcshower*)storage->get_data(DATA::MCShower);
    if(!ev_mcshower) {
      print(larlight::MSG::ERROR,__FUNCTION__,Form("Did not find specified data product, MCShower!"));
      return false;
    }
    
    //clear the TTree variables before doing any calculations (or, set to non-physical values
    //to indicate that somehting has gone wrong if you find those values in your output tree)
    ClearTTreeVars();


    //***********************************************************//
    //implement your own functions w/ loops, etc seperately here!// 
    //they should calculate variables (in this scope) that you   //
    //want to save to the ttree (filled once per analyze loop)   // 
    //***********************************************************//

    //kaleko testing how well his refine-direction works
    RefineDirectionTest(storage, ev_cluster, ev_mcshower);
    



    //fill TTree (make sure all variables going into it are defined in this scope)
    if(_mcss_ana_tree) _mcss_ana_tree->Fill();
  
  
    return true;
  } //end of analyze function
  
  bool MCShowerStudy::finalize() {
    
    // This function is called at the end of event loop.
    // Do all variable finalization you wish to do here.
    // If you need, you can store your ROOT class instance in the output
    // file. You have an access to the output file through "_fout" pointer.
    //
    // Say you made a histogram pointer h1 to store. You can do this:
    //
    // if(_fout) { _fout->cd(); h1->Write(); }
    //
    // else 
    //   print(MSG::ERROR,__FUNCTION__,"Did not find an output file pointer!!! File not opened?");
    //
  
    if(_fout) {

      _fout->cd();
      if(_mcss_ana_tree)
	_mcss_ana_tree->Write();

    }
    else 
      print(MSG::ERROR,__FUNCTION__,"Did not find an output file pointer!!! File not opened?");

    delete _mcss_ana_tree;
    return true;
  }


  void MCShowerStudy::PrepareTTree() {
    //define branches on your ttree here for your calculated parameters

    if(!_mcss_ana_tree) {
      _mcss_ana_tree = new TTree("mcss_ana_tree","");
      _mcss_ana_tree->Branch("refinestartend_FOM", "std::vector<double>", &_refinestartend_FOM);
      _mcss_ana_tree->Branch("nhits","std::vector<int>",&_nhits);
      _mcss_ana_tree->Branch("qhit","std::vector<double>",&_qhit);
      _mcss_ana_tree->Branch("mom_energy","std::vector<double>",&_mom_energy);
      _mcss_ana_tree->Branch("cluscharge","std::vector<double>",&_cluscharge);
      _mcss_ana_tree->Branch("cluslength","std::vector<double>",&_cluster_length);
      _mcss_ana_tree->Branch("dirqual","std::vector<double>",&_dir_quality);
      _mcss_ana_tree->Branch("alpha_s","std::vector<double>",&_alpha1);
      _mcss_ana_tree->Branch("alpha_e","std::vector<double>",&_alpha2);

    }

  }
  void MCShowerStudy::ClearTTreeVars() {
    
    _refinestartend_FOM.clear();
    _nhits.clear();
    _qhit.clear();
    _mom_energy.clear();
    _cluscharge.clear();
    _cluster_length.clear();
    _dir_quality.clear();
    _alpha1.clear();
    _alpha2.clear();
  }

  
  bool MCShowerStudy::RefineDirectionTest(storage_manager* storage, larlight::event_cluster* ev_cluster, larlight::event_mcshower* ev_mcshower){
    
    //for multi shower events, can use bryce's code that matches reco clusters with showers.. 
    //demand that 90% of the hits in my reco shower came from one MC shower, then use that MC shower for the comparison


    //for kaleko's current study, this only works for single-shower events, and single-cluster (fully merged)
    if(ev_mcshower->size() != 1){
      print(larlight::MSG::INFO,__FUNCTION__,Form("This event has more than one MCShower in it! In fact, it has %d! Wtf?",(int)ev_mcshower->size()));
      return false;
    }

    if(ev_cluster->size() != 3){
      print(larlight::MSG::INFO,__FUNCTION__,Form("This event has != 3 reco clusters in it! In fact, it has %d! Wtf?",(int)ev_cluster->size()));
      return false;
    }
    
    //will have to use geometry utility for this
    const larutil::GeometryUtilities *myGeoUtil = larutil::GeometryUtilities::GetME();
    
    
    //assuming one true shower, this will hold the PxPoint of the mother start position
    //one entry for each view
    std::vector<larutil::PxPoint> mother_sw_view(3,larutil::PxPoint());
    
    //loop over MCShowers in the event (there should only be one, for now)
    for(auto i_mcshower: *ev_mcshower){

      Double_t pos[3]={0.};
      pos[0]=i_mcshower.MotherPosition()[0];
      pos[1]=i_mcshower.MotherPosition()[1];
      pos[2]=i_mcshower.MotherPosition()[2];

      Double_t mom_energy = i_mcshower.MotherMomentum()[3];
      _mom_energy.push_back(mom_energy);

      for(size_t i = 0; i < mother_sw_view.size(); ++i)
	mother_sw_view.at(i)=myGeoUtil->Get2DPointProjection(pos,i);



      //debugging stuff
      std::ostringstream msg;
      msg << Form("Motherposition of MCShower is (%f,%f,%f,%f).\n",
		  i_mcshower.MotherPosition()[0],
		  i_mcshower.MotherPosition()[1],
		  i_mcshower.MotherPosition()[2],
		  i_mcshower.MotherPosition()[3]);
      
      msg << Form("This MCShower's start (wire,time) on view0 is (%f,%f)\n",
		  mother_sw_view[0].w,mother_sw_view[0].t);
      msg << Form("This MCShower's start (wire,time) on view1 is (%f,%f)\n",
		  mother_sw_view[1].w,mother_sw_view[1].t);
      msg << Form("This MCShower's start (wire,time) on view2 is (%f,%f)\n",
		  mother_sw_view[2].w,mother_sw_view[2].t);

      print(larlight::MSG::DEBUG,__FUNCTION__,msg.str());
               
    }


    



    //debugging stuff
    std::ostringstream msg;
    
    //loop over reconstructed clusters in the event (there should only be three, for now)    
    for(auto const i_cluster: *ev_cluster){
      
      int iview = (int)i_cluster.View();
            
      Double_t start_wire = i_cluster.StartPos()[0];
      Double_t start_time = i_cluster.StartPos()[1];
      Double_t end_wire = i_cluster.EndPos()[0];
      Double_t end_time = i_cluster.EndPos()[1];
	      
      
      
      print(larlight::MSG::DEBUG,__FUNCTION__,Form("Heads up! You're using refine_direction function!"));
      DATA::DATA_TYPE hit_type = ev_cluster->get_hit_type();
      
      //you need the hits for the refine function
      larlight::event_hit* ev_hits = (event_hit*)storage->get_data(hit_type);
      
      if(!ev_hits) {
	print(larlight::MSG::ERROR,__FUNCTION__,Form("Did not find specified data product, Hits!"));
	return false;
      }
      
      //list of the hits's indicies associated with this cluster
      auto const& hit_index = i_cluster.association(hit_type);
      
      //vector to hold these actual hits
      std::vector<larlight::hit> hits(hit_index.size());
      
      //pushing back the actual hits into the containter
      for(auto const &index : hit_index)
	hits.push_back(ev_hits->at(index));
      
      //this function should edit start_wire, start_time etc, flipping stuff when necessary
      RefineStartPointsShowerShape(start_wire,start_time,end_wire,end_time,hits);
      

      double dw1, dt1, da1, db1, dw2, dt2, da2, db2;
      //calculate start/end refining figure of merit for this cluster
      dw1 = mother_sw_view[iview].w - start_wire;
      dt1 = mother_sw_view[iview].t - start_time;
      
      da1 = dw1 * _wire_2_cm;
      db1 = dt1 * _time_2_cm;
      
      dw2 = mother_sw_view[iview].w - end_wire;
      dt2 = mother_sw_view[iview].t - end_time;
      
      da2 = dw2 * _wire_2_cm;
      db2 = dt2 * _time_2_cm;
      
      double clusterlength;
      clusterlength = std::sqrt(pow((end_wire-start_wire)*_wire_2_cm,2)+pow((end_time-start_time)*_time_2_cm,2));
      _cluster_length.push_back(clusterlength);
      
      //this is the variable that i want to go into the ttree... for single cluster events,
      //this will be a vector of length 3 (one per cluster, one cluster per view)
      double myFOM =  std::sqrt( da2*da2 + db2*db2 ) - std::sqrt( da1*da1 + db1*db1 );
      _refinestartend_FOM.push_back(myFOM);
      
      //debugging stuff
      msg << Form("This cluster's start (wire,time) on view %d is (%f,%f)\n",
		  iview, start_wire, start_time);
      msg << Form("So, the _refinestartend_FOM value is %f", myFOM);

    }

    print(larlight::MSG::DEBUG,__FUNCTION__,msg.str());

    return true;
    
 
  }
  
  
  //for now, hand-copying this function from the merge alg until kazu implements it in showercluster
  void MCShowerStudy::RefineStartPointsShowerShape(Double_t &start_wire, Double_t &start_time, 
						   Double_t &end_wire, Double_t &end_time,
						   const std::vector<larlight::hit> &in_hit_v){
    //assuming start/endpoint are correct, make histogram of angles between
    //individual hits and the vector pointing from start to end point
    _hit_angles_forwards -> Reset();
    _hit_angles_backwards-> Reset();
    
    double clus_q = 0;
    
    double cosangle = 99999999.;
    
    //vector from start point to end point is SEPvec
    //vector from start point to hit is SHITvec ;)
    
    //loop over hits

    _nhits.push_back((int)in_hit_v.size());

    for(auto const i_hit: in_hit_v){
      clus_q += i_hit.Charge();

      _qhit.push_back(i_hit.Charge());
      //cutoff testing here
      if(i_hit.Charge() < 25) continue;
      
      double SEP_x  = (end_wire      - start_wire) * _wire_2_cm;
      double SEP_y  = (end_time      - start_time) * _time_2_cm;
      double SHIT_x = (i_hit.Wire()     - start_wire) * _wire_2_cm;
      double SHIT_y = (i_hit.PeakTime() - start_time) * _time_2_cm;    
      
      cosangle = ( SEP_x*SHIT_x + SEP_y*SHIT_y );
      cosangle = cosangle / (
			     pow( pow(SEP_x,2)+ pow(SEP_y,2) ,0.5) *
			     pow( pow(SHIT_x,2)+pow(SHIT_y,2),0.5)
			     );
      
      //      _hit_angles_forwards->Fill(cosangle,std::sqrt(i_hit.Charge()));
      _hit_angles_forwards->Fill(cosangle);
      
      //now switch the vector to be from end point to start point and re-do
      SEP_x  = (start_wire    - end_wire) * _wire_2_cm;
      SEP_y  = (start_time    - end_time) * _time_2_cm;
      SHIT_x = (i_hit.Wire()     - end_wire) * _wire_2_cm;
      SHIT_y = (i_hit.PeakTime() - end_time) * _time_2_cm;        
      
      cosangle = ( SEP_x*SHIT_x + SEP_y*SHIT_y );
      cosangle = cosangle / (
			     pow( pow(SEP_x,2)+ pow(SEP_y,2) ,0.5) *
			     pow( pow(SHIT_x,2)+pow(SHIT_y,2),0.5)
			     );
      
      //_hit_angles_backwards->Fill(cosangle,std::sqrt(i_hit.Charge()));
      _hit_angles_backwards->Fill(cosangle);
    }

    //more ttree variables
    _cluscharge.push_back(clus_q);

    
    //for now, use biggest mean && smallest RMS to decide
    //works slightly better than just using mean to decide
    //there's probably a better way to do this
    double alpha_start = (_hit_angles_forwards->GetMean())/(_hit_angles_forwards->GetRMS());
    double alpha_end = (_hit_angles_backwards->GetMean())/(_hit_angles_backwards->GetRMS());
    double DirectionQualityScore = (alpha_start-alpha_end)/(alpha_start+alpha_end);
    if(DirectionQualityScore < -1 || DirectionQualityScore > 1)
      DirectionQualityScore = 0;

    _dir_quality.push_back(DirectionQualityScore);
    _alpha1.push_back(alpha_start);
    _alpha2.push_back(alpha_end);
    /*
      if(DirectionQualityScore<-1 || DirectionQualityScore>1){
	std::cout<<"DQS = "<<DirectionQualityScore<<std::endl;
	std::cout<<"alpha_Start, alpha_end = "<<alpha_start<<", "<<alpha_end<<std::endl;
	}
    */
    
    //if you want to actually switch clusters that this function determined should be switched
    if(_refine_direction){
      //decide if you want to switch the start and end point
    
      if(_hit_angles_forwards->GetMean() < _hit_angles_backwards->GetMean() && 
    	 _hit_angles_forwards->GetRMS()  > _hit_angles_backwards->GetRMS() ){
	double new_end_wire   = start_wire;
	double new_end_time   = start_time;
	double new_start_wire = end_wire;
	double new_start_time = end_time;
	//double new_angle = something?!! not trivial to switch it
	start_wire = new_start_wire;
	end_wire   = new_end_wire;
	start_time = new_start_time;
	end_time   = new_end_time;	
      }
    }
 
  }//end RefineStart shit
  




} // end namespace larlight

#endif
