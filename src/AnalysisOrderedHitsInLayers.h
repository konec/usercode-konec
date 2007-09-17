#ifndef AnalysisOrderedHitsInLayers_H
#define AnalysisOrderedHitsInLayers_H

#include <vector>
#include "RecoTracker/TkSeedingLayers/interface/SeedingLayerSets.h"
#include "RecoTracker/TkSeedingLayers/interface/SeedingHitSet.h"
namespace edm { class Event; class EventSetup; }


class AnalysisOrderedHitsInLayers {
public:
  AnalysisOrderedHitsInLayers(
      const ctfseeding::SeedingLayers & layers, const edm::Event& ev, const edm::EventSetup& es); 

  ~AnalysisOrderedHitsInLayers(){}
  
  bool next();
  bool onlySingleHits() const;  
  SeedingHitSet getHitSet() const;
private:
  bool increaseCounter(unsigned int depth);
  bool theFirstEvent; 
  std::vector<unsigned int>    theIndexCounters;
  std::vector< SeedingHitSet > theHitsInLayers; 

};
#endif
