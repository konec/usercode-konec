#ifndef SimpleSingleHitGenerator_H
#define SimpleSingleHitGenerator_H

#include "RecoTracker/TkTrackingRegions/interface/OrderedHitsGenerator.h"
#include "RecoTracker/TkSeedingLayers/interface/OrderedSeedingHits.h"
#include "RecoTracker/TkSeedingLayers/interface/SeedingLayerSets.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <vector>


class TrackingRegion;
namespace edm { class Event; class EventSetup; }



class SimpleSingleHitGenerator : public OrderedHitsGenerator {
private:
  class SimpleOrderedHits : public OrderedSeedingHits {
  public:
    typedef TransientTrackingRecHit::ConstRecHitPointer Hit;
    virtual ~SimpleOrderedHits(){}
    virtual unsigned int size() const  { return theHitSets.size(); }
    virtual const SeedingHitSet & operator[](unsigned int i) const { return theHitSets[i]; }
    void add( const Hit & hit) {
      std::vector<Hit> hitSet(1,hit);
      theHitSets.push_back(hitSet);
    }
  private:
    std::vector<SeedingHitSet> theHitSets;   
  };
public:
  SimpleSingleHitGenerator(const edm::ParameterSet & cfg) : initialised(false), theConfig(cfg) {}
  virtual ~SimpleSingleHitGenerator() {} 
  virtual const OrderedSeedingHits & run(
    const TrackingRegion& region, const edm::Event & ev, const edm::EventSetup& es);
  private:
    void init(const ctfseeding::SeedingLayerSets & layerSets);
    void init(const edm::ParameterSet & cfg, const edm::EventSetup& es);
  private:
    bool initialised;
    edm::ParameterSet theConfig;
    ctfseeding::SeedingLayerSets theLayers;
    SimpleOrderedHits theOrederedHits;

};

#endif 
