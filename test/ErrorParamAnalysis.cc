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
#include "DataFormats/Math/interface/deltaPhi.h"


//add simhit info
#include "SimTracker/TrackerHitAssociation/interface/TrackerHitAssociator.h"

//simtrack
#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"

#include "UserCode/konec/interface/Analysis.h"
#include "R2DTimerObserver.h"
#include "TProfile.h"
#include "TH1D.h"
#include "TH2D.h"
#include <sstream>
#include "RecoPixelVertexing/PixelTrackFitting/interface/PixelTrackErrorParam.h"



using namespace std;
using namespace ctfseeding;
template <class T> T sqr( T t) {return t*t;}

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
  HistoMap thePtRes, theTipRes, theZipRes, theCotRes, thePhiRes;
  HistoMap thePtPulls, theTipPulls, theZipPulls, theCotPulls, thePhiPulls;
  HistoMap thePtPullsBar, thePtPullsInt, thePtPullsEnd;
  TProfile *hProfPtPullsBar, *hProfPtPullsInt, *hProfPtPullsEnd, *hProfPtPullsAll;
  TH2D* hCorrPullsPtPhi;
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
  hCorrPullsPtPhi = new TH2D("hCorrPullsPtPhi","hCorrPullsPtPhi",100,-8.,8., 100, -8.,8.);

  gHistos.Add(hPhi);
  gHistos.Add(hPt);
  gHistos.Add(hNum);

  gHistos.Add(hCorrPullsPtPhi);

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
  for (IHM i=theCotRes.begin(); i!=theCotRes.end();++i) gHistos.Add(i->second);
  for (IHM i=thePhiRes.begin(); i!=thePhiRes.end();++i) gHistos.Add(i->second);

  //
  // Pulls histos
  //
  for (int ipt=0; ipt < npt; ipt++) {
    HistoKey key;

    key = HistoKey("hPtPulls",0,pt[ipt]);    thePtPulls[key] = new TH1D(key.name().c_str(), key.name().c_str(), 100, -8., 8.);
    key = HistoKey("hPtPullsBar",0,pt[ipt]); thePtPullsBar[key] = new TH1D(key.name().c_str(), key.name().c_str(), 100, -8., 8.);
    key = HistoKey("hPtPullsInt",0,pt[ipt]); thePtPullsInt[key] = new TH1D(key.name().c_str(), key.name().c_str(), 100, -8., 8.);
    key = HistoKey("hPtPullsEnd",0,pt[ipt]); thePtPullsEnd[key] = new TH1D(key.name().c_str(), key.name().c_str(), 100, -8., 8.);

    key = HistoKey("hTipPulls",0,pt[ipt]);
    theTipPulls[key] = new TH1D(key.name().c_str(), key.name().c_str(), 100, -8., 8.);

    key = HistoKey("hZipPulls",0,pt[ipt]);
    theZipPulls[key] = new TH1D(key.name().c_str(), key.name().c_str(), 100, -8., 8.);

    key = HistoKey("hCotPulls",0,pt[ipt]);
    theCotPulls[key] = new TH1D(key.name().c_str(), key.name().c_str(), 100, -8., 8.);

    key = HistoKey("hPhiPulls",0,pt[ipt]);
    thePhiPulls[key] = new TH1D(key.name().c_str(), key.name().c_str(), 100, -8., 8.);
  }
  for (IHM i=thePtPulls.begin(); i!=thePtPulls.end();++i) gHistos.Add(i->second);
  for (IHM i=thePtPullsBar.begin(); i!=thePtPullsBar.end();++i) gHistos.Add(i->second);
  for (IHM i=thePtPullsInt.begin(); i!=thePtPullsInt.end();++i) gHistos.Add(i->second);
  for (IHM i=thePtPullsEnd.begin(); i!=thePtPullsEnd.end();++i) gHistos.Add(i->second);
  for (IHM i=theTipPulls.begin(); i!=theTipPulls.end();++i) gHistos.Add(i->second);
  for (IHM i=theZipPulls.begin(); i!=theZipPulls.end();++i) gHistos.Add(i->second);
  for (IHM i=theCotPulls.begin(); i!=theCotPulls.end();++i) gHistos.Add(i->second);
  for (IHM i=thePhiPulls.begin(); i!=thePhiPulls.end();++i) gHistos.Add(i->second);

  hProfPtPullsBar = new TProfile("hProfPtPullsBar","hProfPtPullsBar", 100,0.,10., -8., 8., "s");  gHistos.Add(hProfPtPullsBar);
  hProfPtPullsInt = new TProfile("hProfPtPullsInt","hProfPtPullsInt", 100,0.,10., -8., 8., "s");  gHistos.Add(hProfPtPullsInt);
  hProfPtPullsEnd = new TProfile("hProfPtPullsEnd","hProfPtPullsEnd", 100,0.,10., -8., 8., "s");  gHistos.Add(hProfPtPullsEnd);
  hProfPtPullsAll = new TProfile("hProfPtPullsAll","hProfPtPullsAll", 100,0.,10., -8., 8., "s");  gHistos.Add(hProfPtPullsAll);
  
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

   key = HistoKey("hCotRes",eta,pt);
   val=0.05; if (pt<1.)val *=2; if(pt<0.5) val*=2; if(pt<0.35) val*=2; if (fabs(eta)>2.) val *= 2;
   theCotRes[key]  = new TH1D(key.name().c_str(), key.name().c_str(), 100, -val, val);

   key = HistoKey("hPhiRes",eta,pt);
   val=0.05; if (pt< 1.) val *=2;  if (pt< 0.35) val *=2;
   thePhiRes[key]  = new TH1D(key.name().c_str(), key.name().c_str(), 100, -val, val);


}

void ErrorParamAnalysis::analyze(
    const edm::Event& ev, const edm::EventSetup& es)
{
  cout <<"*** ErrorParamAnalysis, analyze event: " << ev.id()<<" event count:"<<++eventCount << endl;

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
  float phi_gen = simTrack->momentum().phi();
  float cot_gen = simTrack->momentum().pz()/simTrack->momentum().pt();
  float eta_gen = simTrack->momentum().eta();
  float pt_gen  = simTrack->momentum().pt();
  int   charge_gen = simTrack->charge();

  edm::Handle<reco::TrackCollection> trackCollection;
  std::string collectionLabel = theConfig.getParameter<std::string>("TrackCollection");
  ev.getByLabel(collectionLabel,trackCollection);

  typedef reco::TrackCollection::const_iterator IT;

  const reco::Track* aTrack = 0;
  double pt_max = 0.;
  for (IT it = trackCollection->begin(); it < trackCollection->end(); ++it) {
    if (it->charge() != charge_gen) continue;
    if (it->pt() > pt_max) {pt_max = it->pt(); aTrack = &(*it); }
  }
  if (aTrack) {
    const reco::Track & track = *aTrack;
    double pt_rec = track.pt();
    double tip    = track.dxy(bs);
    double zip    = track.dz(bs); 
    double cot    = track.pz()/track.pt(); 
    double phi    = track.momentum().phi(); 
    double dPhi   = deltaPhi(phi,phi_gen);
//    std::cout <<"ZIP: " << zip << std::endl;
//    Analysis::print(track);
    thePtRes[ HistoKey( "hPtRes",eta_gen,pt_gen)]->Fill(pt_rec/pt_gen-1.);
    theTipRes[ HistoKey( "hTipRes",eta_gen,pt_gen)]->Fill(tip);
    theZipRes[ HistoKey( "hZipRes",eta_gen,pt_gen)]->Fill(zip);
    theCotRes[ HistoKey( "hCotRes",eta_gen,pt_gen)]->Fill(cot-cot_gen);
    thePhiRes[ HistoKey( "hPhiRes",eta_gen,pt_gen)]->Fill(dPhi);

//    thePtPulls[  HistoKey( "hPtPulls",0,pt_gen)]->Fill( (pt_gen - pt_rec)/ track.ptError());
    
    thePtPulls[  HistoKey( "hPtPulls",0,pt_gen)]->Fill( (1./pt_rec - 1./pt_gen)/ (track.ptError()/sqr(pt_rec) ));
    theTipPulls[  HistoKey( "hTipPulls",0,pt_gen)]->Fill( tip/ track.d0Error());
    theZipPulls[ HistoKey( "hZipPulls",0,pt_gen)]->Fill( zip/ track.dzError());

    double errorCot =  track.lambdaError()/sqr( sin(track.theta()));
    theCotPulls[ HistoKey( "hCotPulls",0,pt_gen)]->Fill( (cot-cot_gen)/ errorCot);
    thePhiPulls[ HistoKey( "hPhiPulls",0,pt_gen)]->Fill( dPhi/ track.phiError());

    double pullPt = (1./pt_rec - 1./pt_gen)/ (track.ptError()/sqr(pt_rec) );
    double pullPhi = dPhi/ track.phiError();
    hCorrPullsPtPhi->Fill(pullPhi,pullPt);
    
    hProfPtPullsAll->Fill(pt_gen, pullPt);
    if (fabs(eta_gen) < 1.) { hProfPtPullsBar->Fill(pt_gen, pullPt);  thePtPullsBar[  HistoKey( "hPtPullsBar",0,pt_gen)]->Fill(pullPt); } 
    else if ( fabs(eta_gen) < 1.5) { hProfPtPullsInt->Fill(pt_gen, pullPt); thePtPullsInt[  HistoKey( "hPtPullsInt",0,pt_gen)]->Fill(pullPt); } 
    else { hProfPtPullsEnd->Fill(pt_gen, pullPt); thePtPullsEnd[  HistoKey( "hPtPullsEnd",0,pt_gen)]->Fill(pullPt); } 

/*
   PixelTrackErrorParam param(eta_gen,pt_gen);
   thePtPulls[  HistoKey( "hPtPulls",0,pt_gen)]->Fill( (1./pt_rec - 1./pt_gen)/(param.errPt()/sqr(pt_rec) ));
   theTipPulls[  HistoKey( "hTipPulls",0,pt_gen)]->Fill( tip/ param.errTip());
   theZipPulls[ HistoKey( "hZipPulls",0,pt_gen)]->Fill( zip/ param.errZip());
   theCotPulls[ HistoKey( "hCotPulls",0,pt_gen)]->Fill( (cot-cot_gen)/ param.errCot());
   thePhiPulls[ HistoKey( "hPhiPulls",0,pt_gen)]->Fill( dPhi/ param.errPhi());
*/


  }

//  theAnalysis->checkEfficiency(tracks);

}

DEFINE_FWK_MODULE(ErrorParamAnalysis);

