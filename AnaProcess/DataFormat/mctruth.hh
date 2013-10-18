/**
 * \file mctruth.hh
 *
 * \ingroup DataFormat
 * 
 * \brief Class def header for mctruth data container
 *
 * @author Kazu - Nevis 2013
 */

/** \addtogroup DataFormat
    
@{*/

#ifndef MCTRUTH_HH
#define MCTRUTH_HH

#include "data_base.hh"
#include <map>
#include <TVector3.h>
/**
   \class part_mc
   Simplified version of MCParticle data container
*/
class part_mc : public data_base {

public:
  
  /// Default constructor requires pdgid, track & parent id
  part_mc(int pdgid, int track, int parent);

  /// Copy constructor
  part_mc(const part_mc& original) : data_base     (original),
				     fStepTime     (original.fStepTime),
				     fStepVertex   (original.fStepVertex),
				     fStepMomentum (original.fStepMomentum),
				     fStepProcess  (original.fStepProcess),
				     fDaughters    (original.fDaughters),
				     fParentID     (original.fParentID),
				     fTrackID      (original.fTrackID),
				     fPDGID        (original.fPDGID)
  {};
  
  /// Default destructor
  virtual ~part_mc(){};
  
  /// Clear method
  void clear_data();

  /// Appender of a particle track
  void add_track(double x,  double y,  double z,  double t,
		 double px, double py, double pz, 
		 std::string process);

  /// Appender of a particle's daughter
  void add_daughter(int id, int track_id=-1);

private:

  /// Default constructor w/o arguments set to private (not-to-be-used)
  part_mc(){clear_data();};

  std::vector<double>      fStepTime;     ///< time at each step
  std::vector<TVector3>    fStepVertex;   ///< vertex at each step
  std::vector<TVector3>    fStepMomentum; ///< momentum at each step
  std::vector<std::string> fStepProcess;  ///< process name at each step
  /// A list of daughters (daughter's particle id, which is unique, & step number)
  std::map<int, int>       fDaughters;
 
  int fParentID; ///< Parent's track id
  int fTrackID;  ///< Unique tracking ID per particle
  int fPDGID;    ///< Particle ID code

  ////////////////////////
  ClassDef(part_mc,3)
  ////////////////////////

};

/**
   \class event_mc
   LArSoft MCTruth class equivalent data container
*/
class event_mc : public data_base {

public:
  
  /// Default constructor
  event_mc() : data_base() {clear_data();};

  /// Copy constructor
  event_mc(const event_mc& original) : data_base   (original),
				       fPartList   (original.fPartList),
				       //fMCNeutrino (original.fMCNeutrino),
				       fOrigin     (original.fOrigin)
	  //fNeutrinoSet(original.fNeutrinoSet
  {};

  /// Default destructor
  virtual ~event_mc(){};

  /// Clear method
  void clear_data();

  /// Setter for neutrino info
  //void set_nu_mc(nu_mc info) {fMCNeutrino = info;};

  /// Appender for particle info
  void add_part(part_mc &info) {fPartList.push_back(info); };
  //void add_part(part_mc &info) {fPartList.push_back(info); fNeutrinoSet=true;};

  /// Setter for generation code
  void set_gen_code(MC::Origin_t info) {fOrigin=info;};

  /// Getter for neutrino info
  //inline const nu_mc& nu_mc() {return fMCNeutrino;};
  
  /// Getter for particle list
  inline const std::vector<part_mc>& part_array() const {return fPartList;};

  /// Getter for generator id
  inline MC::Origin_t gen_code() const {return fOrigin;};

  /// Getter for a boolean to suggest if neutrinos were generated or not
  //inline const bool nu_info_set() {return fNeutrinoSet;};

private:
  
  std::vector<part_mc> fPartList;    ///< list of particles in this event
  //nu_mc                fMCNeutrino;  ///< reference to neutrino info - null if not a neutrino
  MC::Origin_t         fOrigin;      ///< origin for this event
  //bool                 fNeutrinoSet; ///< flag for whether the neutrino information has been set
  
  ////////////////////////
  ClassDef(event_mc,2)
  ////////////////////////

};

#endif

/** @} */ // end of doxygen group 
