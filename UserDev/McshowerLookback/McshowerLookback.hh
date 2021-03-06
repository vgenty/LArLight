/**
 * \file McshowerLookback.hh
 *
 * \ingroup McshowerLookback
 * 
 * \brief Class def header for a class McshowerLookback
 *
 * @author littlejohn
 */

/** \addtogroup McshowerLookback

    @{*/
#ifndef MCSHOWERLOOKBACK_HH
#define MCSHOWERLOOKBACK_HH

#include <iostream>
#include <vector>
#include "ana_base.hh"

/**
   \class McshowerLookback
   User defined class McshowerLookback ... these comments are used to generate
   doxygen documentation!
 */

namespace larlight {
 
  class McshowerLookback{
    
  public:
    
    /// Default constructor (min energy is 0.02GeV)
    McshowerLookback(){ _min_energy = 0.02; _max_energy = 1.e3;};
    
    /// Takes in event_mcshowers and fills a map of 
    /// (G4trackid of daughter particle => MCShower index in ev_mcshower)
    void FillShowerMap(const larlight::event_mcshower* my_mcshow, 
		       std::map<UInt_t,UInt_t> &shower_idmap);

    /// Utilitiy function: convert std::map(track ID, shower ID) to std::vector(unique shower ID)
    void UniqueShowerID(const std::map<UInt_t,UInt_t> &shower_idmap, std::vector<UInt_t> &shower_id) const;

    /// Takes in event_simchannels and fills a map of
    /// (channel => larlight::simch)
    void FillSimchMap(const larlight::event_simch* my_simch, 
		      std::map<UShort_t, larlight::simch> &simch_map);
    
    /// Takes in a single hit, two maps, and a list of MCShower indices and returns 
    /// a vector of charge fractions from the different MCShowers the hit belongs to.
    /// For example, if the list of MCShowers indices was (0, 1, 3)
    /// and the hit's charge is 20% in MCShower 0, 70% in MCShower1, 
    /// 0% in MCShower3 the returned vector is (0.2, 0.7, 0.0, 0.1)
    /// This last element is charge from either
    ///    * non-showers
    ///    * showers not included in the inpur list of MCShower indices
    ///    * showers below the cutoff threshold (20MeV default)
    std::vector<float> MatchHitsAll(const larlight::hit &this_hit, 
				    const std::map<UShort_t, larlight::simch> &simch_map, 
				    const std::map<UInt_t,UInt_t> &shower_idmap, 
				    const std::vector<UInt_t> &MCShower_indices);

    std::vector<float> MatchHitsAll(const std::vector<const larlight::hit*> &hits,
				    const std::map<UShort_t, larlight::simch> &simch_map, 
				    const std::map<UInt_t,UInt_t> &shower_idmap,
				    bool fraction=true) const;
    
    void SetMaxEnergyCut(double energy) { _max_energy = energy; }
    void SetMinEnergyCut(double energy) { _min_energy = energy; }
    
    //std::vector<double> MatchHitsAll;
    //double MatchClusOne;
    //std::vector<double> MatchClusAll;
    
    
    /// Default destructor
    virtual ~McshowerLookback(){};
    
  protected:
    
    double _max_energy;
    double _min_energy;
  };
  
}
#endif
/** @} */ // end of doxygen group 

