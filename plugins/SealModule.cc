#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "RecoTracker/TkTrackingRegions/interface/TrackingRegionProducerFactory.h"
#include "RecoTracker/TkTrackingRegions/interface/TrackingRegionProducer.h"
#include "MyRegions.h"

DEFINE_EDM_PLUGIN(TrackingRegionProducerFactory, MyRegions, "MyRegions");

#include "RecoTracker/TkTrackingRegions/interface/OrderedHitsGeneratorFactory.h"
#include "RecoTracker/TkTrackingRegions/interface/OrderedHitsGenerator.h"
// #include "SimpleSingleHitGenerator.h"
// DEFINE_EDM_PLUGIN(OrderedHitsGeneratorFactory, SimpleSingleHitGenerator, "SimpleSingleHitGenerator");

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/Utilities/interface/typelookup.h"
#include "SimpleLayersESProducer.h"
DEFINE_FWK_EVENTSETUP_MODULE(SimpleLayersESProducer);
