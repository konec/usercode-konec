#include "CSCFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"


#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTExtendedCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTReadoutCollection.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <set>

using namespace edm;
using namespace std;

CSCFilter::CSCFilter(const edm::ParameterSet& cfg)
{ }

bool CSCFilter::filter(edm::Event&ev, const edm::EventSetup&es)
{
  edm::Handle<L1MuGMTReadoutCollection> pCollection;
  InputTag rpctfSource_("l1GtUnpack");
  ev.getByLabel(rpctfSource_,pCollection);

  L1MuGMTReadoutCollection const* gmtrc = pCollection.product();
  if (!gmtrc) return false;
  vector<L1MuGMTReadoutRecord> gmt_records = gmtrc->getRecords();
  vector<L1MuGMTReadoutRecord>::const_iterator RRItr;
  for( RRItr = gmt_records.begin() ; RRItr != gmt_records.end() ; RRItr++ ) {
    vector<L1MuRegionalCand> CSCCands = RRItr->getCSCCands();
    for( vector<L1MuRegionalCand>::const_iterator ECItr = CSCCands.begin() ; ECItr != CSCCands.end() ; ++ECItr ) {
      if (ECItr->empty()) continue;
      if (ECItr->bx()==0) return true;
    }
  }
  return false;
}
