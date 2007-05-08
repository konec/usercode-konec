#include "MyRegions.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertex.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"

#include "RecoTracker/TkTrackingRegions/interface/GlobalTrackingRegion.h"
#include "RecoTracker/TkTrackingRegions/interface/RectangularEtaPhiTrackingRegion.h"

#include "UserCode/konec/interface/Analysis.h"

using namespace edm;
using namespace std;

MyRegions::MyRegions(const edm::ParameterSet& cfg)
  : theRegionPSet(cfg.getParameter<edm::ParameterSet>("RegionPSet")),
    theSizes(cfg.getParameter<std::vector<double> >("deltaR")),
    theVertexFromParticle(cfg.getParameter<bool>("useParticleVertex")),
    theParticleId(cfg.getParameter<int>("useParticleId")),
    thePtMinLeadingTrack(cfg.getParameter<double>("ptMinLeadingTrack"))
{}

vector<TrackingRegion* > MyRegions::regions( const Event& ev, const EventSetup& es) const
{
  vector<TrackingRegion* > result;

  Handle<SimTrackContainer> simTk;
  ev.getByLabel("g4SimHits",simTk);
  vector<SimTrack> simTracks = *(simTk.product());

  Handle<SimVertexContainer> simVc;
  ev.getByLabel("g4SimHits", simVc);
  vector<SimVertex> simVtcs = *(simVc.product());
  int nVtx = simVtcs.size();
  cout <<" number of vertices: " << nVtx <<endl;
  if (nVtx>0)  cout <<" first vertex: "<< simVtcs[0].position().perp() <<endl;

  const SimTrack * myTrack = 0;
  float ptMin = thePtMinLeadingTrack;
  cout <<" MyRegions: "<<simTracks.size()<<" SimTracks found"<<endl;
  typedef  SimTrackContainer::const_iterator IP;
  for (IP ip=simTracks.begin(); ip != simTracks.end(); ip++) {

    const SimTrack & track = (*ip);

    if ( track.noVertex() ) continue;
    if ( track.type() == -99) continue;

    float eta_gen = track.momentum().eta();
    if ( fabs(eta_gen) > 2.1 ) continue; 

    float pt_gen = track.momentum().perp();
    if (simVtcs[track.vertIndex()].position().perp() > 0.1) continue;
//    if (pt_gen >5.) {
//      Analysis::print(track);
//      cout <<"vertex: "<<simVtcs[track.vertIndex()].position()<<endl;
//    }
    if ( abs(track.type()) != theParticleId ) continue;
    if (pt_gen < ptMin) continue;

    myTrack = &track;
    ptMin = pt_gen;
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
    float z_vtx = simVtcs[myTrack->vertIndex()].position().z();
    if (theVertexFromParticle) vtx = GlobalPoint(0.,0.,z_vtx);
  } else {
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
