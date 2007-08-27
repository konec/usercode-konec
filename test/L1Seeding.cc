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


#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"
#include "CondFormats/L1TObjects/interface/L1MuTriggerScales.h"
#include "CondFormats/DataRecord/interface/L1MuTriggerScalesRcd.h"


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
  float getPt(float phi0, float phiL1, float eta)const;
  float getBending(float eta, float pt) const; 
  void param(float eta, float &p1, float& p2) const;
private:
  edm::ParameterSet theConfig;
  OrderedHitsGenerator * theGenerator;
  TrackingRegionProducer* theRegionProducer;
  int eventCount;
  Analysis * theAnalysis;
  SeedingLayerSets theLayers;

  TProfile *hNumHP;
  TH1D *hCPU;
  TH1D *hEtaDiff, *hPhiDiff, *hPt;
  TH1D *hEtaRPC, *hPhiRPC;
  R2DTimerObserver * theTimer;
  
};


L1Seeding::L1Seeding(const edm::ParameterSet& conf) 
  : theConfig(conf), theGenerator(0), eventCount(0), theAnalysis(0),
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

  edm::ParameterSet regfactoryPSet =
      theConfig.getParameter<edm::ParameterSet>("RegionFactoryPSet");
  std::string regfactoryName = regfactoryPSet.getParameter<std::string>("ComponentName");
  theRegionProducer = TrackingRegionProducerFactory::get()->create(regfactoryName,regfactoryPSet);

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
  hEtaRPC = new TH1D("hEtaRPC","hEtaRPC",100,-1.2,1.2);
  hPhiRPC = new TH1D("hPhiRPC","hPhiRPC",1000,-1.,7.);
  hPhiDiff = new TH1D("hPhiDiff","hPhiDiff",100, -0.35,0.35);
  hPt = new TH1D("hPt","hPt",100,0.,20.);
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

  edm::ESHandle< L1MuTriggerScales > trigscales_h;
  es.get< L1MuTriggerScalesRcd >().get( trigscales_h );
  const L1MuTriggerScales* theTriggerScales = trigscales_h.product(); 

  edm::Handle< edm::DetSetVector<PixelDigi> > digiCollection;
  ev.getByType( digiCollection);

  std::vector<edm::Handle<std::vector<L1MuRegionalCand> > > alldata;
  ev.getManyByType(alldata);
  std::vector<edm::Handle<std::vector<L1MuRegionalCand> > >::iterator it;
  TrackingRegion * region = 0;
  for(it=alldata.begin(); it!=alldata.end(); it++) {
    edm::Provenance const* prov = it->provenance();
    if(prov->productInstanceName() == "RPCb") {
      std::vector<L1MuRegionalCand> l1rpc = *(it->product());
      cout <<" numvber of muons: " << l1rpc.size() << endl;
      if (l1rpc.size() > 0) {
        L1MuRegionalCand & muon = l1rpc.front();
        cout <<" is Empty: " << muon.empty() << endl;
        if (muon.empty()) break;
        muon.setPhiValue( theTriggerScales->getPhiScale()->getLowEdge(muon.phi_packed()) );
        muon.setPtValue(  theTriggerScales->getPtScale()->getLowEdge(muon.pt_packed()) );
        muon.setEtaValue( theTriggerScales->getRegionalEtaScale(muon.type_idx())->getCenter(muon.eta_packed()) );
        cout <<" pT: " << muon.ptValue() << " coded: "<<muon.pt_packed() << endl;
        cout <<" eta: " << muon.etaValue() << " coded: "<<muon.eta_packed()<< endl;
        cout <<" phi: " << muon.phiValue() << " coded: "<<muon.phi_packed() <<endl;
        hEtaRPC->Fill(muon.etaValue());
        hPhiRPC->Fill(muon.phiValue());

//        float phi_rec = muon.phiValue()-getBending(muon.etaValue(), 10.); 
//        hPhiDiff->Fill(phi_rec-phi_gen);
//        hEtaDiff->Fill(eta_gen-muon.etaValue());

        float dx = cos(muon.phiValue());
        float dy = sin(muon.phiValue());
        float dz = sinh(muon.etaValue());
        GlobalVector dir(dx,dy,dz);
        GlobalPoint vtx(0.,0.,0.);

        cout <<"GlobalVector: phi:" << dir.phi()<<" eta:"<<dir.eta()<<endl;

      //  float pt_rec = getPt(phi_gen, muon.phiValue(), muon.etaValue());
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

          float phi_gen_reco = (p1-p0).phi();
          float pt_rec = getPt(phi_gen_reco, muon.phiValue(), muon.etaValue()); 
          hPhiDiff->Fill(deltaPhi(phi_gen,phi_gen_reco));
          hEtaDiff->Fill(eta_gen-eta_rec);
          hPt->Fill(pt_rec);
          if (pt_rec > 8.) numFiltered++;
        }
        hNumHP->Fill(3, float(numFiltered));
        

      }
    } 
  }
  if (region) delete region;
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

float L1Seeding::getPt(float phi0, float phiL1, float eta) const {

  float dphi_min = fabs(deltaPhi(phi0,phiL1));
  float pt_best = 1.;
  float pt_cur = 1;
  while ( pt_cur < 100.) {
    float phi_exp = phi0+getBending(eta, pt_cur);    
    float dphi = fabs(deltaPhi(phi_exp,phiL1)); 
    if ( dphi < dphi_min) {
      pt_best = pt_cur;
      dphi_min = dphi; 
    }
    pt_cur += 0.001;
  };
  return pt_best; 
}

float L1Seeding::getBending(float eta, float pt) const 
{
  float p1, p2;
  param(eta,p1,p2);
  return p1/pt + p2/(pt*pt) - 0.02; 
}

void L1Seeding::param(float eta, float &p1, float& p2) const
{
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

DEFINE_FWK_MODULE(L1Seeding);

