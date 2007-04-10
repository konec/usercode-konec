#include "UserCode/konec/interface/Analysis.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "SimTracker/TrackerHitAssociation/interface/TrackerHitAssociator.h"

//simtrack
#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "RecoTracker/TkHitPairs/interface/LayerWithHits.h"

#include "DataFormats/Math/interface/Vector3D.h"
#include "DataFormats/Math/interface/Point3D.h"
#include "DataFormats/Math/interface/Error.h"

#include "RecoTracker/TkTrackingRegions/interface/RectangularEtaPhiTrackingRegion.h"
#include "RecoTracker/TkTrackingRegions/interface/GlobalTrackingRegion.h"

#include "RecoTracker/TkSeedingLayers/interface/SeedingLayerSets.h"
#include "RecoTracker/TkSeedingLayers/interface/SeedingHitSet.h"


#include "RecoTracker/TkSeedingLayers/interface/OrderedSeedingHits.h"

#include "TFile.h"
#include "TH1D.h"
#include "TProfile.h"


#include <iostream>
using namespace std;
using namespace edm;
using namespace ctfseeding;

template <class T> T sqr( T t) {return t*t;}

//----------------------------------------------------------------------------------------
Analysis::Analysis( const edm::ParameterSet& conf)
  : theConfig(conf), theAssociator(0)
{ 
  rootFile = new TFile("analysis.root","RECREATE");
  hEffEta_N = new TH1D("hEffEta_N","hEffEta_N",26,-2.6,2.6);
  hEffEta_D = new TH1D("hEffEta_D","hEffEta_D",26,-2.6,2.6);
  hEffAlgoEta_N = new TH1D("hEffAlgoEta_N","hEffAlgoEta_N",26,-2.6,2.6);
  hEffAlgoEta_D = new TH1D("hEffAlgoEta_D","hEffAlgoEta_D",26,-2.6,2.6);
  hEffPhi_N = new TH1D("hEffPhi_N","hEffPhi_N",63,-3.15,3.15);
  hEffPhi_D = new TH1D("hEffPhi_D","hEffPhi_D",63,-3.15,3.15);
  hEffPt_N = new TH1D("hEffPt_N","hEffPt_N",101,0.0,10.1);
  hEffPt_D = new TH1D("hEffPt_D","hEffPt_D",101,0.0,10.1);
  hEffAlgoPt_D = new TH1D("hEffAlgoPt_D","hEffAlgoPt_D",101,0.0,10.1);
  hEffAlgoPt_N = new TH1D("hEffAlgoPt_N","hEffAlgoPt_N",101,0.0,10.1);
  hPurePt_N =  new TH1D("hPurePt_N","hPurePt_N",101,0.0,10.1);
  hPurePt_D =  new TH1D("hPurePt_D","hPurePt_D",101,0.0,10.1);

}

//----------------------------------------------------------------------------------------
Analysis::~Analysis()
{ 
  cout <<"WRITING ROOT FILE"<< std::endl;
    rootFile->Write();
  cout <<"rootFile WRITTEN, Analysis DTOR"<<endl; 
}

//----------------------------------------------------------------------------------------
void Analysis::init(const edm::Event& ev, const edm::EventSetup& es) 
{
  theEvent = &ev;
  theSetup = &es;
  theSimTracks.clear();
  Handle<SimTrackContainer> simTk;
  ev.getByLabel("g4SimHits",simTk);
  theSimTracks = *(simTk.product());
  cout <<" Analysis has: " << theSimTracks.size()<<" tracks"<<endl;

  if (theAssociator) delete theAssociator;
  edm::ParameterSet asset = theConfig.getParameter<edm::ParameterSet>("AssociatorPSet");
  theAssociator = new TrackerHitAssociator(ev,asset);
}

//----------------------------------------------------------------------------------------
void Analysis::checkAlgoEfficiency(const SeedingLayerSets &layersSets, const OrderedSeedingHits& candidates)
{
  for (SeedingLayerSets::const_iterator ils = layersSets.begin(); ils != layersSets.end(); ils++) {
  const vector<SeedingLayer> & layers = (*ils);
  int nLayers = layers.size();
  typedef  SimTrackContainer::const_iterator IP;
  for (IP ip=theSimTracks.begin(); ip != theSimTracks.end(); ip++) {
    const SimTrack & track = (*ip);
    bool selected = select(track);
    if (!selected) continue;
    float eta_gen = track.momentum().eta();
    float pt_gen = track.momentum().perp();
    int nHitsInLayes = 0;
    typedef vector<SeedingLayer>::const_iterator IL;
    for (IL il = layers.begin(); il != layers.end(); il++) {
      vector<SeedingHit> hits = (*il).hits(*theEvent,*theSetup);
      int nmatched = matchedHits(track.trackId(), hits);
      if (nmatched > 0) nHitsInLayes++; 
    }
    if (nHitsInLayes!=nLayers) continue;

    bool matched = false;
    unsigned int nSets = candidates.size();
    for (unsigned int ic= 0; ic <nSets; ic++) {
      typedef vector<ctfseeding::SeedingHit> RecHits;
      const RecHits & hits = candidates[ic].hits();
      unsigned int nmatched = matchedHits(track.trackId(), hits);
      if (nmatched == hits.size() ) matched = true;
    }

    if (fabs(eta_gen) < 2.1) {
      hEffAlgoPt_D->Fill(pt_gen);
      if(matched) hEffAlgoPt_N->Fill(pt_gen);
    }
    if (pt_gen > 3.0) {
      hEffAlgoEta_D->Fill(eta_gen);
      if(matched) hEffAlgoEta_N->Fill(eta_gen);
    }
  }
  }
}

//----------------------------------------------------------------------------------------
void Analysis::checkEfficiency( const OrderedSeedingHits & candidates)
{
  typedef  SimTrackContainer::const_iterator IP;

  typedef vector<ctfseeding::SeedingHit> RecHits;

  for (IP ip=theSimTracks.begin(); ip != theSimTracks.end(); ip++) {
    const SimTrack & track = (*ip); 
    bool selected = select(track);
    if (!selected) continue;
    float eta_gen = track.momentum().eta();
    float pt_gen = track.momentum().perp();
//    print(track);

    bool matched = false;
    unsigned int nSets = candidates.size();
    for (unsigned int ic= 0; ic <nSets; ic++) {
      const RecHits & hits = candidates[ic].hits();
      unsigned int nmatched = matchedHits(track.trackId(), hits); 
      if (nmatched == hits.size() ) matched = true;
    }

    if (fabs(eta_gen) < 2.1) {
      hEffPt_D->Fill(pt_gen);
      if(matched) hEffPt_N->Fill(pt_gen);
    }
    if (pt_gen > 3.0) {
      hEffEta_D->Fill(eta_gen);
      if(matched) hEffEta_N->Fill(eta_gen);
    }
    
  }
}


//----------------------------------------------------------------------------------------
unsigned int Analysis::matchedHits(unsigned int trackId, const SeedingHitSet& hitset) 
{
  unsigned int nmatched = 0;
  typedef vector<ctfseeding::SeedingHit> RecHits;
  const RecHits & hits = hitset.hits(); 
  for (RecHits::const_iterator it = hits.begin(); it != hits.end(); it++) {
    const TrackingRecHit & hit = *(it->RecHit());
    //typedef vector<unsigned int> SimTrackIds;
    typedef vector<SimHitIdpr> SimTrackIds;
    SimTrackIds simTrackIds = theAssociator->associateHitId(hit);
    bool ok = false;
    for (SimTrackIds::const_iterator ist = simTrackIds.begin(); ist != simTrackIds.end(); ist++) {
      if ( (*ist).first == trackId) ok = true;
    }
    if (ok) nmatched++;
  }
  return nmatched;
}
//----------------------------------------------------------------------------------------

bool Analysis::select(const SimTrack & track) const
{
  if ( track.noVertex() ) return false;
  if ( track.type() == -99) return false;
  if ( track.vertIndex() != 0) return false;
  if ( abs(track.type()) != 13 ) return false;

//  float eta_gen = track.momentum().eta();
//  if ( fabs(eta_gen) > 2.1 ) return false;
  return true;
}

//----------------------------------------------------------------------------------------
void Analysis::print(const SimTrack & track) const
{
  if ( track.type() == -99) return;
  float phi_gen = track.momentum().phi();
  float pt_gen = track.momentum().perp();
  float eta_gen = track.momentum().eta();
//    HepLorentzVector vtx =(*simVtcs)[p->vertIndex()].position();
//    float z_gen  = vtx.z();
   cout <<" trackId: " <<track.trackId()
          << " pt_gen: " << pt_gen <<" eta_gen: "<<eta_gen<<" phi_gen: "<<phi_gen 
          <<" vtx: "<<track.vertIndex()<<" type: "<<track.type()
          << endl;

}
