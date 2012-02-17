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
  virtual void beginJob();
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() { }
private:
  edm::ParameterSet theConfig;
  int eventCount;
  Analysis * theAnalysis;
  TH1D *hPhi, *hNum, *hPt; 
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

void TrackAnalysis::beginJob()
{
  edm::ParameterSet apset = theConfig.getParameter<edm::ParameterSet>("AnalysisPSet");
  theAnalysis = new Analysis(apset);
  hPhi = new  TH1D("hPhi","hPhi",100, -3.2,3.2);
  hPt = new TH1D("hPt","hPt",100,0.,5.);
  hNum = new TH1D("hNum","hNum",40,0.,40.);

  gHistos.Add(hPhi);
  gHistos.Add(hPt);
  gHistos.Add(hNum);
}


void TrackAnalysis::analyze(
    const edm::Event& ev, const edm::EventSetup& es)
{
  cout <<"*** TrackAnalysisA, analyze event: " << ev.id()<<" event count:"<<++eventCount << endl;

  edm::ParameterSet assPset = theConfig.getParameter<edm::ParameterSet>("AssociatorPSet");
  theAnalysis->init(ev,es,0);
  const SimTrack* myTrack = theAnalysis->bestTrack();
  
  if(myTrack)  Analysis::print(*myTrack);
  else { std::cout <<" NO MY TRACK!"<<std::endl; return; }

  const SimVertex * simVertex = theAnalysis->vertex(myTrack);
  Analysis::print(*simVertex);
  math::XYZPoint bs(simVertex->position().x(),
                simVertex->position().y(),
                simVertex->position().z());

  
//  TrackerHitAssociator assoc(ev, assPset);

  edm::Handle<reco::TrackCollection> trackCollection;
  std::string collectionLabel = theConfig.getParameter<std::string>("TrackCollection");
  ev.getByLabel(collectionLabel,trackCollection);
  reco::TrackCollection tracks = *(trackCollection.product());
  cout <<"#RECONSTRUCTED tracks: " << tracks.size() << endl;
  hNum->Fill(tracks.size());

  typedef reco::TrackCollection::const_iterator IT;
  for (IT it = tracks.begin(); it !=tracks.end(); ++it) {
    const reco::Track & track = *it;
    Analysis::print(track,bs);
    hPt->Fill(track.pt());
    hPhi->Fill(track.momentum().phi());
  }


//  theAnalysis->init(ev,es,&assoc);
//  theAnalysis->checkEfficiency(tracks);


}

DEFINE_FWK_MODULE(TrackAnalysis);

