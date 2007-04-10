#ifndef UserCode_konec_MyRegions_H 
#define UserCode_konec_MyRegions_H 

#include "RecoTracker/TkTrackingRegions/interface/TrackingRegionProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <vector>

class MyRegions : public TrackingRegionProducer {

public:

  MyRegions(const edm::ParameterSet& cfg);

  virtual ~MyRegions(){}

  virtual std::vector<TrackingRegion* > regions(const edm::Event&, const edm::EventSetup&) const;

private:
  edm::ParameterSet theRegionPSet;
  std::vector<double> theSizes; 
};

#endif

