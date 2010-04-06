#ifndef SynchroFilter_H
#define SynchroFilter_H

#include "FWCore/Framework/interface/EDFilter.h"

namespace edm {class ParameterSet; class Event; class EventSetup; }

class SynchroFilter :  public edm::EDFilter {
public:
  explicit SynchroFilter(const edm::ParameterSet&);
  ~SynchroFilter() {}
private:
  virtual bool filter(edm::Event&, const edm::EventSetup&);
  virtual void endJob() {} 
};
#endif
