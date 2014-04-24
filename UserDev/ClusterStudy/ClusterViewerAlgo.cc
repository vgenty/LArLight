#ifndef CLUSTERVIEWERALGO_CC
#define CLUSTERVIEWERALGO_CC

#include "ClusterViewerAlgo.hh"

namespace cluster {
  //####################################
  ClusterViewerAlgo::ClusterViewerAlgo()
  //####################################
  {
    _nplanes = larutil::Geometry::GetME()->Nplanes();
    _cAllCluster = new TCanvas("cAllCluster","Wire vs. Time Cluster Viewer",900,600);
    _cAllCluster->Divide(2,_nplanes);
    _cOneCluster = new TCanvas("cOneCluster","Individual Cluster Start/End Points",500,300);

  }

  //#############################
  void ClusterViewerAlgo::Reset()
  //#############################
  {
    // delete
    for(auto &h : _hAllHits)        { delete h; h=0; }

    for(auto &hs : _hClusterHits)
      for(auto &h : hs)
	{ delete h; h=0; }

    for(auto &gs : _gClusterStart)
      for(auto &g : gs)
	{ delete g; g=0; }

    for(auto &gs : _gClusterEnd)
      for(auto &g : gs)
	{ delete g; g=0; }

    for(auto &gs : _gClusterPolygon)
      for(auto &g : gs)
	{ delete g; g=0; }

    // clear
    _hAllHits.clear();
    _hClusterHits.clear();
    _gClusterStart.clear();
    _gClusterEnd.clear();
    _gClusterPolygon.clear();
    _xrange.clear();
    _yrange.clear();
    _range_set.clear();

    // resize
    _hAllHits.resize(_nplanes,nullptr);
    _hClusterHits.resize(_nplanes,std::vector<TH2D*>());
    _gClusterStart.resize(_nplanes,std::vector<TGraph*>());
    _gClusterEnd.resize(_nplanes,std::vector<TGraph*>());
    _gClusterPolygon.resize(_nplanes,std::vector<TGraph*>());

    _xrange.resize(_nplanes,std::pair<double,double>(0,0));
    _yrange.resize(_nplanes,std::pair<double,double>(0,0));
    _range_set.resize(_nplanes,false);
  }


  //######################################################################
  void ClusterViewerAlgo::SetRange(UChar_t plane, 
				   const std::pair<double,double> &xrange,
				   const std::pair<double,double> &yrange)
  //######################################################################
  {
    if(plane >= _nplanes)
      throw ViewerException(Form("Invalid plane ID: %d",plane));

    if(ReadyTakeData())
      throw ViewerException("SetRange() can be called only after Reset()!");

    _xrange.at(plane) = xrange;
    _yrange.at(plane) = yrange;
    _range_set.at(plane) = true;
  }

  //#####################################################
  bool ClusterViewerAlgo::ReadyTakeData(bool raise) const
  //#####################################################
  {
    bool status = true;
    for(auto b : _range_set) status = status && b;

    if(raise)
      throw ViewerException("X and Y min/max range not yet set for all planes!");

    return status;
  }

  //#################################################################################
  void ClusterViewerAlgo::AddHits(const UChar_t plane, 
				  const std::vector<std::pair<double,double> > &hits)
  //#################################################################################
  {
    if(!ReadyTakeData(true)) return;
    if(plane >= _nplanes)
      throw ViewerException(Form("Invalid plane ID: %d",plane));

    if(_hAllHits.at(plane)) { delete _hAllHits.at(plane); _hAllHits.at(plane) = nullptr; }

    _hAllHits.at(plane) = GetPlaneViewHisto(plane,
					    Form("hAllHitsPlane%02d",plane),
					    Form("Hits for Plane %d; Wire; Time",plane));
    for(auto const& h : hits) 

      _hAllHits.at(plane)->Fill(h.first,h.second);
  }

  //############################################################################################
  void ClusterViewerAlgo::AddCluster(const UChar_t plane,
				     const std::vector<std::pair<double,double> > &cluster_hits)
  //############################################################################################
  {
    if(!ReadyTakeData(true)) return;
    if(plane >= _nplanes)
      throw ViewerException(Form("Invalid plane ID: %d",plane));

    size_t index = _hClusterHits.at(plane).size();

    // Create
    _hClusterHits.at(plane).push_back(GetPlaneViewHisto(plane,
							Form("hClusterHitsPlane%02dCluster%03zu",
							     plane,
							     index),
							Form("Cluster Hits on Plane %d Cluster %zu; Wire; Time",
							     plane,
							     index)
							)
				      ); 

    _gClusterStart.at(plane).push_back(nullptr);

    _gClusterEnd.at(plane).push_back(nullptr);
    
    _gClusterPolygon.at(plane).push_back(nullptr);

    // Fill
    for(auto const &h : cluster_hits)
      
      _hClusterHits.at(plane).at(index)->Fill(h.first,h.second);
    
    // Set Color
    _hClusterHits.at(plane).at(index)->SetMarkerColor(index);
    _hClusterHits.at(plane).at(index)->SetMarkerStyle(kFullStar);

  }

  //############################################################################################
  void ClusterViewerAlgo::AddCluster(const UChar_t plane,
				     const std::vector<std::pair<double,double> > &cluster_hits,
				     const std::pair<double,double> &cluster_start,
				     const std::pair<double,double> &cluster_end)
  //############################################################################################
  {
    if(!ReadyTakeData(true)) return;
    if(plane >= _nplanes)
      throw ViewerException(Form("Invalid plane ID: %d",plane));

    size_t index = _hClusterHits.at(plane).size();

    // Create
    _hClusterHits.at(plane).push_back(GetPlaneViewHisto(plane,
							Form("hClusterHitsPlane%02dCluster%03zu",
							     plane,
							     index),
							Form("Cluster Hits on Plane %d Cluster %zu; Wire; Time",
							     plane,
							     index)
							)
				      ); 

    _gClusterStart.at(plane).push_back(GetPlaneViewGraph(plane,
							 Form("gClusterStartPlane%02dCluster%03zu",plane,index),
							 ""
							 )
				       );
    
    _gClusterEnd.at(plane).push_back(GetPlaneViewGraph(plane,
						       Form("gClusterEndPlane%02dCluster%03zu",plane,index),
						       ""
						       )
				     );
    
    _gClusterPolygon.at(plane).push_back(nullptr);

    // Fill
    for(auto const &h : cluster_hits)
      
      _hClusterHits.at(plane).at(index)->Fill(h.first,h.second);
    
    _gClusterStart.at(plane).at(index)->SetPoint(0,cluster_start.first,cluster_start.second);
    _gClusterEnd.at(plane).at(index)->SetPoint(0,cluster_end.first,cluster_end.second);
    
    // Set Color
    _hClusterHits.at(plane).at(index)->SetMarkerColor(index);
    _hClusterHits.at(plane).at(index)->SetMarkerStyle(kFullStar);
    _gClusterStart.at(plane).at(index)->SetMarkerSize(3);
    _gClusterStart.at(plane).at(index)->SetMarkerStyle(30);
    _gClusterEnd.at(plane).at(index)->SetMarkerSize(3);
    _gClusterEnd.at(plane).at(index)->SetMarkerStyle(29);
    
  }
  
  //###############################################################################################
  void ClusterViewerAlgo::AddCluster(const UChar_t plane,
				     const std::vector<std::pair<double,double> > &cluster_hits,
				     const std::pair<double,double> &cluster_start,
				     const std::pair<double,double> &cluster_end,
				     const std::vector<std::pair<double,double> > &cluster_polygon)
  //###############################################################################################
  {
    if(!ReadyTakeData(true)) return;
    if(plane >= _nplanes)
      throw ViewerException(Form("Invalid plane ID: %d",plane));

    size_t index = _hClusterHits.at(plane).size();

    // Create
    _hClusterHits.at(plane).push_back(GetPlaneViewHisto(plane,
							Form("hClusterHitsPlane%02dCluster%03zu",
							     plane,
							     index),
							Form("Cluster Hits on Plane %d Cluster %zu; Wire; Time",
							     plane,
							     index)
							)
				      ); 

    _gClusterStart.at(plane).push_back(GetPlaneViewGraph(plane,
							 Form("gClusterStartPlane%02dCluster%03zu",plane,index),
							 ""
							 )
				       );
    
    _gClusterEnd.at(plane).push_back(GetPlaneViewGraph(plane,
						       Form("gClusterEndPlane%02dCluster%03zu",plane,index),
						       ""
						       )
				     );
    
    _gClusterPolygon.at(plane).push_back(GetPlaneViewGraph(plane,
							   Form("gClusterPolygonPlane%02dCluster%03zu",plane,index),
							   ""
							   )
					 );

    // Fill
    for(auto const &h : cluster_hits)
      
      _hClusterHits.at(plane).at(index)->Fill(h.first,h.second);
    
    _gClusterStart.at(plane).at(index)->SetPoint(0,cluster_start.first,cluster_start.second);
    _gClusterEnd.at(plane).at(index)->SetPoint(0,cluster_end.first,cluster_end.second);
    
    for(size_t i=0; i<cluster_polygon.size(); ++i)
      
      _gClusterPolygon.at(plane).at(index)->SetPoint(i,
						     cluster_polygon.at(i).first,
						     cluster_polygon.at(i).second);
    // Set Color
    _hClusterHits.at(plane).at(index)->SetMarkerColor(index);
    _hClusterHits.at(plane).at(index)->SetMarkerStyle(kFullStar);
    _gClusterStart.at(plane).at(index)->SetMarkerSize(3);
    _gClusterStart.at(plane).at(index)->SetMarkerStyle(30);
    _gClusterEnd.at(plane).at(index)->SetMarkerSize(3);
    _gClusterEnd.at(plane).at(index)->SetMarkerStyle(29);
    _gClusterPolygon.at(plane).at(index)->SetMarkerColor(index);
    _gClusterPolygon.at(plane).at(index)->SetMarkerStyle(20);
  }

  //###############################################################################################
  void ClusterViewerAlgo::AddCluster(const UChar_t plane,
				     const std::vector<std::pair<double,double> > &cluster_hits,
				     const std::vector<std::pair<double,double> > &cluster_polygon)
  //###############################################################################################
  {
    if(!ReadyTakeData(true)) return;
    if(plane >= _nplanes)
      throw ViewerException(Form("Invalid plane ID: %d",plane));

    size_t index = _hClusterHits.at(plane).size();

    // Create
    _hClusterHits.at(plane).push_back(GetPlaneViewHisto(plane,
							Form("hClusterHitsPlane%02dCluster%03zu",
							     plane,
							     index),
							Form("Cluster Hits on Plane %d Cluster %zu; Wire; Time",
							     plane,
							     index)
							)
				      ); 

    _gClusterStart.at(plane).push_back(nullptr);

    _gClusterEnd.at(plane).push_back(nullptr);
    
    _gClusterPolygon.at(plane).push_back(GetPlaneViewGraph(plane,
							   Form("gClusterPolygonPlane%02dCluster%03zu",plane,index),
							   ""
							   )
					 );

    // Fill
    for(auto const &h : cluster_hits)
      
      _hClusterHits.at(plane).at(index)->Fill(h.first,h.second);
    
    for(size_t i=0; i<cluster_polygon.size(); ++i)
      
      _gClusterPolygon.at(plane).at(index)->SetPoint(i,
						     cluster_polygon.at(i).first,
						     cluster_polygon.at(i).second);
    // Set Color
    _hClusterHits.at(plane).at(index)->SetMarkerColor(index);
    _hClusterHits.at(plane).at(index)->SetMarkerStyle(kFullStar);
    _gClusterPolygon.at(plane).at(index)->SetMarkerColor(index);
    _gClusterPolygon.at(plane).at(index)->SetMarkerStyle(20);
  }

  //########################################################################
  TH2D* ClusterViewerAlgo::GetPlaneViewHisto(const UChar_t plane,
					     const std::string &name, 
					     const std::string &title) const
  //########################################################################
  {
    if(plane >= _nplanes) throw ViewerException(Form("Invalid plane ID: %d",plane));
    TH2D* h = new TH2D(name.c_str(), title.c_str(),
		       100, _xrange.at(plane).first * 0.9, _xrange.at(plane).second * 1.1,
		       100, _yrange.at(plane).first * 0.9, _yrange.at(plane).second * 1.1);
    return h;
  }
  
  
  //##########################################################################
  TGraph* ClusterViewerAlgo::GetPlaneViewGraph(const UChar_t plane,
					       const std::string &name, 
					       const std::string &title) const
  //##########################################################################
  {
    if(plane >= _nplanes) throw ViewerException(Form("Invalid plane ID: %d",plane));
    TGraph* g = new TGraph;
    g->Set(0);
    g->GetXaxis()->SetRangeUser(_xrange.at(plane).first * 0.9, _xrange.at(plane).second * 1.1);
    g->SetMaximum(_yrange.at(plane).second * 1.1);
    g->SetMinimum(_yrange.at(plane).first * 0.9);
    g->SetTitle(title.c_str());
    g->SetName(name.c_str());
    return g;
  }

  //#############################################
  void ClusterViewerAlgo::DrawAllClusters() const
  //#############################################
  {
    _cAllCluster->cd();
    for(UChar_t plane=0; plane<_nplanes; ++plane) {
      
      _cAllCluster->cd((plane*2 + 1));
      for(size_t cindex=0; cindex<_hClusterHits.at(plane).size(); ++cindex) {

	if(!cindex) _hClusterHits.at(plane).at(cindex)->Draw();
	else  _hClusterHits.at(plane).at(cindex)->Draw("sames");

      }

      _cAllCluster->cd((plane+1)*2);
      _hAllHits.at(plane)->Draw("COLZ");

    }
    _cAllCluster->Update();
  }

  //#######################################################################
  void ClusterViewerAlgo::DrawOneCluster(UChar_t plane, size_t index) const
  //#######################################################################
  {
    if(plane >= _nplanes) throw ViewerException(Form("Invalid plane ID: %d",plane));

    if(index >= _hClusterHits.at(plane).size()) throw ViewerException(Form("Invalid cluster index: %zu (for plane %d)",
									   index,
									   plane)
								      );
    _cOneCluster->cd();
    
    _hClusterHits.at(plane).at(index)->Draw();

    if( _gClusterStart.at(plane).at(index)   ) _gClusterStart.at(plane).at(index)->Draw("AP");
    if( _gClusterEnd.at(plane).at(index)     ) _gClusterEnd.at(plane).at(index)->Draw("P");
    if( _gClusterPolygon.at(plane).at(index) ) _gClusterPolygon.at(plane).at(index)->Draw("PL");

  }
  
}
#endif
