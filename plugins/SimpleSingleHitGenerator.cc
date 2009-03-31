#include "SimpleSingleHitGenerator.h"

#include "RecoTracker/TkSeedingLayers/interface/SeedingLayerSetsBuilder.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "RecoTracker/TkTrackingRegions/interface/TrackingRegion.h"

using namespace std;
using namespace ctfseeding;

typedef TransientTrackingRecHit::ConstRecHitPointer Hit;

const OrderedSeedingHits & SimpleSingleHitGenerator::run(const TrackingRegion& region, 
    const edm::Event & ev, const edm::EventSetup& es)
{
  if (!initialised) init(theConfig,es);
  theOrederedHits = SimpleOrderedHits();
  for (ctfseeding::SeedingLayerSets::const_iterator ils = theLayers.begin();
      ils != theLayers.end(); ils++) {
    const SeedingLayer & layer = (*ils).front();
    typedef std::vector<Hit> SH; 
    SH htmp = region.hits(ev,es,&layer);
    for (SH::const_iterator ith = htmp.begin(); ith != htmp.end(); ith++) {
        const TrackingRecHit * trh = (*ith)->hit();
        std::cout <<"RecHit at: " << trh<<" localPosiont: " << trh->localPosition()<<endl;
        theOrederedHits.add( *ith);
    }
  } 
  std::cout <<"HERE my GENERATOR!!!!, hits: " << theOrederedHits.size() << endl;
  return theOrederedHits;
}

void SimpleSingleHitGenerator::init(const edm::ParameterSet & cfg, const edm::EventSetup& es) 
{
  initialised = true;

  std::string layerBuilderName = cfg.getParameter<std::string>("SeedingLayers");
  edm::ESHandle<SeedingLayerSetsBuilder> layerBuilder;
  es.get<TrackerDigiGeometryRecord>().get(layerBuilderName, layerBuilder);

  theLayers =  layerBuilder->layers(es);
  std::cout << "INITIALISED, the layers: " << theLayers.size() << std::endl;

} 
