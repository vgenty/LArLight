/**
 * \file DataFormatConstants.hh
 *
 * \ingroup DataFormat
 * 
 * \brief defines constants used in DataFormat
 *
 * @author Kazu - Nevis 2013
 */

/** \addtogroup Base

    @{*/

#ifndef DATAFORMATCONSTANTS_HH
#define DATAFORMATCONSTANTS_HH

#include <string>
#include <limits>
#include <climits>
#include <TROOT.h>

/// Namespace of everything in this framework
namespace larlight{
  
  /// Defines constants for data structure definition (system utility)
  namespace DATA{

    const UChar_t  INVALID_UCHAR  = std::numeric_limits<UChar_t>::max();
    const Char_t   INVALID_CHAR   = std::numeric_limits<Char_t>::max();
    const UShort_t INVALID_USHORT = std::numeric_limits<UShort_t>::max();
    const Short_t  INVALID_SHORT  = std::numeric_limits<Short_t>::max();
    const UInt_t   INVALID_UINT   = std::numeric_limits<UInt_t>::max();
    const Int_t    INVALID_INT    = std::numeric_limits<Int_t>::max();

    const Double_t INVALID_DOUBLE = std::numeric_limits<Double_t>::max();
    const Double_t INVALID_FLOAT  = std::numeric_limits<Float_t>::max();
    
    /// Define identifier for a data container class to be loaded through storage_manager::get_data()
    enum DATA_TYPE{
      Event=0,
      MCTruth,            ///< MCTruth 
      MCParticle,         ///< MCParticle
      MCTrajectory,       ///< MCTrajectory
      MCNeutrino,         ///< MCNeutrino
      SimChannel,         ///< SimChannel
      MCShower,           ///< LArSoft utility class MCShower
      Wire,               ///< Wire
      Hit,                ///< Hit
      MCShowerHit,        ///< Hit created from MCShower
      CrawlerHit,         ///< ClusterCrawler Hit algo
      GausHit,            ///< Gaus Hit algo
      APAHit,             ///< APA Hit algo
      FFTHit,             ///< FFT Hit algo
      RFFHit,             ///< RFF Hit algo
      Cluster,            ///< Cluster
      MCShowerCluster,    ///< Cluster created from MCShower
      FuzzyCluster,       ///< Fuzzy Cluster
      DBCluster,          ///< DBCluster
      CrawlerCluster,     ///< Crawler
      HoughCluster,       ///< Hough
      ShowerAngleCluster, ///< ShowerAngleCluster
      RyanCluster,        ///< Ryan's clustering algorithm for pi0
      Seed,               ///< Seed
      SpacePoint,         ///< Spacepoints
      Track,              ///< Track
      Bezier,             ///< Track (Bezier)
      Kalman3DSPS,        ///< Track (Kalman3DSPS)
      Kalman3DHit,        ///< Track (Kalman3DHit)
      Shower,             ///< Shower
      RyanShower,         ///< Ryan's shower
      Vertex,             ///< Vertex
      FeatureVertex,      ///< FeatureVertex
      HarrisVertex,       ///< HarrisVertex
      EndPoint2D,         ///< EndPoint2D
      FeatureEndPoint2D,  ///< EndPoint2D from Feature Vertex Finder
      HarrisEndPoint2D,   ///< EndPoint2D from Harris Vertex Finder 
      Calorimetry,        ///< Calorimetry
      UserInfo,           ///< dynamic data container 
      /*
	Following enums are LArLight original except FIFOChannel which
	is meant to be compatible with LArSoft FIFOChannel
      */
      FIFO,               ///< LArSoft waveform 
      PMTFIFO,            ///< PMT channel waveform data
      TPCFIFO,            ///< TPC channel waveform data
      Pulse,              ///< Generic event_pulse
      PMTPulse_ThresWin,  ///< Reconstructed event_pulse for PMT waveform
      PMTPulse_FixedWin,  ///< Reconstructed event_pulse for PMT waveform
      TPCPulse_ThresWin,  ///< reconstructed event_pulse for TPC waveform
      TPCPulse_FixedWin,  ///< reconstructed event_pulse for TPC waveform
      Trigger,            ///< Trigger data

      CosmicTag,          ///< anab::CosmicTag 
      OpHit,              ///< opdet::OpHit
      OpFlash,            ///< opdet::OpFlash

      DATA_TYPE_MAX
    };
    
    /// Define tree name
    const std::string DATA_TREE_NAME[DATA_TYPE_MAX] = {
      "event",
      "mctruth",
      "mcpart",
      "mctrajectory",
      "mcnu",
      "simch",
      "mcshower",
      "wire",
      "hit",
      "mcshit",
      "crawlerhit",
      "gaushit",
      "apahit",
      "ffthit",
      "rffhit",
      "cluster",
      "mcscluster",
      "fuzzycluster",
      "dbcluster",
      "crawlercluster",
      "houghcluster",
      "showeranglecluster",
      "ryancluster",
      "seed",
      "sps",
      "track",
      "bezier",
      "kalman3dsps",
      "kalman3dhit",
      "shower",
      "ryanshower",
      "vertex",
      "feature_vtx",
      "harris_vtx",
      "end2d",
      "feature_end2d",
      "harris_end2d",
      "calorimetry",
      "user",
      "fifo",
      "pmtfifo",
      "tpcfifo",
      "pulse",
      "pmt_threswin",
      "pmt_fixedwin",
      "tpc_threswin",
      "tpc_fixedwin",
      "trigger",
      "cosmictag",
      "ophit",
      "opflash"
    };
    
  }
  
}
#endif
/** @} */ // end of doxygen group
