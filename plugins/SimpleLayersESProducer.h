#ifndef SimpleLayersESProducer_H
#define SimpleLayersESProducer_H

#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <boost/shared_ptr.hpp>

#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "RecoTracker/TkSeedingLayers/interface/SeedingLayerSetsBuilder.h"

class SimpleLayersESProducer: public edm::ESProducer {
public:

  SimpleLayersESProducer(const edm::ParameterSet & cfg) : theConfig(cfg) {
    setWhatProduced(this, theConfig.getParameter<std::string>("ComponentName") );
  }

  boost::shared_ptr<SeedingLayerSetsBuilder> produce(const TrackerDigiGeometryRecord & r) {
    theResult = boost::shared_ptr<SeedingLayerSetsBuilder>( new SeedingLayerSetsBuilder(theConfig));
    return theResult;
  }
  
private:
  edm::ParameterSet theConfig;
  boost::shared_ptr<SeedingLayerSetsBuilder> theResult;
};
#endif

