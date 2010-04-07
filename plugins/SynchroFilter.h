#ifndef SynchroFilter_H
#define SynchroFilter_H

#include "FWCore/Framework/interface/EDFilter.h"

namespace edm {class ParameterSet; class Event; class EventSetup; }
class TH1D;

class SynchroFilter :  public edm::EDFilter {
public:
  explicit SynchroFilter(const edm::ParameterSet&);
  ~SynchroFilter();
private:
  bool checkMatching(float rpcEta, float rpcPhi, edm::Event&);
  virtual bool filter(edm::Event&, const edm::EventSetup&);
  virtual void endJob() {} 
  TH1D *hDeltaPhi, *hDeltaEta;
  float mindeta, mindphi; 
};
#endif
