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


#include "TObjArray.h"
#include "TFile.h"
#include "TH1D.h"

TObjArray histos;



#include <iostream>
#include <fstream>
#include <vector>
#include <set>

using namespace edm;
using namespace std;

SynchroFilter::SynchroFilter(const edm::ParameterSet& cfg)
{ 
    hDeltaPhi = new TH1D("hDeltaPhi","hDeltaPhi",100, 0., 3.);
    hDeltaEta = new TH1D("hDeltaEta","hDeltaEta",100, 0, 1.);
    histos.SetOwner();
    histos.Add(hDeltaPhi);
    histos.Add(hDeltaEta);
}

SynchroFilter::~SynchroFilter()
{ 
  cout <<"WRITING ROOT FILE"<< std::endl;
  TFile rootFile("analysis.root","RECREATE");
  histos.Write();
  rootFile.Close();
  cout <<"rootFile WRITTEN, DTOR"<<endl;
}

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
  mindeta = 100.;
  mindphi = 100.;
  bool matched = false;

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
      if (checkMatching(rpcEta,rpcPhi,ev)) matched = true;
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
      if (checkMatching(rpcEta,rpcPhi,ev)) matched = true;
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
//  if (brlRPC || fwdRPC)   std::cout << str.str() << std::endl;

  if (mindphi < 99.) hDeltaPhi->Fill(mindphi);
  if (mindeta < 99.) hDeltaEta->Fill(mindeta);
  if (matched) goodEvent=true;
  return goodEvent;
}

bool SynchroFilter::checkMatching(float rpcEta, float rpcPhi, edm::Event&ev)
{
  bool matched = false;

//  rpcEta *= -1;
//  rpcPhi += M_PI;
//  if (rpcPhi > 2*M_PI) rpcPhi -= 2*M_PI;

  edm::Handle<reco::TrackCollection> trackCollection;
  edm::Handle<reco::TrackCollection> muonCollection;
  ev.getByLabel(InputTag("generalTracks"),trackCollection);
  ev.getByLabel(InputTag("globalMuons"),muonCollection);

//  if (muonCollection->size() != 0) return false;

//  reco::TrackCollection tracks = *(trackCollection.product());
  reco::TrackCollection tracks = *(muonCollection.product());
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

    if (fabs(dphi) < mindphi ) mindphi = fabs(dphi);
    if (fabs(deta) < mindeta ) mindeta = fabs(deta);

    float pt_rec = track.pt();
//    std::cout <<"RECO: pt:" <<pt_rec<<" phi: "<<phi<<" eta: "<<eta<<endl;
    if ( fabs(dphi) < 1.2 && fabs(deta) < 0.25 && pt_rec > 2.) {
     matched = true;
   }
   
//    Analysis::print(track);
  }
  return matched;
}
