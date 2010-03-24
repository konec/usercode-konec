#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_SEAL_MODULE();

#include "RecoTracker/TkTrackingRegions/interface/TrackingRegionProducerFactory.h"
#include "RecoTracker/TkTrackingRegions/interface/TrackingRegionProducer.h"
#include "MyRegions.h"

DEFINE_EDM_PLUGIN(TrackingRegionProducerFactory, MyRegions, "MyRegions");

#include "RecoTracker/TkTrackingRegions/interface/OrderedHitsGeneratorFactory.h"
#include "RecoTracker/TkTrackingRegions/interface/OrderedHitsGenerator.h"
#include "SimpleSingleHitGenerator.h"

DEFINE_EDM_PLUGIN(OrderedHitsGeneratorFactory, SimpleSingleHitGenerator, "SimpleSingleHitGenerator");

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/Framework/interface/eventsetupdata_registration_macro.h"

#include "SimpleLayersESProducer.h"
DEFINE_FWK_EVENTSETUP_MODULE(SimpleLayersESProducer);

#include "FilterOrbit.h"
DEFINE_FWK_MODULE(FilterOrbit);

#include "FilterL1.h"
DEFINE_ANOTHER_FWK_MODULE(FilterL1);
