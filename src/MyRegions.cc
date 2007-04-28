#include "MyRegions.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "RecoTracker/TkTrackingRegions/interface/GlobalTrackingRegion.h"
#include "RecoTracker/TkTrackingRegions/interface/RectangularEtaPhiTrackingRegion.h"

using namespace edm;
using namespace std;

MyRegions::MyRegions(const edm::ParameterSet& cfg)
  : theRegionPSet(cfg.getParameter<edm::ParameterSet>("RegionPSet")),
    theSizes(cfg.getParameter<std::vector<double> >("deltaR"))
{}

vector<TrackingRegion* > MyRegions::regions( const Event& ev, const EventSetup& es) const
{
  vector<TrackingRegion* > result;
  Handle<SimTrackContainer> simTk;
  ev.getByLabel("g4SimHits",simTk);
  vector<SimTrack> simTracks = *(simTk.product());
  const SimTrack * myTrack = 0;
  float ptMax = 0.2;
  typedef  SimTrackContainer::const_iterator IP;
  for (IP ip=simTracks.begin(); ip != simTracks.end(); ip++) {

    const SimTrack & track = (*ip);

    if ( track.noVertex() ) continue;
    if ( track.type() == -99) continue;
    if ( track.vertIndex() != 0) continue;
    if ( abs(track.type()) != 13 ) continue;

    float eta_gen = track.momentum().eta();
    if ( fabs(eta_gen) > 2.1 ) continue; 

    float pt_gen = track.momentum().perp();
    if (pt_gen < ptMax) continue;

    myTrack = &track;
  }

  GlobalVector dir;
  GlobalPoint vtx(0.,0.,0.);
  if (myTrack) {
    cout << " BEST TRACK: "
         <<" pt="<<myTrack->momentum().perp()
         <<" eta="<< myTrack->momentum().eta() << endl;

    dir = GlobalVector(myTrack->momentum().x(),
                                    myTrack->momentum().y(),
                                    myTrack->momentum().z()).unit();
  //  vtx =(*simVtcs)[p->vertIndex()].position();
  } else {
    dir = GlobalVector(1.,1.,0.);
    return result; 
  }

  float ptmin =        theRegionPSet.getParameter<double>("ptMin");
  float dr =           theRegionPSet.getParameter<double>("originRadius");
  float dz =           theRegionPSet.getParameter<double>("originHalfLength");
  bool precise =       theRegionPSet.getParameter<bool>("precise");
  for (vector<double>::const_iterator ireg = theSizes.begin(); ireg < theSizes.end(); ireg++) {
    double deltaR = (*ireg);
    TrackingRegion * region = 0;
    if (deltaR > 0) {
      region = new RectangularEtaPhiTrackingRegion( dir, vtx, ptmin,  dr, dz, deltaR, deltaR);
    }
    else {
      region =  new GlobalTrackingRegion(ptmin,  dr, dz, 0., precise);
    }
    result.push_back(region);
  }
//  }
//  else {
//    std::cout <<"no region!"<<std::endl;
//  }
  return result;
}
