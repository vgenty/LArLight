/**
 * \file constants.hh
 *
 * \ingroup Base
 * 
 * \brief defines basic constants used in this framework
 *
 * @author Kazu - Nevis 2013
 */

/** \addtogroup Base

    @{*/

#ifndef CONSTANTS_HH
#define CONSTANTS_HH
#include <string>
#include "types.hh"

/// Defines constants for Message utility
namespace MSG{

  /// Defines message level
  enum Level{
    DEBUG=0,      ///< Message level ... useful to debug a crash
    INFO,         ///< Debug info but not the lowest level
    NORMAL,       ///< Normal stdout
    WARNING,      ///< notify a user in the standard operation mode for an important finding.
    ERROR,        ///< notify a user when something is clearly wrong
    MSG_TYPE_MAX
  };

  const std::string ColorPrefix[MSG_TYPE_MAX] =
    {
      "\033[94m", ///< blue ... DEBUG   
      "\033[92m", ///< green ... INFO
      "\033[95m", ///< magenta ... NORMAL
      "\033[93m", ///< yellow ... WARNING
      "\033[91m"  ///< red ... ERROR
    };
  ///< Color coding of message

  const std::string StringPrefix[MSG_TYPE_MAX] =
    {
      "     [DEBUG]  ", ///< DEBUG message prefix
      "      [INFO]  ", ///< INFO message prefix
      "    [NORMAL]  ", ///< NORMAL message prefix
      "   [WARNING]  ", ///< WARNING message prefix 
      "     [ERROR]  "  ///< ERROR message prefix
    };
  ///< Prefix of message
}

/// Defines constants for data structure definition (system utility)
namespace DATA{

  /// Define identifier for a data container class to be loaded through storage_manager::get_data()
  enum DATA_TYPE{
    Event=0,
    UserInfo,           ///< dynamic data container
    MCTruth,            ///< MCTruth
    FIFOChannel,        ///< FIFOChannel
    Wire,               ///< Wire
    Hit,                ///< Hit
    Track,              ///< Track
    Bezier,             ///< Track (Bezier)
    Kalman3DSPS,        ///< Track (Kalman3DSPS)
    Shower,             ///< Shower
    Cluster,            ///< Cluster
    Calorimetry,        ///< Calorimetry
    DATA_TYPE_MAX
  };

  /// Define tree name
  const std::string DATA_TREE_NAME[DATA_TYPE_MAX] = {
    "event",
    "user",
    "mc",
    "pmt",
    "wire",
    "hit",
    "track",
    "bezier",
    "kalman3dsps",
    "shower",
    "cluster",
    "calorimetry"
  };

  /// Invalid (or initialized) channel number  
  const DATA::ch_number_t INVALID_CH=0xffff;

}

/// Defines constants for PMT related electronics
namespace PMT{

  /// PMT discriminator type
  enum DISCRIMINATOR {
    BASE_DISC = 0, ///< lowest disc. threshold so-called disc. 0
    COSMIC_DISC,   ///< discriminator for cosmics
    MICHEL_DISC,   ///< discriminator for michel electrons (not used, just a place hlder)
    BEAM_DISC,     ///< discriminator for signal within the beam window
    BEAM_WIN,      ///< discriminator for beam window (forced N samples read-out)
    DISC_MAX
  };
}

/// Defines constants for MC truth information
namespace MC{

  /// MC generator type
  typedef enum _ev_origin{
    kUnknown,           ///< ???
    kBeamNeutrino,      ///< Beam neutrinos
    kCosmicRay,         ///< Cosmic rays
    kSuperNovaNeutrino, ///< Supernova neutrinos
    kSingleParticle     ///< single particles thrown at the detector
  } Origin_t;
}

#endif
/** @} */ // end of doxygen group
