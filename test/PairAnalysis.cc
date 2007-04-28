#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"

//add simhit info
#include "SimTracker/TrackerHitAssociation/interface/TrackerHitAssociator.h"

//simtrack
#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"

#include "RecoTracker/TkTrackingRegions/interface/TrackingRegionProducerFactory.h"
#include "RecoTracker/TkTrackingRegions/interface/TrackingRegionProducer.h"
#include "RecoTracker/TkTrackingRegions/interface/TrackingRegion.h"

#include "RecoTracker/TkTrackingRegions/interface/OrderedHitsGeneratorFactory.h"
#include "RecoTracker/TkTrackingRegions/interface/OrderedHitsGenerator.h"
#include "RecoTracker/TkTrackingRegions/interface/GlobalTrackingRegion.h"

#include "RecoTracker/TkSeedingLayers/interface/SeedingLayerSetsBuilder.h"
#include "RecoTracker/TkSeedingLayers/interface/SeedingLayerSets.h"

#include "UserCode/konec/interface/Analysis.h"
#include "TProfile.h"

using namespace std;
using namespace ctfseeding;


class PairAnalysis : public edm::EDAnalyzer {
public:
  explicit PairAnalysis(const edm::ParameterSet& conf);
  ~PairAnalysis();
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
  
};


PairAnalysis::PairAnalysis(const edm::ParameterSet& conf) 
  : theConfig(conf), theGenerator(0), eventCount(0), theAnalysis(0)
{
  edm::LogInfo("PairAnalysis")<<" CTORXX";
}


PairAnalysis::~PairAnalysis() 
{ 
  delete theAnalysis;
  delete theRegionProducer;
  delete theGenerator;

  edm::LogInfo("PairAnalysis")<<" DTOR";
}

void PairAnalysis::beginJob(const edm::EventSetup& es)
{

  edm::ParameterSet regfactoryPSet =
      theConfig.getParameter<edm::ParameterSet>("RegionFactoryPSet");
  std::string regfactoryName = regfactoryPSet.getParameter<std::string>("ComponentName");
  theRegionProducer = TrackingRegionProducerFactory::get()->create(regfactoryName,regfactoryPSet);

  edm::ParameterSet orderedPSet =
      theConfig.getParameter<edm::ParameterSet>("OrderedHitsFactoryPSet");
  std::string orderedName = orderedPSet.getParameter<std::string>("ComponentName");
  theGenerator = OrderedHitsGeneratorFactory::get()->create( orderedName, orderedPSet);


  edm::ParameterSet leyerPSet = orderedPSet.getParameter<edm::ParameterSet>("LayerPSet");
  theLayers = SeedingLayerSetsBuilder(leyerPSet).layers(es);
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

}


void PairAnalysis::analyze(
    const edm::Event& ev, const edm::EventSetup& es)
{
  cout <<"*** PairAnalysisX, analyze event: " << ev.id()<<" event count:"<<++eventCount << endl;

  theAnalysis->init(ev,es);

  typedef vector<TrackingRegion* > Regions;
  Regions regions = theRegionProducer->regions(ev,es);

  for (int iReg = 0, nRegions = regions.size(); iReg < nRegions; ++iReg) { 
    const TrackingRegion & region = *regions[iReg];

    const OrderedSeedingHits & candidates = theGenerator->run(region,ev,es);
    cout <<"Region_idx: "<<iReg<<", number of seeds: " << candidates.size() << endl;
    hNumHP->Fill(float(iReg), float(candidates.size()));

    if (iReg==3) { 
      theAnalysis->init(ev,es);
      theAnalysis->checkEfficiency(candidates);
      theAnalysis->checkAlgoEfficiency(theLayers, candidates);
    }
  }

  for (Regions::const_iterator ir=regions.begin(); ir != regions.end(); ++ir) delete *ir;

}

#include "PluginManager/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(PairAnalysis);

