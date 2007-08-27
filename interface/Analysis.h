#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimDataFormats/Track/interface/SimTrack.h"
#include "RecoTracker/TkSeedingLayers/interface/SeedingLayerSets.h"

class SeedingHitSet;
class TrackerHitAssociator;
class OrderedSeedingHits;
class TrackingRegion;
class TH1D;
class TFile;
class TProfile;


#include <vector>

namespace edm { class Event; class EventSetup; } 


class Analysis {
public:

  Analysis(const edm::ParameterSet& conf);
  ~Analysis();

  const SimTrack * bestTrack(const edm::Event& ev) const;

  void init(const edm::Event& ev, const edm::EventSetup& es, TrackerHitAssociator * ass);

  void checkEfficiency(const OrderedSeedingHits& candidates);
  void checkAlgoEfficiency(const ctfseeding::SeedingLayerSets& layersSets, const OrderedSeedingHits& candidates);

  static void print(const SimTrack & track) ;
private:
  unsigned int matchedHits(unsigned int trackId, const SeedingHitSet& hits);
  bool select(const SimTrack & track) const;

private:
  edm::ParameterSet theConfig;

  const edm::Event * theEvent;
  const edm::EventSetup * theSetup;

  TrackerHitAssociator * theAssociator;
  std::vector<SimTrack> theSimTracks;

  TH1D *hEffPt_N, *hEffPt_D, *hEffAlgoPt_N, *hEffAlgoPt_D;
  TH1D *hEffEta_N, *hEffEta_D, *hEffAlgoEta_N, *hEffAlgoEta_D;
  TH1D *hEffPhi_N, *hEffPhi_D;
  TH1D *hPurePt_N, *hPurePt_D;

  TFile * rootFile;


};

