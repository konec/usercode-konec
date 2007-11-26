#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Vertex/interface/SimVertex.h"

#include "RecoTracker/TkSeedingLayers/interface/SeedingLayerSets.h"

class SeedingHitSet;
class TrackerHitAssociator;
class OrderedSeedingHits;
class TrackingRegion;
class TH1D;
class TFile;
class TProfile;

#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"


#include <vector>
#include <string>

namespace edm { class Event; class EventSetup; } 

class Analysis {
public:

  Analysis(const edm::ParameterSet& conf);
  ~Analysis();

  const SimTrack * bestTrack() const;
  const SimVertex * vertex(const SimTrack * track) const;

  void init(const edm::Event& ev, const edm::EventSetup& es, TrackerHitAssociator * ass = 0);

  void checkEfficiency(const reco::TrackCollection & tracks);
  void checkEfficiency(const OrderedSeedingHits& candidates);
  void checkAlgoEfficiency1(const ctfseeding::SeedingLayerSets&, const OrderedSeedingHits& );
  void checkAlgoEfficiency2(const ctfseeding::SeedingLayerSets&, const OrderedSeedingHits& );

  static void print(const SimTrack & track) ;
  static std::string print(const ctfseeding::SeedingHit & hit);
private:
  unsigned int matchedHits(unsigned int trackId, const SeedingHitSet& hits);
  bool select(const SimTrack & track) const;
  bool compareHitSets(const SeedingHitSet& hits1, const SeedingHitSet& hits2) const;

private:
  edm::ParameterSet theConfig;

  const edm::Event * theEvent;
  const edm::EventSetup * theSetup;

  TrackerHitAssociator * theAssociator;
  std::vector<SimTrack> theSimTracks;
  std::vector<SimVertex> theSimVertices;

  TH1D *hEffPt_N, *hEffPt_D, *hEffAlgoPt_N, *hEffAlgoPt_D;
  TH1D *hEffEta_N, *hEffEta_D, *hEffAlgoEta_N, *hEffAlgoEta_D;
  TH1D *hEffPhi_N, *hEffPhi_D;
  TH1D *hPurePt_N, *hPurePt_D;

  TFile * rootFile;


};

