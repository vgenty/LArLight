/**
 * \file data_base.hh
 *
 * \ingroup DataFormat
 * 
 * \brief Base class def for data class
 *
 * @author Kazu - Nevis 2013
 */

/** \addtogroup DataFormat

    @{*/
#ifndef DATA_BASE_HH
#define DATA_BASE_HH

#include "Base-TypeDef.hh"
#include <vector>
#include <map>
#include <TObject.h>
#include <TString.h>
namespace larlight{
  /**
     \class data_base
     A base coass for all data objects in LArLight
  */

  class data_base : public TObject {
    
  public:
    
    /// Default constructor
    data_base(DATA::DATA_TYPE type=DATA::DATA_TYPE_MAX) : TObject(){ clear_data(); _type=type; }
    
    /// Default copy constructor to avoid memory leak in ROOT streamer
    data_base(const data_base &original) : TObject(original),
					   _type(original._type),
					   _ass(original._ass)
    {}

    
    /// Default destructor
    virtual ~data_base(){}
    
    /// Clear method
    virtual void clear_data() {_ass.clear();}
    
    /// data type getter
    DATA::DATA_TYPE data_type() const {return _type; }

    /// Adder for a set of association
    void add_association(DATA::DATA_TYPE type, const std::vector<unsigned int> ass);

    /// Getter for # of associations
    size_t size_association(DATA::DATA_TYPE type) const;
     
    /// Getter of an association
    const std::vector<unsigned int> association(DATA::DATA_TYPE type, size_t index=0) const;

  protected:
    
    /// DATA_TYPE
    DATA::DATA_TYPE _type;

    /// Association storage ... allow multiple set of associations
    std::map<larlight::DATA::DATA_TYPE,std::vector<std::vector<unsigned int> > > _ass;
    
    ////////////////////////
    ClassDef(data_base,5)
    ////////////////////////
      
  };

  /**
     \class event_base
     A base coass for event-wise data holder class
  */
  class event_base : public data_base {
    
  public:
    
    /// Default constructor
    event_base(DATA::DATA_TYPE type=DATA::DATA_TYPE_MAX) : data_base(type) { clear_data(); }
    
    /// Default copy constructor to avoid memory leak in ROOT streamer
    event_base(const event_base &original) : data_base(original),
					     fRunNumber(original.fRunNumber),
					     fSubRunNumber(original.fSubRunNumber),
					     fEventID(original.fEventID)
    {}

    
    /// Default destructor
    virtual ~event_base(){}
    
    /// Clear method
    virtual void clear_data();
    
    /// run number setter
    void set_run      (UInt_t run) { fRunNumber    = run; }
    /// sub-run number setter
    void set_subrun   (UInt_t run) { fSubRunNumber = run; }
    /// event-id setter
    void set_event_id (UInt_t id ) { fEventID      = id;  }
    
    /// run number getter
    UInt_t run      () const { return fRunNumber;    }
    /// sub-run number getter
    UInt_t subrun   () const { return fSubRunNumber; }
    /// event-id getter
    UInt_t event_id () const { return fEventID;      }
    /// data type getter
    DATA::DATA_TYPE data_type() const {return _type; }

  protected:
    
    /// Run number
    UInt_t fRunNumber;
    
    /// Sub-Run number
    UInt_t fSubRunNumber;
    
    /// Event ID
    UInt_t fEventID;

    ////////////////////////
    ClassDef(event_base,4)
    ////////////////////////
      
  };
}
#endif
/** @} */ // end of doxygen group 
