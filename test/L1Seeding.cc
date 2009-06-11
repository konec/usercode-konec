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


#include "RecoTracker/TkMSParametrization/interface/PixelRecoUtilities.h"
#include "RecoTracker/TkMSParametrization/interface/PixelRecoPointRZ.h"
#include "RecoTracker/TkMSParametrization/interface/PixelRecoLineRZ.h"
#include "RecoMuon/TrackerSeedGenerator/interface/L1MuonPixelTrackFitter.h"

typedef L1MuonPixelTrackFitter::Circle Circle;

#include "R2DTimerObserver.h"

using namespace std;
using namespace ctfseeding;

struct HistoKey {
  int iEta, iPt;
  std::string type;

  HistoKey(){}
  HistoKey(const std::string & atype, float eta, float pt) : type(atype) {
    iEta = min(23,int(fabs(10.*eta)+1.e-4));
    iPt  = int(pt+1.e-4);
  }

  std::string name() const {
     std::ostringstream str; str << type <<"_"<<iEta<<"_"<<iPt; return str.str();
  }

  bool operator< (const HistoKey & other) const {
    if (iEta < other.iEta) return true;
    if (iEta > other.iEta) return false;
    if (iPt  < other.iPt)  return true;
    return false;
  }
};



class L1Seeding : public edm::EDAnalyzer {
public:
  explicit L1Seeding(const edm::ParameterSet& conf);
  ~L1Seeding();
  virtual void beginJob(const edm::EventSetup& iSetup);
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() { }
private:
  void bookBendingHisto(float eta, float pt);
private:
  edm::ParameterSet theConfig;
  OrderedHitsGenerator * theGenerator;
  TrackingRegionProducer* theRegionProducer;
  int eventCount;
  Analysis * theAnalysis;
  SeedingLayerSets theLayers;

  TProfile *hNumHP;
  TH1D *hCPU;
  TH1D *hEtaDist, *hPhiDist;
  TH1D *hPt, *hEta, *hPhi, *hTIP, *hZIP;
  TH1D *hEffCharge_N, *hEffCharge_D;
  R2DTimerObserver * theTimer;
  TH1D * hTMP;

  typedef std::map<HistoKey, TH1D *> HistoMap;
  HistoMap theL1BendingHisto;
  HistoMap thePtRes, thePhiRes, theCotRes, theTipRes, theZipRes;

  L1MuonPixelTrackFitter theFitter;
  
};

L1Seeding::L1Seeding(const edm::ParameterSet& conf) 
  : theConfig(conf), theGenerator(0), theRegionProducer(0), eventCount(0), theAnalysis(0),
    theTimer(0), theFitter(L1MuonPixelTrackFitter(conf.getParameter<edm::ParameterSet>("FitterPSet")))
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
  LogDebug("L1Seeding")<<" HERE DTOR";
}

void L1Seeding::beginJob(const edm::EventSetup& es)
{
  std::cout <<"HERE BJ" << std::endl;

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

  int npt = 16;
  float pt[] = { 5, 6, 7, 8, 9, 10, 12, 15, 20, 25, 30, 40, 50, 75, 99, 1000}; 
  for (int ieta=0; ieta <=23; ieta++) {
    for (int ipt=0; ipt < npt; ipt++) {
     bookBendingHisto( float(ieta)/10., pt[ipt]);
    }
  }

  int Nsize = 7;
  hNumHP = new TProfile("hNumHP","NTRACKS", Nsize,0.,float(Nsize),"S");
  hCPU = new TH1D ("hCPU","hCPU",20,0.,0.05);
  hEtaDist = new TH1D("hEtaDist","hEtaDist",100,-0.2,2.5);
  hPhiDist = new TH1D("hPhiDist","hPhiDist",1000,-1.,7.);

  hPt = new TH1D("hPt","hPt",100,-8,8);
  hTIP = new TH1D("hTIP","hTIP",100,-8,8);
  hZIP = new TH1D("hZIP","hZIP",100,-8,8);
  hEta= new TH1D("hEta","hEta",100,-8,8);
  hPhi= new TH1D("hPhi","hPhi",100, -8,8);

  hEffCharge_N = new TH1D("hEffCharge_N","hEffCharge_N",npt-1,pt);
  hEffCharge_D = new TH1D("hEffCharge_D","hEffCharge_D",npt-1,pt);

  hTMP = new TH1D("hTMP","hTMP",100,-0.1,0.1);

  gHistos.Add(hEffCharge_N);
  gHistos.Add(hEffCharge_D);
  gHistos.Add(hEtaDist);
  gHistos.Add(hPhiDist);
  gHistos.Add(hPt);
  gHistos.Add(hTIP);
  gHistos.Add(hZIP);
  gHistos.Add(hEta);
  gHistos.Add(hPhi);
  gHistos.Add(hTMP);

  typedef HistoMap::const_iterator IHM;
  for (IHM i=theL1BendingHisto.begin(); i!=theL1BendingHisto.end();++i) gHistos.Add(i->second);
  for (IHM i=thePtRes.begin(); i!=thePtRes.end();++i) gHistos.Add(i->second);
  for (IHM i=thePhiRes.begin(); i!=thePhiRes.end();++i) gHistos.Add(i->second);
  for (IHM i=theCotRes.begin(); i!=theCotRes.end();++i) gHistos.Add(i->second);
  for (IHM i=theTipRes.begin(); i!=theTipRes.end();++i) gHistos.Add(i->second);
  for (IHM i=theZipRes.begin(); i!=theZipRes.end();++i) gHistos.Add(i->second);

}


void L1Seeding::bookBendingHisto(float eta, float pt)
{
  HistoKey key;

  key = HistoKey("hDPhiL1",eta,pt);
  float phi = theFitter.getBending(1./pt,eta,1);
  double dphi = max(0.2,0.5*fabs(phi));
  double phi_min = phi-dphi;
  double phi_max = phi+dphi;
  cout <<"START NEW BENDING HISTO:" 
       <<"eta: " << eta<<" pt: " << pt
       <<" histo: " <<key.name().c_str()
       <<" range(phi bending): "<< phi_min<<"max:"<<phi_max<<endl; 
  theL1BendingHisto[key] =  new TH1D(key.name().c_str(),key.name().c_str(),50,phi_min,phi_max);

  key = HistoKey("hPtRes",eta,pt);
  double val = 1.; if (fabs(pt) > 29.) val = 2.; if (fabs(eta) > 1.1 ) val *= 2; 
  thePtRes[key] = new TH1D(key.name().c_str(), key.name().c_str(), 50,-val,val);

  key = HistoKey("hPhiRes",eta,pt); 
  thePhiRes[key] = new TH1D(key.name().c_str(),key.name().c_str(), 100,-0.01,0.01);

  key = HistoKey("hCotRes",eta,pt); 
  theCotRes[key] = new TH1D(key.name().c_str(),key.name().c_str(), 100,-0.01,0.01);

  key = HistoKey("hTipRes",eta,pt);
  theTipRes[key] = new TH1D(key.name().c_str(),key.name().c_str(), 50,-0.04,0.04);

  key = HistoKey("hZipRes",eta,pt);
  theZipRes[key] = new TH1D(key.name().c_str(),key.name().c_str(), 50,-0.05,0.05);

}


void L1Seeding::analyze(
    const edm::Event& ev, const edm::EventSetup& es)
{
  cout <<"*** L1Seeding, analyze event: " << ev.id()<<" event count:"<<++eventCount << endl;

  theAnalysis->init(ev,es);
  const SimTrack * track = theAnalysis->bestTrack();
  const SimVertex * vertex = theAnalysis->vertex(track);
  if (!track) return;
  Analysis::print(*track);
  float phi_gen = track->momentum().phi();
  float eta_gen = track->momentum().eta();
  float pt_gen  = track->momentum().pt();

  int charge_gen = (track->type() == 13) ? -1 : 1; 

  edm::Handle< edm::DetSetVector<PixelDigi> > digiCollection;
  ev.getByType( digiCollection);

  // Get GMTReadoutCollection

  edm::Handle<L1MuGMTReadoutCollection> gmtrc_handle;
  ev.getByType(gmtrc_handle);
  L1MuGMTReadoutCollection const* gmtrc = gmtrc_handle.product();
  vector<L1MuGMTReadoutRecord> gmt_records = gmtrc->getRecords();
  vector<L1MuGMTReadoutRecord>::const_iterator igmtrr;

  int nMuons = 0;
  for(igmtrr=gmt_records.begin(); igmtrr!=gmt_records.end(); igmtrr++) {
    vector<L1MuGMTExtendedCand>::const_iterator gmt_iter;
    vector<L1MuGMTExtendedCand> exc = igmtrr->getGMTCands();
    if (exc.size() <= 0) continue;
    L1MuGMTExtendedCand & muon = exc.front();
    if (muon.bx() != 0 ) continue;
    if (muon.empty() ) continue;
    nMuons++;

    hEffCharge_D->Fill(pt_gen);

    if (charge_gen==muon.charge()) hEffCharge_N->Fill(pt_gen);

    float phi_rec = muon.phiValue()+0.021817;

    hEtaDist->Fill(fabs(muon.etaValue()));
    hPhiDist->Fill(phi_rec);
    

    TH1D * hDPhiL1 =  theL1BendingHisto[HistoKey("hDPhiL1",muon.etaValue(), pt_gen)];
    if (!hDPhiL1)  abort();
    //hDPhiL1->Fill(charge_gen*theFitter.deltaPhi(phi_gen,phi_rec));

    
    float dx = cos(phi_rec);
    float dy = sin(phi_rec);
    float dz = sinh(muon.etaValue());
    GlobalVector dir(dx,dy,dz);
    GlobalPoint vtx(0.,0.,0.);


    //RectangularEtaPhiTrackingRegion region( dir, vtx, 10.,  0.1, 16., 0.15, 0.35);
    GlobalTrackingRegion region( 5., vtx, 0.1, 16., true);
    theTimer->start();
    const OrderedSeedingHits & candidates = theGenerator->run(region,ev,es);
    theTimer->stop();
    cout << "TIMING IS: (real)" << theTimer->lastMeasurement().real() << endl;
    hCPU->Fill( theTimer->lastMeasurement().real() );

    int numFiltered = 0;
    hNumHP->Fill(2, float(candidates.size()));
    unsigned int nSets = candidates.size();

    for (unsigned int ic= 0; ic <nSets; ic++) {
      const SeedingHitSet & hits = candidates[ic];
      GlobalPoint p0 =  hits[0]->globalPosition();
      GlobalPoint p1 =  hits[1]->globalPosition();
      PixelRecoLineRZ line(p0,p1);

      double phi_hit = (p1-p0).phi();
      hDPhiL1->Fill(charge_gen*theFitter.deltaPhi(phi_hit,phi_rec));

      double invPt_rec = theFitter.valInversePt(phi_hit, phi_rec, muon.etaValue()); 
      //double invPtErr  = theFitter.errInversePt(invPt_rec, muon.etaValue()); 
      double invPtErr  = theFitter.errInversePt(1/pt_gen, muon.etaValue()); 
    
      int charge_rec = (invPt_rec > 0.) ? 1 : -1;
      //double curvature = PixelRecoUtilities::curvature(invPt_rec, es); 
      double curvature = PixelRecoUtilities::curvature(charge_gen*(1/pt_gen), es); 
      Circle circle(p0,p1,curvature);
      //float phi_vtx = theFitter.valPhi(circle, charge_rec);
      float phi_vtx = theFitter.valPhi(circle, charge_gen);

      double tip = theFitter.valTip(circle,curvature);
      double zip = theFitter.valZip(curvature, p0,p1) - vertex->position().z();

      //pull histos
      hTIP->Fill(tip/theFitter.errTip(invPt_rec,muon.etaValue()));
      hZIP->Fill(zip/theFitter.errZip(invPt_rec,muon.etaValue()));
      hPhi->Fill(theFitter.deltaPhi(phi_gen,phi_vtx)/theFitter.errPhi(invPt_rec,muon.etaValue()));
      hEta->Fill((theFitter.valCotTheta(line)-sinh(eta_gen))/theFitter.errCotTheta(invPt_rec,muon.etaValue()));  
      hPt->Fill( (invPt_rec-charge_gen/pt_gen)/invPtErr);
      hTMP->Fill( charge_gen*theFitter.deltaPhi(phi_gen,phi_rec) );

      // resolution histos
      thePtRes[ HistoKey( "hPtRes",eta_gen,pt_gen)]->Fill(charge_gen*pt_gen*invPt_rec-1.);
      thePhiRes[HistoKey("hPhiRes",eta_gen,pt_gen)]->Fill( theFitter.deltaPhi(phi_gen, phi_vtx));
      theCotRes[HistoKey("hCotRes",eta_gen,pt_gen)]->Fill(line.cotLine()-sinh(eta_gen));
      theTipRes[HistoKey("hTipRes",eta_gen,pt_gen)]->Fill(tip);
      theZipRes[HistoKey("hZipRes",eta_gen,pt_gen)]->Fill(zip);

    }
    hNumHP->Fill(3, float(numFiltered));
  }
  if (nMuons > 0) cout <<"NMUONS: there were "<<nMuons <<" L1 muons in the event"<<endl;
}


DEFINE_FWK_MODULE(L1Seeding);
