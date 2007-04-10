#include "PluginManager/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_SEAL_MODULE();

#include "RecoTracker/TkTrackingRegions/interface/TrackingRegionProducerFactory.h"
#include "MyRegions.h"
DEFINE_SEAL_PLUGIN(TrackingRegionProducerFactory, MyRegions, "MyRegions");
