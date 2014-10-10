#ifndef SHOWERCONTAINMENTCUT_CC
#define SHOWERCONTAINMENTCUT_CC

#include "ShowerContainmentCut.hh"

namespace showerana {

  ShowerContainmentCut::ShowerContainmentCut()
    : _xyz_min(3,0), _xyz_max(3,0)
  {
    Reset();
  }

  ShowerContainmentCut::ShowerContainmentCut(double const x_min, double const x_max,
					     double const y_min, double const y_max,
					     double const z_min, double const z_max)
    : _xyz_min(3,0), _xyz_max(3,0)
  {
    Reset();
    SetXYZMax(x_max,y_max,z_max);
    SetXYZMin(x_min,y_min,z_min);
  }

  void ShowerContainmentCut::Reset() 
  {
    _xyz_min[0] = 0.;
    _xyz_min[1] = -116.;
    _xyz_min[2] = 0.;

    _xyz_max[0] = 256.;
    _xyz_max[1] = 116.;
    _xyz_max[2] = 1030.;
  }

  double ShowerContainmentCut::DistanceToWall(std::vector<double> const& point) const
  {

    if( point.size() < 3) {

      std::ostringstream msg;
      msg 
	<< "<<" << __FUNCTION__ << ">>"
	<< " expects length 3 vector for point ... given: "
	<< point.size()
	<< std::endl;
      throw ShowerAnaException(msg.str());
    }
    
    //
    // This function computes the distance from a point to the closest wall of the box.
    // The box is pre-defined by _xyz_min and _xyz_max values.
    // The function only works only if the point is inside the box for now because
    // otherwise it adds a complication to the problem.
    // 
    // The function proceeds in the following steps of computation:
    //
    // (1) Check if a point is within the box. If not, return -1.
    // (2) Compute the distance to the YZ wall (i.e. x=0 plane)
    // (3) Compute the distance to the ZX wall (i.e. y=0 plane)
    // (4) Compute the distance to the XY wall (i.e. z=0 plane)
    // (5) Return the minimum of (2), (3) and (4).

    //
    // (1) Check if a point is inside the box. If not, return -1.
    //
    if( (point[0] < _xyz_min[0] || _xyz_max[0] < point[0]) || // point is outside X boundaries OR
	(point[1] < _xyz_min[1] || _xyz_max[1] < point[1]) || // point is outside Y boundaries OR
	(point[2] < _xyz_min[2] || _xyz_max[2] < point[2]) )  // point is outside Z boundaries 

      return -1;

    //
    // (2) Compute the distance to the YZ wall
    //
    double dist_to_yz = point[0] - _xyz_min[0];
    if( dist_to_yz > (_xyz_max[0] - point[0]) ) dist_to_yz = _xyz_max[0] - point[0];

    //
    // (3) Compute the distance to the ZX wall
    //
    double dist_to_zx = point[1] - _xyz_min[1];
    if( dist_to_zx > (_xyz_max[1] - point[1]) ) dist_to_zx = _xyz_max[1] - point[1];

    //
    // (4) Compute the distance to the XY wall
    //
    double dist_to_xy = point[2] - _xyz_min[2];
    if( dist_to_xy > (_xyz_max[2] - point[2]) ) dist_to_xy = _xyz_max[2] - point[2];

    //
    // (6) Return the minimum of (3), (4), and (5)
    //
    double dist = ( dist_to_yz < dist_to_zx ? dist_to_yz : dist_to_zx );

    dist = ( dist < dist_to_xy ? dist : dist_to_xy );

    return dist;
  }


  double ShowerContainmentCut::DistanceToWall(std::vector<double> const& point,
					      std::vector<double> dir) const
  {
    
    if( point.size() < 3 || dir.size() <3) {

      std::ostringstream msg;
      msg 
	<< "<<" << __FUNCTION__ << ">>"
	<< " expects length 3 vector for point & dir ... given: "
	<< point.size() << " and " << dir.size()
	<< std::endl;
      throw ShowerAnaException(msg.str());
    }

    //
    // This function computes the distance from a point to the closest wall of the box
    // along the specified direction (both taken from input arguments). The box is
    // pre-defined by _xyz_min and _xyz_max values. 
    // The function only works only if the point is inside the box for now because
    // otherwise it adds a complication to the problem.
    // 
    // The function proceeds in the following steps of computation:
    //
    // (1) Check if a point is within the box. If not, return -1.
    // (2) Normalize dir to make it into a unit vector.
    // (3) Compute distance to reach YZ plane (i.e. x=0 plane)
    // (4) Compute distance to reach ZX plane (i.e. y=0 plane)
    // (5) Compute distance to reach XY plane (i.e. z=0 plane)
    // (6) Return the minimum of (3), (4), and (5)
    //

    //
    // (1) Check if a point is inside the box. If not, return -1.
    //
    if( (point[0] < _xyz_min[0] || _xyz_max[0] < point[0]) || // point is outside X boundaries OR
	(point[1] < _xyz_min[1] || _xyz_max[1] < point[1]) || // point is outside Y boundaries OR
	(point[2] < _xyz_min[2] || _xyz_max[2] < point[2]) )  // point is outside Z boundaries 

      return -1;
    
    //
    // (2) Normalize dir vector
    //
    double dir_magnitude = sqrt( pow(dir[0],2) + pow(dir[1],2) + pow(dir[2],2) );
    dir[0] /= dir_magnitude;
    dir[1] /= dir_magnitude;
    dir[2] /= dir_magnitude;

    //
    // (3) Compute distance to reach YZ plane
    //
    double dist_to_yz = 0;
    if(dir[0] < 0)

      dist_to_yz = (point[0] - _xyz_min[0]) / (-1. * dir[0]);

    else 

      dist_to_yz = (_xyz_max[0] - point[0]) / dir[0];

    //
    // (4) Compute distance to reach ZX plane
    //
    double dist_to_zx = 0;
    if(dir[1] < 0)

      dist_to_zx = (point[1] - _xyz_min[1]) / (-1. * dir[1]);

    else 

      dist_to_zx = (_xyz_max[1] - point[1]) / dir[1];

    //
    // (5) Compute distance to reach XY plane
    //
    double dist_to_xy = 0;
    if(dir[2] < 0)

      dist_to_xy = (point[2] - _xyz_min[2]) / (-1. * dir[2]);

    else 

      dist_to_xy = (_xyz_max[2] - point[2]) / dir[2];

    //
    // (6) Return the minimum of (3), (4), and (5)
    //
    double dist = ( dist_to_yz < dist_to_zx ? dist_to_yz : dist_to_zx );

    dist = ( dist < dist_to_xy ? dist : dist_to_xy );

    return dist;
  }


  double ShowerContainmentCut::DistanceBackwardsToWall(std::vector<double> const& point,
					      std::vector<double> dir) const
  {
    
    if( point.size() < 3 || dir.size() <3) {

      std::ostringstream msg;
      msg 
	<< "<<" << __FUNCTION__ << ">>"
	<< " expects length 3 vector for point & dir ... given: "
	<< point.size() << " and " << dir.size()
	<< std::endl;
      throw ShowerAnaException(msg.str());
    }

    //
    // This function computes the distance from a point to the closest wall of the box
    // *BACKWARDS* along the specified direction (both taken from input arguments). The box is
    // pre-defined by _xyz_min and _xyz_max values. 
    // The function only works if the point is inside the box for now because
    // otherwise it adds a complication to the problem.
    // 
    // The function proceeds in the following steps of computation:
    //
    // (1) Check if a point is within the box. If not, return -1.
    // (2) Normalize dir to make it into a unit vector.
    // (3) Compute distance to reach YZ plane (i.e. x=0 plane)
    // (4) Compute distance to reach ZX plane (i.e. y=0 plane)
    // (5) Compute distance to reach XY plane (i.e. z=0 plane)
    // (6) Return the backwards distance from vertex

    //
    // (1) Check if a point is inside the box. If not, return -1.
    //
    if( (point[0] < _xyz_min[0] || _xyz_max[0] < point[0]) || // point is outside X boundaries OR
	(point[1] < _xyz_min[1] || _xyz_max[1] < point[1]) || // point is outside Y boundaries OR
	(point[2] < _xyz_min[2] || _xyz_max[2] < point[2]) )  // point is outside Z boundaries 

      return -1;
    
    //
    // (2) Normalize dir vector
    //
    double dir_magnitude = sqrt( pow(dir[0],2) + pow(dir[1],2) + pow(dir[2],2) );
    dir[0] /= dir_magnitude;
    dir[1] /= dir_magnitude;
    dir[2] /= dir_magnitude;

    //
    // (3) Compute distance to reach YZ plane
    //
    double dist_to_yz = 0;
    if(dir[0] < 0)

      dist_to_yz = (_xyz_max[0] - point[0]) / (-1 * dir[0] ) ;

    else 

      dist_to_yz = (point[0] - _xyz_min[0]) /  dir[0];


    //
    // (4) Compute distance to reach ZX plane
    //
    double dist_to_zx = 0;
    if(dir[1] < 0)

      dist_to_zx = (_xyz_max[1] - point[1]) / ( -1 * dir[1] );

    else 

      dist_to_zx = (point[1] - _xyz_min[1]) /  dir[1] ;


    //
    // (5) Compute distance to reach XY plane
    //
    double dist_to_xy = 0;
    if(dir[2] < 0)

      dist_to_xy = (_xyz_max[2] - point[2]) / ( -1 * dir[2] );

    else 

      dist_to_xy = (point[2] - _xyz_min[2]) / dir[2];



    //
    // (6) Return the minimum of (3), (4), and (5)
    //
    double dist = ( dist_to_yz < dist_to_zx ? dist_to_yz : dist_to_zx );

    dist = ( dist < dist_to_xy ? dist : dist_to_xy );

    return dist;
  }

}

#endif
