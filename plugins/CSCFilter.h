#ifndef CSCFilter_H
#define CSCFilter_H

#include "FWCore/Framework/interface/EDFilter.h"

namespace edm {class ParameterSet; class Event; class EventSetup; }

class CSCFilter :  public edm::EDFilter {
public:
  explicit CSCFilter(const edm::ParameterSet&);
  ~CSCFilter() {}
private:
  virtual void beginJob(const edm::EventSetup&)  {}
  virtual bool filter(edm::Event&, const edm::EventSetup&);
  virtual void endJob() {} 
};
#endif
