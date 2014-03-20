#include <iostream>
#ifndef UTIL_PXUTILS_H
#define UTIL_PXUTILS_H


namespace larutil{
    class PxPoint {
    public:
      double w; // wire distance in cm
      double t; // time distance in cm (drift distance)
      unsigned int plane; // plane 0, 1, 2
      
      PxPoint(){
	Clear();
	//	std::cout<< "This is the default point ctor." << std::endl;
      }
      
      PxPoint(unsigned int pp,double ww,double tt){
	plane=pp;
	w=ww;
	t=tt;
      }
      
      void Clear()
      {
	plane = 0;
	w     = 0;
	t     = 0;
      }
      
      ~PxPoint(){}
    };
  
  class PxHit : public PxPoint {
    
  public:
    
    double charge; //charge
    
    PxHit(){
      Clear();
      std::cout << "This is the $^_*&ing default ctor." << std::endl;
    }
    ~PxHit(){}

    void Clear()
    {
      PxPoint::Clear();
      charge = 0;
    }

  };
  
  
  //helper class needed for the seeding
  class PxLine {
  public:
    
    PxPoint pt0() { return PxPoint(plane,w0,t0); }
    PxPoint pt1() { return PxPoint(plane,w1,t1); }
    
    double w0; ///<defined to be the vertex w-position
    double t0; ///<defined to be the vertex t-position
    double w1; ///<defined to be the ending w-position (of line or seed depending)
    double t1; ///<defined to be the ending t-position (of line or seed depending)
    unsigned int plane;
    
    PxLine(unsigned int pp,double ww0,double tt0, double ww1, double tt1){
      Clear();
      plane=pp;
      w0=ww0;
      t0=tt0;
      w1=ww1;
      t1=tt1;
    }   

    PxLine(){Clear();}

    ~PxLine(){}

    void Clear()
    {
      plane=0;
      w0=0;
      t0=0;
      w1=0;
      t1=0;
    }
    
  };
  
}



#endif
