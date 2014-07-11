#ifndef CFALGO3DANGLE_CXX
#define CFALGO3DANGLE_CXX

#include "CFAlgo3DAngle.hh"

namespace cmtool {

  //-------------------------------------------------------
  CFAlgo3DAngle::CFAlgo3DAngle() : CFloatAlgoBase()
  //-------------------------------------------------------
  {
	_theta_cut = 70 ;
	_phi_cut   = 70 ;
	_ratio_cut = 0.01;

  }

  //-----------------------------
  void CFAlgo3DAngle::Reset()
  //-----------------------------
  {

  }

  //----------------------------------------------------------------------------------------------
  float CFAlgo3DAngle::Float(const std::vector<const cluster::ClusterParamsAlgNew*> &clusters)
  //----------------------------------------------------------------------------------------------
  {

	int plane_0 = clusters.at(0)->GetParams().start_point.plane ;
	int plane_1 = clusters.at(1)->GetParams().start_point.plane ;
	int plane_2 = clusters.at(2)->GetParams().start_point.plane ;

	double angle_2d_0 = clusters.at(0)->GetParams().angle_2d;
	double angle_2d_1 = clusters.at(1)->GetParams().angle_2d;
	double angle_2d_2 = clusters.at(2)->GetParams().angle_2d;

	//Calculate angles theta and phi for cluster pairs across 2 planes
	double phi_01    = 0;   	
	double theta_01  = 0;
	double phi_12    = 0;   
	double theta_12  = 0;

	double max_phi, min_phi	 = 0;
	double max_theta, min_theta = 0; 

	auto hits_0 = clusters.at(0)->GetParams().N_Hits ;
	auto hits_1 = clusters.at(1)->GetParams().N_Hits ;
	auto hits_2 = clusters.at(2)->GetParams().N_Hits ;
	
	//For ordering hits and rejecting pairs which have a large difference in hit number
	double max(0), middle(0), min(0) ;

	//Calculate phi and theta from first 2 planes; check if third plane is consistent
	larutil::GeometryUtilities::GetME()->Get3DaxisN(plane_0,plane_1,angle_2d_0,angle_2d_1,phi_01,theta_01);	
	larutil::GeometryUtilities::GetME()->Get3DaxisN(plane_1,plane_2,angle_2d_1,angle_2d_2,phi_12,theta_12);	

	//Adjust the range of phis/thetas that are bigger than 360 or less than 0.
	FixPhiTheta(phi_01,theta_01);
	FixPhiTheta(phi_12,theta_12);
	
	//void function that sets max and min phi and theta for ratio calculation later
	SetMaxMin(phi_01,phi_12,max_phi,min_phi);
	SetMaxMin(theta_01,theta_12,max_theta,min_theta);


	//Order hits from most to least 	
	SortHits(hits_0,hits_1,hits_2,max,middle,min);
	
	double ratio_phi 	= 1;
	double ratio_theta  = 1;
	double ratio_max_min = 1;
	double ratio_max_middle =1;
	double ratio 		= 1;		

	//This takes into account the fact that 0 and 360 having the same relative value (for both theta and phi)
	if(min_phi + 360 < max_phi +_phi_cut && min_phi +360 > max_phi - _phi_cut)
	 {	
		min_phi +=360 ;
		SetMaxMin(min_phi,max_phi,max_phi,min_phi); 
	  }
		
	 if(min_theta + 360 < max_theta +_theta_cut && min_theta +360 > max_theta -  _theta_cut)
	  {	
		min_theta +=360 ;
		SetMaxMin(min_theta,max_theta,max_theta,min_theta); 
	   }
	//Test to make sure that max hits is not too much bigger than min
	if(max > min + 500 )
		ratio *=0.001 ;
	else{
		ratio_phi = min_phi / max_phi ;
		ratio_theta = min_theta/ max_theta ;
		ratio_max_min = min / max ;
		ratio_max_middle = middle/max ;	
		ratio = ratio_phi* ratio_theta * ratio_max_min * ratio_max_middle ;

		}	

	if(_verbose && ratio > _ratio_cut ){	
		std::cout<<"\nTotal ratio is: " <<ratio<<" theta: "<<ratio_theta<<" phi: "<<ratio_phi ;
		std::cout<<"\nNhits planes 0, 1, 2: " <<clusters.at(0)->GetParams().N_Hits<<", "<<clusters.at(1)->GetParams().N_Hits
				 <<", "<<clusters.at(2)->GetParams().N_Hits ;
		std::cout<<"\nAdjusted theta and phi : "<<max_theta<<", "<<min_theta<<", "<<max_phi<<", "<<min_phi;
		std::cout<<"\nFor planes 0 and 1: "<<std::endl ;
		std::cout<<"\tPhi : "<<phi_01<<std::endl;
		std::cout<<"\tTheta : "<<theta_01<<std::endl ;
		std::cout<<"For planes 1 and 2: "<<std::endl ;
		std::cout<<"\tPhi : "<<phi_12<<std::endl;
		std::cout<<"\tTheta : "<<theta_12<<std::endl ;
	}
	
	
	return(ratio > _ratio_cut ? ratio : -1) ;
  }

 //--------------------------------
  void CFAlgo3DAngle::FixPhiTheta(double &phi, double &theta)
 //--------------------------------	
   {
		while(phi <= 0)
			phi +=360 ;
		while(phi > 360)
			phi -= 360 ;

		while(theta <= 0)
			theta += 360 ;
	    while(theta > 360)
			theta-=360;
	
	
	}	

  //------------------------------
  void CFAlgo3DAngle::SetMaxMin(const double angle_1, const double angle_2, double &max_angle, double &min_angle)
  //------------------------------
	{

		if(angle_1 > angle_2){
			max_angle = angle_1 ;
			min_angle = angle_2 ;
		 }
		else{
			max_angle = angle_2;
			min_angle	= angle_1 ;
		  }
   }

  //------------------------------
  void CFAlgo3DAngle::SortHits(const double hits_0, const double hits_1, const double hits_2, double &max, double &middle, double &min) 
  //------------------------------
  {

	if(hits_0 > hits_1 && hits_0 > hits_2){
		max = hits_0;
		}
	else if (hits_0 > hits_1 && hits_0 < hits_2){
		max = hits_2 ;
		middle = hits_0 ;
		min = hits_1 ;
		}
	else if (hits_0 > hits_2 && hits_0 < hits_1){
		max = hits_1 ;
		middle = hits_0 ;
		min = hits_2 ; 
		}
	else if(hits_0 <hits_1 && hits_0 <hits_2)
		min = hits_0 ;
	

	if (max == hits_0 && hits_1 > hits_2){
		middle = hits_1 ; 
		min = hits_2    ;
		}
	else if (max ==hits_0 && hits_2 > hits_1){
		middle = hits_2 ;	
		min = hits_1 	;
		}

	if(min ==hits_0 && hits_1 > hits_2){
		middle = hits_2 ;
		max	= hits_1;
		}
	else if(min ==hits_0 && hits_2 > hits_1){
		middle = hits_1 ;
		max = hits_2 ;
	    }
}

  //------------------------------
  void CFAlgo3DAngle::Report()
  //------------------------------
  {

  }
    
}
#endif