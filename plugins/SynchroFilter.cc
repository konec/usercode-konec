#include "SynchroFilter.h"

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

#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/print.h"




#include <iostream>
#include <fstream>
#include <vector>
#include <set>

using namespace edm;
using namespace std;

SynchroFilter::SynchroFilter(const edm::ParameterSet& cfg)
{ }

bool SynchroFilter::filter(edm::Event&ev, const edm::EventSetup&es)
{

  bool goodEvent = false;
// if (   ev.id().event() == 3856444 ) goodEvent = true;
      
  edm::Handle<L1MuGMTReadoutCollection> pCollection;
  InputTag rpctfSource_("l1GtUnpack");
  ev.getByLabel(rpctfSource_,pCollection);

  L1MuGMTReadoutCollection const* gmtrc = pCollection.product();
  if (!gmtrc) return goodEvent;

  vector<L1MuGMTReadoutRecord> gmt_records = gmtrc->getRecords();
  vector<L1MuGMTReadoutRecord>::const_iterator RRItr;
  bool brlRPC = false;
  bool fwdRPC = false;
  bool CSC = false;
  bool DT = false;
  vector<L1MuRegionalCand> Cands;
  typedef vector<L1MuRegionalCand>::const_iterator ITC;

  std::ostringstream str;

  float rpcEta = -10.;
  float rpcPhi = -10.;
  float muonEta = -01.;
  float muonPhi=-10.;

  for( RRItr = gmt_records.begin() ; RRItr != gmt_records.end() ; RRItr++ ) {
    Cands = RRItr->getCSCCands();
    for(ITC it = Cands.begin() ; it != Cands.end() ; ++it ) {
      if (it->empty()) continue;
      CSC = true;
      muonEta = it->etaValue();
      muonPhi = it->phiValue();
      str <<"HAS  CSC cand "
          <<" pt: "<<it->ptValue()
          <<" eta: "<<it->etaValue()
          <<" phi: "<<it->phiValue()
          <<" Q:" <<it->quality()
          <<" bx: " <<it->bx()
          <<std::endl;
    }
    Cands = RRItr->getDTBXCands();
    for(ITC it = Cands.begin() ; it != Cands.end() ; ++it ) {
      if (it->empty()) continue;
      muonEta = it->etaValue();
      muonPhi = it->phiValue();
      DT= true;
      str <<"HAS   DT cand "
          <<" pt: "<<it->ptValue()
          <<" eta: "<<it->etaValue()
          <<" phi: "<<it->phiValue()
          <<" Q:" <<it->quality()
          <<" bx: " <<it->bx()
          <<std::endl;
    }
    Cands = RRItr->getBrlRPCCands();
    for(ITC it = Cands.begin() ; it != Cands.end() ; ++it ) {
      if (it->empty()) continue;
      brlRPC = true;
      rpcEta = it->etaValue();
      rpcPhi = it->phiValue();
      str <<"HAS RPCB cand "
          <<" pt: "<<it->ptValue()
          <<" eta: "<<it->etaValue()
          <<" phi: "<<it->phiValue()
          <<" Q:" <<it->quality()
          <<" bx: " <<it->bx()
          <<std::endl;
    }
    Cands = RRItr->getFwdRPCCands();
    for(ITC it = Cands.begin() ; it != Cands.end() ; ++it ) {
      if (it->empty()) continue;
      fwdRPC = true;
      rpcEta = it->etaValue();
      rpcPhi = it->phiValue();
      str <<"HAS RPCF cand "
          <<" pt: "<<it->ptValue()
          <<" eta: "<<it->etaValue()
          <<" phi: "<<it->phiValue()
          <<" Q:" <<it->quality()
          <<" bx: " <<it->bx()
          <<std::endl;
    }
  }
//  if ( brlRPC || fwdRPC || CSC || DT)  std::cout << str.str() << std::endl;
//  if (brlRPC || fwdRPC || CSC || DT) goodEvent = true;
//  if (brlRPC || fwdRPC) goodEvent = true;

  bool matched = false;

  edm::Handle<reco::TrackCollection> trackCollection;
 // InputTag trackCollectionTag("generalTracks");
  InputTag trackCollectionTag("globalMuons");
  ev.getByLabel(trackCollectionTag,trackCollection);
  reco::TrackCollection tracks = *(trackCollection.product());
//  cout <<"#RECONSTRUCTED tracks: " << tracks.size() << endl;
  typedef reco::TrackCollection::const_iterator IT;
  for (IT it = tracks.begin(); it !=tracks.end(); ++it) {
    const reco::Track & track = *it;
    float phi = track.momentum().phi();
    float dphi = phi-rpcPhi;
    //float dphi = phi-muonPhi;
    if (dphi < -M_PI) dphi+=2*M_PI;
    if (dphi > M_PI) dphi-=2*M_PI;
    float eta = track.momentum().eta();
    float deta = eta-rpcEta;
    float pt_rec = track.pt();
    if ( fabs(dphi) < 1.0 && fabs(deta) < 0.25 && pt_rec > 2.) {
     matched = true;
 //    std::cout <<"MATCHED: pt:" <<pt_rec<<" phi: "<<phi<<" eta: "<<eta<<endl;
   }
   
//    Analysis::print(track);
  }

  if (matched) goodEvent=true;

  return goodEvent;
//  return true;
}

