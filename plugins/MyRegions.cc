#include "MyRegions.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/Common/interface/Handle.h"
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

template <class T> T sqr( T t) {return t*t;}

MyRegions::MyRegions(const edm::ParameterSet& cfg)
  : theRegionPSet(cfg.getParameter<edm::ParameterSet>("RegionPSet")),
    theSizes(cfg.getParameter<std::vector<double> >("deltaR")),
    theVertexFromParticle(cfg.getParameter<bool>("useParticleVertex")),
    theParticleId(cfg.getParameter<int>("useParticleId")),
    thePtMinLeadingTrack(cfg.getParameter<double>("ptMinLeadingTrack"))
{}

vector<TrackingRegion* > MyRegions::regions( const Event& ev, const EventSetup& es) const
{
  cout <<"** My regions ***************************************"<<endl;
  vector<TrackingRegion* > result;

  Handle<SimTrackContainer> simTk;
  ev.getByLabel("g4SimHits",simTk);
  vector<SimTrack> simTracks = *(simTk.product());

  Handle<SimVertexContainer> simVc;
  ev.getByLabel("g4SimHits", simVc);
  vector<SimVertex> simVtcs = *(simVc.product());
  int nVtx = simVtcs.size();
  cout <<" number of vertices: " << nVtx <<endl;
  if (nVtx>0)  cout <<" first vertex: "<< simVtcs[0].position() <<endl;

  const SimTrack * myTrack = 0;

  float ptMin = thePtMinLeadingTrack;
  cout <<" MyRegions: "<<simTracks.size()<<" SimTracks found"<<endl;
  typedef  SimTrackContainer::const_iterator IP;

  double x_vtx = theRegionPSet.getParameter<double>("originXPos");
  double y_vtx = theRegionPSet.getParameter<double>("originYPos");
  double z_vtx = theRegionPSet.getParameter<double>("originZPos");

  double rectOption = -1.;
  if (theRegionPSet.exists("RectOption")) 
      rectOption=theRegionPSet.getParameter<double>("RectOption"); 

  for (IP ip=simTracks.begin(); ip != simTracks.end(); ip++) {

    const SimTrack & track = (*ip);

    if ( track.noVertex() ) continue;
    if ( track.type() == -99) continue;
    if ( abs(track.type()) != theParticleId ) continue;

    float eta_gen = track.momentum().eta();
    if ( fabs(eta_gen) > 2.5 ) continue; 

    float pt_gen = track.momentum().pt();
    if (pt_gen < ptMin) continue;

    float tr_x = simVtcs[track.vertIndex()].position().x();
    float tr_y = simVtcs[track.vertIndex()].position().y();
    if ( sqrt(sqr(tr_x-x_vtx)+sqr(tr_y-y_vtx)) > 1.0) continue;

    myTrack = &track;
    ptMin = pt_gen;
  }

  if (!myTrack) return result;
  GlobalVector dir = GlobalVector( myTrack->momentum().x(),
                                   myTrack->momentum().y(),
                                   myTrack->momentum().z() ).unit();

  cout << " BEST TRACK: "
       <<" type="<<myTrack->type()
       <<" pt="<<myTrack->momentum().pt()
       <<" eta="<< myTrack->momentum().eta() << endl;



  GlobalPoint vtx = (theVertexFromParticle) ?
  GlobalPoint(simVtcs[myTrack->vertIndex()].position().x(),
              simVtcs[myTrack->vertIndex()].position().y(),
              simVtcs[myTrack->vertIndex()].position().z())
    : GlobalPoint (x_vtx,y_vtx,z_vtx);

//  vtx = GlobalPoint(0.1*cos(M_PI/4), 0.1*sin(M_PI/4), vtx.z());
//  dir = GlobalVector(sin(M_PI/4), -cos(M_PI/4), 0.0).unit();

  int charge = (myTrack->type()==13) ? -1 : 1;
  double ip = -charge*(vtx.x()*dir.y()-vtx.y()*dir.x())/sqrt(sqr(dir.x()) + sqr(dir.y()));
  cout <<"** Computed IP="<<ip<<" vtx_vy="<<vtx.perp()<<endl;

  float ptmin = (theVertexFromParticle)? ptMin : theRegionPSet.getParameter<double>("ptMin");
  float dr =           theRegionPSet.getParameter<double>("originRadius");
  float dz =           theRegionPSet.getParameter<double>("originHalfLength");
  bool precise =       theRegionPSet.getParameter<bool>("precise");

  for (vector<double>::const_iterator ireg = theSizes.begin(); ireg < theSizes.end(); ireg++) {
    double deltaR = (*ireg);
    TrackingRegion * region = 0;
    if (deltaR > 0) {
      region = new RectangularEtaPhiTrackingRegion(dir,vtx,ptmin,dr,dz,deltaR,deltaR,rectOption);
    }
    else {
      GlobalTrackingRegion * g = new GlobalTrackingRegion(ptmin,  vtx, dr, dz, precise );
      g->setDirection(dir);
      region = g;
//      region =  new GlobalTrackingRegion(ptmin,  vtx, dr, dz, precise );
    }
    cout << "Region: " << region->print() << endl;
    result.push_back(region);
  }

  return result;
}
