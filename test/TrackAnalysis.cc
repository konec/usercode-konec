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

using namespace std;
using namespace ctfseeding;


class TrackAnalysis : public edm::EDAnalyzer {
public:
  explicit TrackAnalysis(const edm::ParameterSet& conf);
  ~TrackAnalysis();
  virtual void beginJob(const edm::EventSetup& iSetup);
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() { }
private:
  edm::ParameterSet theConfig;
  int eventCount;
  Analysis * theAnalysis;

};


TrackAnalysis::TrackAnalysis(const edm::ParameterSet& conf) 
  : theConfig(conf), eventCount(0), 
   theAnalysis(0) 
{
  edm::LogInfo("TrackAnalysis")<<" CTORXX";
}


TrackAnalysis::~TrackAnalysis() 
{ 
  delete theAnalysis;

  edm::LogInfo("TrackAnalysis")<<" DTOR";
}

void TrackAnalysis::beginJob(const edm::EventSetup& es)
{
  edm::ParameterSet apset = theConfig.getParameter<edm::ParameterSet>("AnalysisPSet");
  theAnalysis = new Analysis(apset);
}


void TrackAnalysis::analyze(
    const edm::Event& ev, const edm::EventSetup& es)
{
  cout <<"*** TrackAnalysisA, analyze event: " << ev.id()<<" event count:"<<++eventCount << endl;

  edm::ParameterSet assPset = theConfig.getParameter<edm::ParameterSet>("AssociatorPSet");
//  TrackerHitAssociator assoc(ev, assPset);

  edm::Handle<reco::TrackCollection> trackCollection;
  std::string collectionLabel = theConfig.getParameter<std::string>("TrackCollection");
  ev.getByLabel(collectionLabel,trackCollection);
  reco::TrackCollection tracks = *(trackCollection.product());
  cout <<" number of tracks: " << tracks.size() << endl;
  typedef reco::TrackCollection::const_iterator IT;
  for (IT it = tracks.begin(); it !=tracks.end(); ++it) Analysis::print(*it);


//  theAnalysis->init(ev,es,&assoc);
//  theAnalysis->checkEfficiency(tracks);


}

DEFINE_FWK_MODULE(TrackAnalysis);

