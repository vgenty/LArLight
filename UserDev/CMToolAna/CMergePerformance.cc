#ifndef CMERGEPERFORMANCE_CC
#define CMERGEPERFORMANCE_CC

#include "CMergePerformance.hh"

namespace larlight {
  
  bool CMergePerformance::initialize() {

    SetDebug(false);

    ana_tree=0;
    PrepareTTree();
    
    PrepareHistos();
    
    return true;
  }
  
  bool CMergePerformance::analyze(storage_manager* storage) {
          
    //grab the reconstructed clusters
    larlight::event_cluster* ev_cluster = (larlight::event_cluster*)storage->get_data(_cluster_type);
    if(!ev_cluster) {
      print(larlight::MSG::ERROR,__FUNCTION__,Form("Did not find specified data product, FuzzyCluster!"));
      return false;
    }
    
    //grab the reconstructed hits
    hit_type = ev_cluster->get_hit_type();
    larlight::event_hit* ev_hits = (larlight::event_hit*)storage->get_data(hit_type);
    if(!ev_hits) {
      print(larlight::MSG::ERROR,__FUNCTION__,Form("Did not find specified data product, Hits!"));
      return false;
    }
    
    //grab the MC showers
    larlight::event_mcshower* ev_mcshower = (larlight::event_mcshower*)storage->get_data(larlight::DATA::MCShower);
    if(!ev_mcshower) {
      print(larlight::MSG::ERROR,__FUNCTION__,Form("Did not find specified data product, MCShower!"));
      return false;
    }

    //grab the simchannels
    larlight::event_simch* ev_simch = (larlight::event_simch*)storage->get_data(larlight::DATA::SimChannel);
    if(!ev_simch) {
      print(larlight::MSG::ERROR,__FUNCTION__,Form("Did not find specified data product, SimChannel!"));
      return false;
    }

    _mcslb.SetMinEnergyCut(_MCShower_mother_energy_cutoff);

    //fill some std::maps that are constant through the whole event
    //_shower_idmap is (G4trackid => MCShower index in ev_mcshower)
    //map does NOT include trackIDs from particles belonging to 
    //MCShowers with less than cutoff energy
    _shower_idmap.clear();
    _mcslb.FillShowerMap(ev_mcshower,_shower_idmap);

    //_simch_map is (channel => larlight::simch)
    _simch_map.clear();
    _mcslb.FillSimchMap(ev_simch,_simch_map);

    std::vector<std::vector<larutil::PxHit> > local_clusters;

    _cru_helper.GeneratePxHit(storage,_cluster_type,local_clusters);

    _mgr.SetClusters(local_clusters);

    local_clusters.clear();

    if(_run_before_merging){
      
      //once that is done, _clusterparams is filled, calculate figure
      //of merit and fill histos with it
      _after_merging = false;
      FillFOMHistos(_after_merging,ev_mcshower,ev_cluster,ev_hits);
    }
    
    if(_run_merging){

      //now run merging with whatever algos you want
      //this overwites _clusterparams with the new (merged) clusters
      _mgr.Process();
      
      //now fill the other FOM histos
      _after_merging = true;
      FillFOMHistos(_after_merging,ev_mcshower,ev_cluster,ev_hits);
    }

    return true;
  }
  
  bool CMergePerformance::finalize() {
    
    if(_fout) { 
      _fout->cd(); 
      
      for(int nview = 0; nview < 3; ++nview){
	//only save "before" histos if you actually filled them
	if(_run_before_merging){
	  hPurity.at(0).at(nview)->Write();
	  hEff.at(0).at(nview)->Write();
	  hClusQoverMCQ.at(0).at(nview)->Write();
	  hEffPerMCShower.at(0).at(nview)->Write();
	  hPurityPerMCShower.at(0).at(nview)->Write();
	  hQFracInBigClusters.at(0).at(nview)->Write();
	}
	//only save "after" histos if you actually filled them
	if(_run_merging){
	  hPurity.at(1).at(nview)->Write();
	  hEff.at(1).at(nview)->Write();
	  hClusQoverMCQ.at(1).at(nview)->Write();
	  hEffPerMCShower.at(1).at(nview)->Write();
	  hPurityPerMCShower.at(1).at(nview)->Write();
	  hQFracInBigClusters.at(1).at(nview)->Write();
	}
	//delete all the histos, whether you filled them or not
	for(int before_after = 0; before_after < 2; ++before_after){	  
	  delete hPurity.at(before_after).at(nview);
	  delete hEff.at(before_after).at(nview);
	  delete hClusQoverMCQ.at(before_after).at(nview);
	  delete hPurityPerMCShower.at(before_after).at(nview);
	  delete hEffPerMCShower.at(before_after).at(nview);
	  delete hQFracInBigClusters.at(before_after).at(nview);
	}
      }
        
      if(ana_tree){
	ana_tree->Write();
	delete ana_tree;
      }
    }
    else 
      print(MSG::ERROR,__FUNCTION__,"Did not find an output file pointer!!! File not opened?");
      
    return true;
  }
  
  void CMergePerformance::PrepareHistos(){
    
    std::vector<TH1D*> tmp;
    tmp.clear();
    for(int i_view = 0; i_view < 3; i_view++)
      tmp.push_back(
		    new TH1D(Form("hPurity_before_merging_view%d",i_view),
			     Form("Clus Purity (ClusQ from Dom. MCShower / ClusQ from all MCShower [incl. unknown]) [before Merge], view %d",i_view),
			     100,-0.1,1.1)
		    );
    hPurity.push_back(tmp);
    
    tmp.clear();
    for(int i_view = 0; i_view < 3; i_view++)
      tmp.push_back(
		    new TH1D(Form("hPurity_after_merging_view%d",i_view),
			     Form("Clus Purity (ClusQ from Dom. MCShower / ClusQ from all MCShower [incl. unknown]) [after Merge], view %d",i_view),
			     100,-0.1,1.1)
		    );
    hPurity.push_back(tmp);
    
    tmp.clear();
    for(int i_view = 0; i_view < 3; i_view++)
      tmp.push_back(
		    new TH1D(Form("hPurityPerMCShower_before_merging_view%d",i_view),
			     Form("For *best* Cluster: Q in Clus from MCShower / Q of Cluster  [before merge], view %d",i_view),
			     100,-0.1,3.1)
		    );
    hPurityPerMCShower.push_back(tmp);
    
    tmp.clear();
    for(int i_view = 0; i_view < 3; i_view++)
      tmp.push_back(
		    new TH1D(Form("hPurityPerMCShower_after_merging_view%d",i_view),
			     Form("For *best* Cluster: Q in Clus from MCShower / Q of Cluster [after Merge], view %d",i_view),
			     100,-0.1,3.1)
		    );
    hPurityPerMCShower.push_back(tmp);
    
    tmp.clear();
    for(int i_view = 0; i_view < 3; i_view++)
      tmp.push_back(
		    new TH1D(Form("hEff_before_merging_view%d",i_view),
			     Form("# MCShowers / # Clusters per evt [before Merging], view %d",i_view), 
			     300,-0.1,3.1)
		    );
    hEff.push_back(tmp);
    
    tmp.clear();
    for(int i_view = 0; i_view < 3; i_view++)
      tmp.push_back(
		    new TH1D(Form("hEff_after_merging_view%d",i_view),
			     Form("# MCShowers / # Clusters per evt [after Merging], view %d",i_view), 
			     300,-0.1,3.1)
		    );
    hEff.push_back(tmp);

    tmp.clear();
    for(int i_view = 0; i_view < 3; i_view++)
      tmp.push_back(
		    new TH1D(Form("hEffPerMCShower_before_merging_view%d",i_view),
			     Form("Largest Frac of Shower Q in single Cluster, before, view %d",i_view), 
			     300,-0.1,3.1)
		    );
    hEffPerMCShower.push_back(tmp);
    
    tmp.clear();
    for(int i_view = 0; i_view < 3; i_view++)
      tmp.push_back(
		    new TH1D(Form("hEffPerMCShower_after_merging_view%d",i_view),
			     Form("Largest Frac of Shower Q in single Cluster, before, view %d",i_view), 
			     300,-0.1,3.1)
		    );
    hEffPerMCShower.push_back(tmp);

    tmp.clear();
    for(int i_view = 0; i_view < 3; i_view++)
      tmp.push_back(
		    new TH1D(Form("hClusQoverMCQ_before_merging_view%d",i_view),
			     Form("Cluster Charge / Dominant MCShower Charge [before Merging], view %d",i_view),
			     100,-0.1,10)
		    );
    hClusQoverMCQ.push_back(tmp);
   
    tmp.clear();
    for(int i_view = 0; i_view < 3; i_view++)
      tmp.push_back(
		    new TH1D(Form("hClusQoverMCQ_after_merging_view%d",i_view),
			     Form("Cluster Charge / Dominant MCShower Charge [after Merging], view %d",i_view),
			     100,-0.1,10)
		    );
    hClusQoverMCQ.push_back(tmp);
    
    
    tmp.clear();
    for(int i_view = 0; i_view < 3; i_view++)
      tmp.push_back(
		    new TH1D(Form("hQFracInBigClusters_beforemerging_view%d",i_view),
			     Form("Fraction of Total Q in Clusters with >10 Hits [before merging];Q Fraction per Event;Number of Events"),
			     102,-0.01,1.01)
		    );
    hQFracInBigClusters.push_back(tmp);
    
    
    tmp.clear();
    for(int i_view = 0; i_view < 3; i_view++)
      tmp.push_back(
		    new TH1D(Form("hQFracInBigClusters_aftermerging_view%d",i_view),
			     Form("Fraction of Total Q in Clusters with >10 Hits [after merging];Q Fraction per Event;Number of Events"),
			     102,-0.01,1.01)
		    );
    hQFracInBigClusters.push_back(tmp);
    
    /*
      std::vector<TH2D*> tmp2D;
      tmp2D.clear();
      for(int i_view = 0; i_view < 3; i_view++)
      tmp2D.push_back(
      new TH2D(Form("hPi0_photonanglediff_vs_Eff_view%d",i_view),
      Form("Pi0 Photon 2D Angle Difference vs. Efficiency [before merging], view %d",i_view),
      300,-0.1,3.1,
      100,0,180)
      );
      hPi0_photonanglediff_vs_Eff.push_back(tmp2D);
      
      tmp2D.clear();
      for(int i_view = 0; i_view < 3; i_view++)
      tmp2D.push_back(
      new TH2D(Form("hPi0_photonanglediff_vs_Eff_view%d",i_view),
      Form("Pi0 Photon 2D Angle Difference vs. Efficiency [after merging], view %d",i_view),
      300,-0.1,3.1,
      100,0,180)
      );
      hPi0_photonanglediff_vs_Eff.push_back(tmp2D);
    */
  }
  
  void CMergePerformance::PrepareTTree(){
    
    if(!ana_tree) {
      
      ana_tree = new TTree("ana_tree","");
      
      ana_tree->Branch("clusQfrac_over_totclusQ",&_clusQfrac_over_totclusQ,"clusQfrac_over_totclusQ/D");
      ana_tree->Branch("clusQ_over_MCQ",&_clusQ_over_MCQ,"clusQ_over_MCQ/D");
      ana_tree->Branch("tot_clusQ",&_tot_clus_charge,"tot_clus_charge/D");
      ana_tree->Branch("tot_clusQ_fromKnownMCS",&_tot_clus_charge_fromKnownMCS,"tot_clus_charge_fromKnownMCS/D");
      ana_tree->Branch("frac_clusQ",&_frac_clusQ,"frac_clusQ/D");
      ana_tree->Branch("dom_MCS_Q",&_dom_MCS_Q,"dom_MCS_Q/D");
      ana_tree->Branch("plane",&_plane,"plane/I");
      ana_tree->Branch("mother_energy",&_mother_energy,"mother_energy/D");
      ana_tree->Branch("nhits",&_nhits,"nhits/I");
      ana_tree->Branch("opening_angle",&_opening_angle,"opening_angle/D");
      ana_tree->Branch("clusQfrac_from_unknown",&_clusQfrac_from_unknown,"clusQfrac_from_unknown/D");
      ana_tree->Branch("after_merging",&_after_merging,"after_merging/O");
    }

  }

  void CMergePerformance::FillFOMHistos(bool after_merging,
					const event_mcshower* ev_mcshower,
					const event_cluster* ev_cluster,
					const event_hit* ev_hits)
  {
    //make a vector of MCShower indices 
    //McshowerLookback has a default 20MeV cut on these energies, don't have to do it here
    std::vector<UInt_t> MCShower_indices;
    for(UInt_t i=0; i < ev_mcshower->size(); ++i)
      MCShower_indices.push_back(i);
    
    //debug: view MCShower_indices
    if(_debug){
      std::cout<<"MCShower_indices = { ";
      for(int a = 0; a < MCShower_indices.size(); ++a)
	std::cout<<MCShower_indices.at(a)<<", ";
      std::cout<<"}"<<std::endl;
    }
    
    //stuff for calculating event charge fraction in clusters with > 10 hits
    double total_event_charge[3] = { 0., 0., 0. };
    double charge_in_small_clusters[3] = { 0., 0., 0. };
    
    
    //count the number of clusters for the denominator of efficiency plot
    //but DO NOT INCLUDE clusters that are 100% belonging to "unknown" MCShowers
    int n_clusters_in_plane[3] = { 0, 0, 0 };
    
    //Here we try to loop over clusters based on the _clusterparams vector, *not* ev_cluster vector,
    //because the merging algorithm returns an altered _clusterparams vector but it does *not*
    //modify the actual ev_cluster vector. 
    //Before merging, we make a vector of vectors like { {1}, {2}, {3}, ...}
    //where each entry is a vector of length 1, corresponding to the index in ev_cluster we are looping on
    //After merging, we retrieve this vector from CBookKeeper and it will look more like
    // { {1, 2}, {3}, {4, 5, 6, 7}, ...} showing which ev_cluster elements should be combined
    //That way, when we can easily combine the hits in different clusters to loop over,
    //as if the clusters were really merged
    
    //solution: get the bookkeeper instance and use the GetResult() function
    //this returns a vector of vector whose length = # of output clusters
    //(index number matches with the std::vector<CPAN> you get from CMergeManager::GetClusters()
    //the internal vector is the input clusters indices that were merged
    
    
    clus_idx_vec.clear();
    if(!after_merging){
      for(int i = 0; i < ev_cluster->size(); ++i){
	std::vector<unsigned short> temporary(1,i);
     	clus_idx_vec.push_back(temporary);
      }
    }
    else{
      clus_idx_vec = _mgr.GetBookKeeper().GetResult();
    }
    
    int cluster_params_idx = 0;
    //debug, viewing idx vec
    if(_debug){
      std::cout<<"after_merging is equal to "<<after_merging<<std::endl;
      std::cout<<"clus_idx_vec = { ";
      for(int a = 0; a < clus_idx_vec.size(); a++){
	std::cout<< "{ ";
	for(int b = 0; b < clus_idx_vec.at(a).size(); b++)
	  std::cout<< clus_idx_vec.at(a).at(b) << ", ";
	std::cout<<" }, ";
      }
      std::cout<<" } "<<std::endl;
    }
    
    
    //Hold max fraction of each MCShower's charge in a single cluster
    std::vector< std::vector<double> >  MCShower_best_clus(3, std::vector<double>(MCShower_indices.size()+1,0) );
    //Try and Keep track of Each MCShower's charge across the clusters
    std::vector< std::vector<double> >  MCShower_Qs(3, std::vector<double>(MCShower_indices.size()+1,0) );
    //Keep track of total Q of the cluster that has largest fract of MCShower Q
    std::vector< std::vector<double> > BestClus_Qs(3, std::vector<double>(MCShower_indices.size()+1,0) );
    
    
    ///////////////////////////////////////////////
    //Loop over reconstructed clusters
    ///////////////////////////////////////////////
    if(_debug){
      std::cout<<"after_merging? = "<<after_merging
	       <<". This event has "<<clus_idx_vec.size()
	       <<" clusters and "<<MCShower_indices.size()
	       <<" viable MCShowers"<<std::endl;
    }
    
    for(int outer_clus_idx=0; outer_clus_idx<clus_idx_vec.size(); ++outer_clus_idx){
      //total cluster charge
      _tot_clus_charge = 0;
      _tot_clus_charge_fromKnownMCS = 0;
      _plane = ev_cluster->at(clus_idx_vec.at(outer_clus_idx).at(0)).View();
      
      //charge from each MCShower object... last element is from unknown MCShower
      //or from MCShowers that were too low energy to pass to McshowerLookback
      std::vector<double> part_clus_charge(MCShower_indices.size()+1,0);
      
      //container to hold the fractional hit charge returned by McshowerLookback
      std::vector<float> hit_charge_frac;
      
      //Get the hits from each cluster and loop over them
      //if after merging, this may be hits from multiple clusters
      //combine the assiciations from all merged clusters into one vector of hit indices to loop over
      //container to hold the final list of hit indices for this(these) cluster(s)
      std::vector<unsigned short> ass_index;
      ass_index.clear();
      //loop over this(these) cluster(s) index(indices)
      for(int in_clus_idx = 0; in_clus_idx < clus_idx_vec.at(outer_clus_idx).size(); ++in_clus_idx){
	//grab the hit associations for each cluster
	auto const& tmp_ass_index(ev_cluster->at(clus_idx_vec.at(outer_clus_idx).at(in_clus_idx)).association(hit_type));
	// and concatenate it to ass_index
	ass_index.insert( ass_index.end(), tmp_ass_index.begin(), tmp_ass_index.end() );
      }
      
      
      //debug: view ass_index
      if(_debug){
	std::cout<<"for this cluster, ass_index is {";
	for(int a = 0; a < ass_index.size(); a++)
	  std::cout<<ass_index.at(a)<<",";
	std::cout<< "}"<<std::endl;
      }
      
      ///////////////////////////////////////////////
      //Loop over hits in this cluster 
      ///////////////////////////////////////////////
      if(_debug)
	std::cout<<"looping over ass_index. n_hits is "<<ass_index.size()<<std::endl;
      for(auto const hit_index: ass_index){
	if(_debug)
	  std::cout<<"hit_index is "<<hit_index<<std::endl;
	
	_tot_clus_charge += ev_hits->at(hit_index).Charge();
	
	hit_charge_frac.clear();
	
	hit_charge_frac = 
	  _mcslb.MatchHitsAll(ev_hits->at(hit_index),
			      _simch_map,
			      _shower_idmap,
			      MCShower_indices);
	
	
	//debug
	if(_debug){
	  std::cout<<"Debug: just got back hit_charge_frac as (";
	  for(int j = 0; j < hit_charge_frac.size(); ++j)
	    std::cout<<hit_charge_frac.at(j)<<", ";
	  std::cout<<")."<<std::endl;
	}
	
	
	//sometimes mcslb returns a null vector if the reco hit couldn't be matched with an IDE at all... this rarely occurs
	//in this case, add 100% of this hit's charge to the "unknown" index
	if(hit_charge_frac.size() == 0){
	  part_clus_charge.back() += (1.)*ev_hits->at(hit_index).Charge();
	  
	  if(_debug){
	    std::cout<<"warning, mcslb returned a null vector"<<std::endl;
	    std::cout<<"event number is "<<ev_hits->event_id()<<std::endl;
	    std::cout<<"this hit's startTime, endTime is "<<ev_hits->at(hit_index).StartTime()
		     <<", "<<ev_hits->at(hit_index).EndTime()<<std::endl;
	    std::cout<<"this hit's channel is "<<ev_hits->at(hit_index).Channel()<<std::endl;
	    std::cout<<"this hits' charge is "<<ev_hits->at(hit_index).Charge()<<std::endl;
	  }
	}
	else{
	  //mcslb has found an IDE, whether it be in an MCShower, or unknown
	  for(int i = 0; i < hit_charge_frac.size(); ++i)
	    part_clus_charge.at(i) += hit_charge_frac.at(i)*ev_hits->at(hit_index).Charge();
	  //count total charge only from known MCShowers (hence -1)
	  for(int i = 0; i < hit_charge_frac.size() - 1; ++i)
	    _tot_clus_charge_fromKnownMCS += hit_charge_frac.at(i)*ev_hits->at(hit_index).Charge();
	  
	}
	
	if(_debug){
	  std::cout<<"This hit is made up of (";
	  for(int i=0; i<hit_charge_frac.size()-1; ++i)
	    std::cout<<hit_charge_frac.at(i)<<", ";
	  std::cout<<") fractional charge from "<<
	    hit_charge_frac.size()-1<<" known MCShowers and "<<
	    (float)hit_charge_frac.back()<<" from unknown ones."<<std::endl;
	}
	
	
      } //end looping over hits in this cluster
      
      
      //debug: view part_clus_charge vector
      if(_debug){
	std::cout<<"part_clus_charge vector = { ";
	for(int a = 0; a < part_clus_charge.size(); ++a)
	  std::cout<<part_clus_charge.at(a)<<", ";
	std::cout<< "}"<<std::endl;
	std::cout<<"total charge is "<<_tot_clus_charge
		 <<" while totQ from known is "<<_tot_clus_charge_fromKnownMCS
		 <<std::endl;
	
	//debug: print contents of _shower_idmap map
	for(std::map<UInt_t,UInt_t>::iterator it=_shower_idmap.begin(); it!=_shower_idmap.end(); ++it)
	  std::cout<<"viewing whoer_idmap: "<<it->first << " => "<<it->second <<std::endl;
      }
      
      ///////////////////////////////////////////////
      // Fill histograms/tree that need once-per-cluster filling
      ///////////////////////////////////////////////
      
      //if all of the charge in the cluster is from unknown MCShower, skip this cluster   
      if(_tot_clus_charge_fromKnownMCS == 0) continue;
      else n_clusters_in_plane[_plane]++;
      
      //calculate _nhits for ttree
      _nhits = (int)ass_index.size();
      
      //stuff for calculating event charge fraction in clusters with >10 hits
      total_event_charge[_plane] += _tot_clus_charge;
      if(_nhits<10) charge_in_small_clusters[_plane] += _tot_clus_charge;
      
      //if _nhits is less than 15, don't fill this clusters' info into histos and ttree
      if(_nhits<15) continue;
      
      //purity of a cluster: highest charge fraction belonging to an MCShower
      //find the MCShower that most of this cluster belongs to & fill some TTree vars
      //don't count "unknown" showers. looks like lots of clusters are 100% pure from
      //"unknown" showers, but many "unknown" things all combine into this one object
      int dominant_MCshower_index = 0;
      _clusQfrac_over_totclusQ = 0;
      _frac_clusQ = 0;
      
      
      //ignore unknown MCShowers entirely
      for(int i = 0; i < part_clus_charge.size() - 1; ++i){	
	if( (double)(part_clus_charge.at(i)/_tot_clus_charge_fromKnownMCS) > _clusQfrac_over_totclusQ){
	  _clusQfrac_over_totclusQ = (double)(part_clus_charge.at(i)/_tot_clus_charge_fromKnownMCS);
	  _frac_clusQ = part_clus_charge.at(i);
	  dominant_MCshower_index = i;
	}
      }
      
      for (int i=0; i < part_clus_charge.size() - 1; i++){
	//std::cout << "Cluster's Charge from MC Shower " << i << " : " << part_clus_charge.at(i) << std::endl;
	MCShower_Qs.at(_plane).at(i) += part_clus_charge.at(i);
	if ( part_clus_charge.at(i) > MCShower_best_clus.at(_plane).at(i) ) {
	  MCShower_best_clus.at(_plane).at(i) = part_clus_charge.at(i);
	  BestClus_Qs.at(_plane).at(i) = (double)(part_clus_charge.at(i)/_tot_clus_charge_fromKnownMCS);
	}
      }
      
      
      hPurity.at(after_merging).at(_plane)->Fill(_clusQfrac_over_totclusQ);
      
      _clusQfrac_from_unknown = part_clus_charge.back()/_tot_clus_charge;
      
      if(_debug){
	if(_clusQfrac_over_totclusQ < 0.50){
	  std::cout<<"_clusQfraC_over_totclusQ is <0.50"<<std::endl;
	  std::cout<<"part_clus_charge vector = { ";
	  for(int a = 0; a < part_clus_charge.size(); ++a)
	    std::cout<<part_clus_charge.at(a)<<", ";
	  std::cout<< "}"<<std::endl;
	  std::cout<<"_nhits is "<<_nhits<<std::endl;
	  std::cout<<"MCShower_indices = { ";
	  for(int a = 0; a < MCShower_indices.size(); ++a)
	    std::cout<<MCShower_indices.at(a)<<", ";
	  std::cout<<"}"<<std::endl;
	}
      }
      //"other purity" :
      //cluster charge divided by charge of dominant MC shower
      //if dominant_index points to the "unknown" element, set ratio to -1
      
      if(dominant_MCshower_index == ev_mcshower->size()){
	
	_dom_MCS_Q = -1;
	
	_clusQ_over_MCQ = -1;
      }
      else{
	//total charge of the dominant MC Shower for this cluster
	_dom_MCS_Q = ev_mcshower->at(dominant_MCshower_index).Charge(_plane);
	
	_clusQ_over_MCQ = 
	  _tot_clus_charge / _dom_MCS_Q;
	
      }
      hClusQoverMCQ.at(after_merging).at(_plane)->Fill(_clusQ_over_MCQ);
      
      //take mother energy (ttree var) to be max of MCShower mother energies
      _mother_energy = 0;
      for(auto this_mcshow : *ev_mcshower){
	double this_mother_energy = 0;
	for(int i = 0; i < 3; ++i)
	  this_mother_energy += pow(this_mcshow.MotherMomentum().at(i),2);
	this_mother_energy = pow(this_mother_energy, 0.5);
	
	if(this_mother_energy > _mother_energy)
	  _mother_energy = this_mother_energy;
      }
      
      //std::cout<<"after_merging is "<<after_merging<<", outer_clus_idx is "<<outer_clus_idx<<", size of _clusterparms is "<<_clusterparams.size()<<", size of clud_idx_vec is "<<clus_idx_vec.size()<<std::endl;
      //calculate opening_angle for ttree
      //get this from the clusterparams
      
      if(after_merging)
	_opening_angle = _mgr.GetClusters().at(cluster_params_idx).GetParams().opening_angle;
      else
	_opening_angle = _mgr.GetInputClusters().at(cluster_params_idx).GetParams().opening_angle;	
      //Fill ana TTree once per cluster
      if(ana_tree)
	ana_tree->Fill();
      
      //make sure to only increment cluster_params_idx (where in the _clusterparams vector this cluster is)
      //after ditching clusters with <15 hits
      cluster_params_idx++;
      
      
    }//end loop over clusters
    
    
    ///////////////////////////////////////////////
    // Fill histograms/tree that need once-per-event filling
    ///////////////////////////////////////////////
    
    //stuff for calculating event charge fraction in clusters with > 10 hits
    for (int iplane = 0; iplane < 3; ++iplane)
      hQFracInBigClusters.at(after_merging).at(iplane)->Fill((total_event_charge[iplane]-charge_in_small_clusters[iplane])/total_event_charge[iplane]);
    
    
    ///////////
    // Per MCShower Efficiency & Purity
    ///////////
    for (int iplane = 0; iplane < 3; ++iplane){
      for (int i=0; i<MCShower_indices.size(); i++){
	//std::cout << "Charge of MCShower in Cluster - max: " << MCShower_best_clus_pl0.at(i) << std::endl;
	//std::cout << "Charge of MCShower Total: " <<MCShower_Qs_pl0.at(i) << std::endl;
	MCShower_best_clus.at(iplane).at(i) /= MCShower_Qs.at(iplane).at(i);
      //std::cout << "Fraction of MCShower's Q in that Cluster: " << MCShower_best_clus_pl0.at(i) << std::endl;
      //BestClus_Qs_pl0.at(i) /= MCShower_Qs_pl0.at(i);
	hEffPerMCShower.at(after_merging).at(iplane)->Fill(MCShower_best_clus.at(iplane).at(i));
      //std::cout << "Fraction of Cluster's Q made up by the Q of that MCShower " << BestClus_Qs_pl0.at(i) << std::endl;      
	hPurityPerMCShower.at(after_merging).at(iplane)->Fill(BestClus_Qs.at(iplane).at(i));
      }
    }


    ///////////
    //efficiency of clustering for the event, per plane
    ///////////

    //count how many MCShowers are above ~20 MeV.
    double n_viable_MCShowers = 0;
    for(auto this_mcshow : *ev_mcshower){
      double this_mother_energy = 0;
      for(int i = 0; i < 3; ++i)
	this_mother_energy += pow(this_mcshow.MotherMomentum().at(i),2);
      this_mother_energy = pow(this_mother_energy, 0.5);
      if(this_mother_energy > _MCShower_mother_energy_cutoff)
	n_viable_MCShowers++; 
    }

    //then loop over 3 planes and fill hEff with the right fraction
    for (int iplane = 0; iplane < 3; ++iplane){

      double NMCSoverNClus = 
	n_viable_MCShowers/(double)n_clusters_in_plane[iplane];
      hEff.at(after_merging).at(iplane)->Fill(NMCSoverNClus);

      //if there are exactly 2 MCShowers in the event, fill some pi0 relevant stuff per plane
      //this needs debugging
      /*
      double angle2D[2] = {999.,999.};
      if(n_viable_MCShowers == 2){
	int blah = 0;
	for(auto this_mcshow : *ev_mcshower){
	  double this_mother_energy = 0;
	  for(int i = 0; i < 3; ++i)
	    this_mother_energy += pow(this_mcshow.MotherMomentum().at(i),2);
	  this_mother_energy = pow(this_mother_energy, 0.5);
	  
	  if(this_mother_energy > _MCShower_mother_energy_cutoff){
	    angle2D[blah]=this_mcshow.MotherAngle2D((larlight::GEO::View_t)iplane);
	  }
	  //	    std::cout<<"blah "<<this_mcshow.MotherAngle2D((larlight::GEO::View_t)iplane)<<std::endl;
	  blah++;
	}
	double angle2Ddiff = std::abs(angle2D[0]-angle2D[1]);
	hPi0_photonanglediff_vs_Eff.at(after_merging).at(iplane)->Fill(NMCSoverNClus,angle2Ddiff);
	//	std::cout<<"filling 2d shit with "<<NMCSoverNClus<<", "<<angle2Ddiff<<std::endl;
      }
      */

      //fraction of total charge in plane in big clusters
      
      
    }//end loop over planes
    
  }//end fillFOMhistos

}
#endif
