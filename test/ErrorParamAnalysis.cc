#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"

#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/print.h"


//add simhit info
#include "SimTracker/TrackerHitAssociation/interface/TrackerHitAssociator.h"

//simtrack
#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"

#include "UserCode/konec/interface/Analysis.h"
#include "R2DTimerObserver.h"
#include "TProfile.h"
#include "TH1D.h"
#include <sstream>



using namespace std;
using namespace ctfseeding;

struct HistoKey {
  int iEta, iPt;
  std::string type;

  HistoKey(){}
  HistoKey(const std::string & atype, float eta, float pt) : type(atype) {
      iEta = min(24,int(fabs(10.*eta)+1.e-4));
      iPt  = int(10.*pt+1.e-4);
  }

  std::string name() const { std::stringstream str; str << type <<"_"<<iEta<<"_"<<iPt; return str.str(); }

  bool operator< (const HistoKey & other) const {
    if (iEta < other.iEta) return true;
    if (iEta > other.iEta) return false;
    if (iPt  < other.iPt)  return true;
    return false;
  }
};



class ErrorParamAnalysis : public edm::EDAnalyzer {
public:
  explicit ErrorParamAnalysis(const edm::ParameterSet& conf);
  ~ErrorParamAnalysis();
  virtual void beginJob();
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() { }
private:
  void bookParamHistos(float eta, float pt);
private:
  edm::ParameterSet theConfig;
  int eventCount;
  Analysis * theAnalysis;
  TH1D *hPhi, *hNum, *hPt; 
  typedef std::map<HistoKey, TH1D *> HistoMap;
  HistoMap thePtRes, theTipRes, theZipRes;
};


ErrorParamAnalysis::ErrorParamAnalysis(const edm::ParameterSet& conf) 
  : theConfig(conf), eventCount(0), 
   theAnalysis(0) 
{
  edm::LogInfo("ErrorParamAnalysis")<<" CTORXX";
}


ErrorParamAnalysis::~ErrorParamAnalysis() 
{ 
  delete theAnalysis;
  edm::LogInfo("ErrorParamAnalysis")<<" DTOR";
}

void ErrorParamAnalysis::beginJob()
{
  edm::ParameterSet apset = theConfig.getParameter<edm::ParameterSet>("AnalysisPSet");
  theAnalysis = new Analysis(apset);
  hPhi = new  TH1D("hPhi","hPhi",100, -3.2,3.2);
  hPt = new TH1D("hPt","hPt",100,0.,5.);
  hNum = new TH1D("hNum","hNum",40,0.,40.);

  gHistos.Add(hPhi);
  gHistos.Add(hPt);
  gHistos.Add(hNum);

  int npt = 18;
  float pt[] = { 0.1, 0.2, 0.3, 0.4, 0.6, 0.8, 1.0, 1.2, 1.5, 2.0, 2.5, 3, 4, 5, 6, 7, 8, 9};
  for (int ieta=0; ieta <=24; ieta++) {
    for (int ipt=0; ipt < npt; ipt++) {
      bookParamHistos( float(ieta)/10., pt[ipt]);
    }
  }
  typedef HistoMap::const_iterator IHM;
  for (IHM i=thePtRes.begin(); i!=thePtRes.end();++i) gHistos.Add(i->second);
  for (IHM i=theTipRes.begin(); i!=theTipRes.end();++i) gHistos.Add(i->second);
  for (IHM i=theZipRes.begin(); i!=theZipRes.end();++i) gHistos.Add(i->second);
}

void ErrorParamAnalysis::bookParamHistos(float eta, float pt)
{
   HistoKey key;
   double val;

   key = HistoKey("hPtRes",eta,pt);
   val = 1.0; 
   //if (fabs(pt) > 29.) val = 2.; if (fabs(eta) > 1.1 ) val *= 2;
   thePtRes[key] = new TH1D(key.name().c_str(), key.name().c_str(), 100,-val,val);

   key = HistoKey("hTipRes",eta,pt);
   val = 0.1; if (pt < 2.) val = 0.3; if( pt < 0.5) val=1.; 
   theTipRes[key]  = new TH1D(key.name().c_str(), key.name().c_str(), 100, -val, val);

   key = HistoKey("hZipRes",eta,pt);
   val=0.1; if (pt< 1.) val *=5; if (fabs(eta) > 2.) val *= 2;
   theZipRes[key]  = new TH1D(key.name().c_str(), key.name().c_str(), 100, -val, val);

}

void ErrorParamAnalysis::analyze(
    const edm::Event& ev, const edm::EventSetup& es)
{
  cout <<"*** ErrorParamAnalysisA, analyze event: " << ev.id()<<" event count:"<<++eventCount << endl;

  theAnalysis->init(ev,es,0);
  const SimTrack* simTrack = theAnalysis->bestTrack();
  const SimVertex * simVertex = theAnalysis->vertex(simTrack);
  if (!simTrack) return;
  if (!simVertex) return;
  math::XYZPoint bs(simVertex->position().x(),
		    simVertex->position().y(),
  		    simVertex->position().z());
//
//     math::XYZPoint bs(0,0,0);

//  Analysis::print(*simtrack);
//  float phi_gen = simTrack->momentum().phi();
  float eta_gen = simTrack->momentum().eta();
  float pt_gen  = simTrack->momentum().pt();

  edm::Handle<reco::TrackCollection> trackCollection;
  std::string collectionLabel = theConfig.getParameter<std::string>("TrackCollection");
  ev.getByLabel(collectionLabel,trackCollection);

  typedef reco::TrackCollection::const_iterator IT;

  for (IT it = trackCollection->begin(); it < trackCollection->end(); ++it) {
    const reco::Track & track = *it;
    double pt_rec = track.pt();
    double tip    = track.dxy(bs);
    double zip    = track.dz(bs); 
//    std::cout <<"ZIP: " << zip << std::endl;
//    Analysis::print(track);
    thePtRes[ HistoKey( "hPtRes",eta_gen,pt_gen)]->Fill(pt_rec/pt_gen-1.);
    theTipRes[ HistoKey( "hTipRes",eta_gen,pt_gen)]->Fill(tip);
    theZipRes[ HistoKey( "hZipRes",eta_gen,pt_gen)]->Fill(zip);
  }

//  theAnalysis->checkEfficiency(tracks);

}

DEFINE_FWK_MODULE(ErrorParamAnalysis);

