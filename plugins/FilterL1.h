#ifndef FilterL1_H
#define FilterL1_H

#include "FWCore/Framework/interface/EDFilter.h"

namespace edm {class ParameterSet; class Event; class EventSetup; }

class FilterL1 :  public edm::EDFilter {
public:
  explicit FilterL1(const edm::ParameterSet&);
  ~FilterL1() {}
private:
  virtual void beginJob(const edm::EventSetup&)  {}
  virtual bool filter(edm::Event&, const edm::EventSetup&);
  virtual void endJob() {} 
};
#endif
