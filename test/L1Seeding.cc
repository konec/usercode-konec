//#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"

//add simhit info
#include "SimTracker/TrackerHitAssociation/interface/TrackerHitAssociator.h"

//simtrack
#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"

#include "RecoTracker/TkTrackingRegions/interface/RectangularEtaPhiTrackingRegion.h"

#include "RecoTracker/TkTrackingRegions/interface/TrackingRegionProducerFactory.h"
#include "RecoTracker/TkTrackingRegions/interface/TrackingRegionProducer.h"
#include "RecoTracker/TkTrackingRegions/interface/TrackingRegion.h"

#include "RecoTracker/TkTrackingRegions/interface/OrderedHitsGeneratorFactory.h"
#include "RecoTracker/TkTrackingRegions/interface/OrderedHitsGenerator.h"
#include "RecoTracker/TkTrackingRegions/interface/GlobalTrackingRegion.h"

#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "RecoTracker/TkSeedingLayers/interface/SeedingLayerSetsBuilder.h"
#include "RecoTracker/TkSeedingLayers/interface/SeedingLayerSets.h"

#include "UserCode/konec/interface/Analysis.h"
#include "TProfile.h"
#include "TH1D.h"


#include "CondFormats/L1TObjects/interface/L1MuTriggerScales.h"
#include "CondFormats/DataRecord/interface/L1MuTriggerScalesRcd.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTReadoutCollection.h"



#include "R2DTimerObserver.h"

using namespace std;
using namespace ctfseeding;


class L1Seeding : public edm::EDAnalyzer {
public:
  explicit L1Seeding(const edm::ParameterSet& conf);
  ~L1Seeding();
  virtual void beginJob(const edm::EventSetup& iSetup);
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() { }
private:
  float deltaPhi(float phi1, float phi2) const;
  float getPt(float phi0, float phiL1, float eta, float charge) const;
  float getBending(float eta, float pt, float charge) const; 
  void param(float eta, float &p1, float& p2) const;
private:
  edm::ParameterSet theConfig;
  OrderedHitsGenerator * theGenerator;
  TrackingRegionProducer* theRegionProducer;
  int eventCount;
  Analysis * theAnalysis;
  SeedingLayerSets theLayers;

  TProfile *hNumHP;
  TProfile *hDPhiL1[21];
  TH1D *hCPU;
  TH1D *hEtaDiff, *hPhiDiff, *hPt;
  TH1D *hEta, *hPhi;
  TH1D *hCharge;
  TH1D *hDphiTEST;
  R2DTimerObserver * theTimer;
  
};


L1Seeding::L1Seeding(const edm::ParameterSet& conf) 
  : theConfig(conf), theGenerator(0), theRegionProducer(0), eventCount(0), theAnalysis(0),
    theTimer(0)
{
  edm::LogInfo("L1Seeding")<<" CTORXX";
  theTimer = new R2DTimerObserver("**** MY TIMING REPORT ***");

}


L1Seeding::~L1Seeding() 
{ 
  delete theAnalysis;
  delete theRegionProducer;
  delete theGenerator;
  delete theTimer;
  edm::LogInfo("L1Seeding")<<" DTOR";
}

void L1Seeding::beginJob(const edm::EventSetup& es)
{

/*
  edm::ParameterSet regfactoryPSet =
      theConfig.getParameter<edm::ParameterSet>("RegionFactoryPSet");
  std::string regfactoryName = regfactoryPSet.getParameter<std::string>("ComponentName");
  theRegionProducer = TrackingRegionProducerFactory::get()->create(regfactoryName,regfactoryPSet);
*/

  edm::ParameterSet orderedPSet =
      theConfig.getParameter<edm::ParameterSet>("OrderedHitsFactoryPSet");
  std::string orderedName = orderedPSet.getParameter<std::string>("ComponentName");
  theGenerator = OrderedHitsGeneratorFactory::get()->create( orderedName, orderedPSet);

  std::string layerBuilderName = orderedPSet.getParameter<std::string>("SeedingLayers");
  edm::ESHandle<SeedingLayerSetsBuilder> layerBuilder;
  es.get<TrackerDigiGeometryRecord>().get(layerBuilderName, layerBuilder);

  theLayers  =  layerBuilder->layers(es);
  cout <<"Number of Sets: " << theLayers.size() << endl;
  typedef SeedingLayerSets::const_iterator ISLS;
  typedef SeedingLayers::const_iterator ISL;

  for (ISLS isls =theLayers.begin(); isls != theLayers.end(); isls++) {
    cout <<"Layers: ";
    for (ISL isl = isls->begin(); isl != isls->end(); isl++) cout <<(*isl).name()<<" ";
    cout << endl;
  }

  edm::ParameterSet apset = theConfig.getParameter<edm::ParameterSet>("AnalysisPSet");
  theAnalysis = new Analysis(apset);

  int Nsize = 7;
  hNumHP = new TProfile("hNumHP","NTRACKS", Nsize,0.,float(Nsize),"S");
  hCPU = new TH1D ("hCPU","hCPU",20,0.,0.05);
  hEtaDiff = new TH1D("hEtaDiff","hEtaDiff",100,-0.15,0.15);
  hPhiDiff = new TH1D("hPhiDiff","hPhiDiff",100, -0.2,0.2);
  hEta = new TH1D("hEta","hEta",100,-0.2,2.5);
  hPhi = new TH1D("hPhi","hPhi",1000,-1.,7.);
  hPt = new TH1D("hPt","hPt",100,0.,25.);
  hCharge = new TH1D("hCharge","hCharge",5,-2.5,2.5);
  TProfile hTmp("hTmp","Fit",96,4.5,100.5);

  hDphiTEST = new TH1D("hDphiTEST","hDphiTEST",100,-0.5, 0.);
  

  char name[80];
  for (int ieta=0; ieta <=20; ieta++) {
    sprintf(name,"hDPhiL1_%d",ieta);
    hDPhiL1[ieta] = new TProfile(hTmp);
    hDPhiL1[ieta]->SetNameTitle(name,name);
  }

}


void L1Seeding::analyze(
    const edm::Event& ev, const edm::EventSetup& es)
{
  cout <<"*** L1Seeding, analyze event: " << ev.id()<<" event count:"<<++eventCount << endl;


  const SimTrack * track = theAnalysis->bestTrack(ev);
  if (!track) return;
  Analysis::print(*track);
  float phi_gen = track->momentum().phi();
  float eta_gen = track->momentum().eta();
  float pt_gen  = track->momentum().perp();

  int charge_gen = (track->type() == 13) ? -1 : 1; 

  edm::Handle< edm::DetSetVector<PixelDigi> > digiCollection;
  ev.getByType( digiCollection);

  // Get GMTReadoutCollection

  edm::Handle<L1MuGMTReadoutCollection> gmtrc_handle;
  ev.getByLabel("l1GmtEmulDigis",gmtrc_handle);
  L1MuGMTReadoutCollection const* gmtrc = gmtrc_handle.product();
  vector<L1MuGMTReadoutRecord> gmt_records = gmtrc->getRecords();
  vector<L1MuGMTReadoutRecord>::const_iterator igmtrr;

  for(igmtrr=gmt_records.begin(); igmtrr!=gmt_records.end(); igmtrr++) {
    vector<L1MuGMTExtendedCand>::const_iterator gmt_iter;
    vector<L1MuGMTExtendedCand> exc = igmtrr->getGMTCands();
    if (exc.size() <= 0) continue;
    L1MuGMTExtendedCand & muon = exc.front();

//      bxg[igmt]=(*gmt_iter).bx();
//      ptg[igmt]=(*gmt_iter).ptValue();
//      chag[igmt]=(*gmt_iter).charge();
//      etag[igmt]=(*gmt_iter).etaValue();
//      phig[igmt]=(*gmt_iter).phiValue();
//      qualg[igmt]=(*gmt_iter).quality();
//      detg[igmt]=(*gmt_iter).detector();
//      rankg[igmt]=(*gmt_iter).rank();
//      isolg[igmt]=(*gmt_iter).isol();
//      mipg[igmt]=(*gmt_iter).mip();
//      int data = (*gmt_iter).getDataWord();

    float phi_rec = muon.phiValue()+0.021817;
    float phi_exp = phi_gen+getBending(eta_gen, pt_gen, charge_gen);
    hPhiDiff->Fill(deltaPhi(phi_rec,phi_exp));
    hEta->Fill(fabs(muon.etaValue()));
    int ieta = min(20, int(fabs(muon.etaValue()*10.)));
    hDPhiL1[ieta]->Fill(pt_gen, charge_gen*deltaPhi(phi_gen,phi_rec)); 

    if (fabs(muon.etaValue()) > 0.1 && fabs(muon.etaValue()) < 0.2) hDphiTEST->Fill(charge_gen*deltaPhi(phi_gen,phi_rec)); 
    
    hPhi->Fill(phi_rec);
//    hCharge->Fill(muon.charge());
    hCharge->Fill(charge_gen);
    
    float dx = cos(phi_rec);
    float dy = sin(phi_rec);
    float dz = sinh(muon.etaValue());
    GlobalVector dir(dx,dy,dz);
    GlobalPoint vtx(0.,0.,0.);

    cout <<"GlobalVector: phi:" << dir.phi()<<" eta:"<<dir.eta()<<endl;
    RectangularEtaPhiTrackingRegion region( dir, vtx, 10.,  0.1, 16., 0.15, 0.35);

    theTimer->start();
    const OrderedSeedingHits & candidates = theGenerator->run(region,ev,es);
    theTimer->stop();
    cout << "TIMING IS: (real)" << theTimer->lastMeasurement().real() << endl;
    hCPU->Fill( theTimer->lastMeasurement().real() );

    int numFiltered = 0;
    hNumHP->Fill(2, float(candidates.size()));
    unsigned int nSets = candidates.size();
    for (unsigned int ic= 0; ic <nSets; ic++) {
      typedef vector<ctfseeding::SeedingHit> RecHits;
      const RecHits & hits = candidates[ic].hits();
      float r0 = hits[0].r();
      float r1 = hits[1].r();
      GlobalPoint p0(r0*cos(hits[0].phi()), r0*sin(hits[0].phi()), 0.);
      GlobalPoint p1(r1*cos(hits[1].phi()), r1*sin(hits[1].phi()), 0.);

      float cotTheta = (hits[1].z()-hits[0].z())/(hits[1].r()-hits[0].r());
      float eta_rec = asinh(cotTheta);

      float phi_vtx = (p1-p0).phi();
//      float pt_rec = getPt(phi_vtx, phi_rec, muon.etaValue(), charge_gen); 
      float pt_rec = max(getPt(phi_vtx, phi_rec, muon.etaValue(), muon.charge()),
                         getPt(phi_vtx, phi_rec, muon.etaValue(), -muon.charge()));
//      hPhiDiff->Fill(deltaPhi(phi_gen,phi_vtx));
      hEtaDiff->Fill(eta_gen-eta_rec);
      hPt->Fill(pt_rec);
      if (pt_rec > 8.) numFiltered++;
    }
    hNumHP->Fill(3, float(numFiltered));
  }
}




float L1Seeding::deltaPhi(float phi1, float phi2) const
{
  while ( phi1 >= 2*M_PI) phi1 -= 2*M_PI;
  while ( phi2 >= 2*M_PI) phi2 -= 2*M_PI;
  while ( phi1 < 0) phi1 += 2*M_PI;
  while ( phi2 < 0) phi2 += 2*M_PI;
  float dPhi = phi2-phi1;
  
  if ( dPhi > M_PI ) dPhi =- 2*M_PI;  
  if ( dPhi < -M_PI ) dPhi =+ 2*M_PI;  

  return dPhi;
}




float L1Seeding::getPt(float phi0, float phiL1, float eta, float charge) const {

  float dphi_min = fabs(deltaPhi(phi0,phiL1));
  float pt_best = 1.;
  float pt_cur = 1;
  while ( pt_cur < 100.) {
    float phi_exp = phi0+getBending(eta, pt_cur, charge);    
    float dphi = fabs(deltaPhi(phi_exp,phiL1)); 
    if ( dphi < dphi_min) {
      pt_best = pt_cur;
      dphi_min = dphi; 
    }
    pt_cur += 0.01;
  };
  return pt_best; 
}




float L1Seeding::getBending(float eta, float pt, float charge) const 
{
  float p1, p2;
  param(eta,p1,p2);
  return charge*p1/pt + charge*p2/(pt*pt); // - 0.0218; 
}

void L1Seeding::param(float eta, float &p1, float& p2) const
{
  
  int ieta = int (10*fabs(eta));
  switch (ieta) {
  case 0:  { p1 = -2.658; p2 = -1.551; break; }
  case 1:  
  case 2:  { p1 = -2.733; p2 = -0.6316; break; }
  case 3:  { p1 = -2.607; p2 = -2.558; break; }
  case 4:  { p1 = -2.715; p2 = -0.9311; break; }
  case 5:  { p1 = -2.674; p2 = -1.145; break; }
  case 6:  { p1 = -2.731; p2 = -0.4343; break; }
  case 7:
  case 8:  { p1 = -2.684; p2 = -0.7035; break; }
  case 9:  
  case 10: { p1 = -2.659; p2 = -0.0325; break; }
  case 11: { p1 = -2.580; p2 = -0.77; break; }
  case 12: { p1 = -2412; p2 = 0.5242; break; }
  case 13: { p1 = -2.192; p2 = 1.691; break; }
  case 14: 
  case 15: { p1 = -1.891; p2 = 0.8936; break; }
  case 16: { p1 = -1.873; p2 = 2.287; break; }
  case 17: { p1 = -1.636; p2 = 1.881; break; }
  case 18: { p1 = -1.338; p2 = -0.006; break; }
  case 19: { p1 = -1.334; p2 = 1.036; break; }
  case 20: { p1 = -1.247; p2 = 0.461; break; }
  default: {p1 = -1.141; p2 = 2.06; }             //above eta 2.1
  }

}


/*
void L1Seeding::param(float eta, float &p1, float& p2) const
{
  
//  int ieta = int (10*fabs(eta));
//  switch (ieta) {
  float feta = fabs(eta);
  if (feta < 0.07) {
    //"0"
    p1 = -2.658;
    p2 = -1.551;
  } else if ( feta < 0.27) {
    //"2"
    p1 = -2.733;
    p2 = -0.6316;
  } else if ( feta < 0.44) {
    //"3"
    p1 = -2.607;
    p2 = -2.558;
  } else if ( feta < 0.58) {
    //"4"
    p1 = -2.715;
    p2 = -0.9311;
  } else if ( feta < 0.72) {
    //"5"
    p1 = -2.674;
    p2 = -1.145; 
  } else if ( feta < 0.83) {
    //"6"
    p1 = -2.731;
    p2 = -0.4343;
  }  else if ( feta < 0.93) {
    //"8"
    p1 = -2.684;
    p2 = -0.7035;
  } else {
    //"10"
    p1 = -2.659;
    p2 = -0.0325;
  }
}
*/

DEFINE_FWK_MODULE(L1Seeding);

