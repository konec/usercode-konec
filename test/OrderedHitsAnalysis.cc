#include "FWCore/Framework/interface/EDAnalyzer.h"
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
#include "R2DTimerObserver.h"
#include "TProfile.h"
#include "TH1D.h"

using namespace std;
using namespace ctfseeding;


class OrderedHitsAnalysis : public edm::EDAnalyzer {
public:
  explicit OrderedHitsAnalysis(const edm::ParameterSet& conf);
  ~OrderedHitsAnalysis();
  virtual void beginJob(const edm::EventSetup& iSetup);
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() { }
private:
  edm::ParameterSet theConfig;
  OrderedHitsGenerator * theGenerator;
  TrackingRegionProducer* theRegionProducer;
  int eventCount;
  Analysis * theAnalysis;
  SeedingLayerSets theLayers;

  TProfile *hNumHP, *hCPU;
  TH1D *hEtaDiff, *hEffReg_N, *hEffReg_D;
  TH1D *hCPUDist;
  TObjArray hList;
  
};


OrderedHitsAnalysis::OrderedHitsAnalysis(const edm::ParameterSet& conf) 
  : theConfig(conf), 
    theGenerator(0), 
   theRegionProducer(0),
    eventCount(0), 
   theAnalysis(0), 
   hList(0)
{
  edm::LogInfo("OrderedHitsAnalysis")<<" CTORXX";
}


OrderedHitsAnalysis::~OrderedHitsAnalysis() 
{ 
  delete theAnalysis;
  delete theRegionProducer;
  delete theGenerator;

  edm::LogInfo("OrderedHitsAnalysis")<<" DTOR";
}

void OrderedHitsAnalysis::beginJob(const edm::EventSetup& es)
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
  hCPU = new TProfile("hCPU","CPU time", Nsize,0.,float(Nsize),"S");
  hEffReg_N = new TH1D("hEffReg_N","hEffReg_N",Nsize,0.,float(Nsize));
  hEffReg_D = new TH1D("hEffReg_D","hEffReg_D",Nsize,0.,float(Nsize));
  hEtaDiff = new TH1D("hEtaDiff","hEtaDiff",100,-0.5,0.5);
  hCPUDist = new TH1D("hCPUDist","hCPUDist",60,0.,0.03);

  gHistos.Add(hNumHP);
  gHistos.Add(hCPU);
  gHistos.Add(hEffReg_N);
  gHistos.Add(hEffReg_D);
  gHistos.Add(hCPUDist);
  gHistos.Add(hEtaDiff);
}


void OrderedHitsAnalysis::analyze(
    const edm::Event& ev, const edm::EventSetup& es)
{
  cout <<"*** OrderedHitsAnalysisA, analyze event: " << ev.id()<<" event count:"<<++eventCount << endl;

  edm::ParameterSet assPset = theConfig.getParameter<edm::ParameterSet>("AssociatorPSet");
  TrackerHitAssociator assoc(ev, assPset);

/*
  //
  // load hits from disk into memory
  //
  typedef SeedingLayerSets::const_iterator ISLS;
  typedef SeedingLayers::const_iterator ISL;
  GlobalTrackingRegion r;
  for (ISLS isls =theLayers.begin(); isls != theLayers.end(); isls++) {
    for (ISL isl = isls->begin(); isl != isls->end(); isl++) r.hits(ev,es,&(*isl));
  }

*/

  // 
  // get regions from producer
  //
  typedef vector<TrackingRegion* > Regions;
  Regions regions = theRegionProducer->regions(ev,es);


  //
  // for time measurement
  //
  static R2DTimerObserver timer("**** MY TIMING REPORT ***");

  for (int iReg = 0, nRegions = regions.size(); iReg < nRegions; ++iReg) { 
    const TrackingRegion & region = *regions[iReg];

    //
    // run generator
    //
    timer.start();
    const OrderedSeedingHits & candidates = theGenerator->run(region,ev,es);
    timer.stop();

    //
    // fil histograms
    // 
    cout <<"Region_idx: "<<iReg<<", number of hit sets: " << candidates.size() <<endl;
    hCPU->Fill( float(iReg), timer.lastMeasurement().real());
    hNumHP->Fill(float(iReg), float(candidates.size()));
    hEffReg_D->Fill(float(iReg)); if(candidates.size() > 0)hEffReg_N->Fill(float(iReg));

//    if(iReg==2) {
//      unsigned int nSets = candidates.size();
//      for (unsigned int ic= 0; ic <nSets; ic++) {
//        typedef vector<ctfseeding::SeedingHit> RecHits;
//        const RecHits & hits = candidates[ic].hits();
//        float cotTheta = (hits[1].z()-hits[0].z())/(hits[1].r()-hits[0].r());
//        float eta = asinh(cotTheta);
//        float dEta = eta-region.direction().eta(); 
//        hEtaDiff->Fill(dEta);
//      }
//    }

    //
    // analysis for particular region
    //
    if (iReg== (int)regions.size()-1) { 
      
      theAnalysis->init(ev,es,&assoc);
//      theAnalysis->checkEfficiency(candidates);
      theAnalysis->checkAlgoEfficiency1(theLayers, candidates);
      hCPUDist->Fill(timer.lastMeasurement().real());
    }
  }

  for (Regions::const_iterator ir=regions.begin(); ir != regions.end(); ++ir) delete *ir;



}

DEFINE_FWK_MODULE(OrderedHitsAnalysis);

