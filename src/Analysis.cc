#include "UserCode/konec/interface/Analysis.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "SimTracker/TrackerHitAssociation/interface/TrackerHitAssociator.h"

//simtrack
#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertex.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"

#include "RecoTracker/TkTrackingRegions/interface/RectangularEtaPhiTrackingRegion.h"
#include "RecoTracker/TkTrackingRegions/interface/GlobalTrackingRegion.h"

#include "RecoTracker/TkSeedingLayers/interface/OrderedSeedingHits.h"
#include "RecoTracker/TkSeedingLayers/interface/SeedingLayerSets.h"
#include "RecoTracker/TkSeedingLayers/interface/SeedingHitSet.h"
#include "AnalysisOrderedHitsInLayers.h"

#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"

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
  hEffPt_N = new TH1D("hEffPt_N","hEffPt_N",1001,0.0,100.1);
  hEffPt_D = new TH1D("hEffPt_D","hEffPt_D",1001,0.0,100.1);
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
const SimTrack * Analysis::bestTrack() const
{

  const SimTrack * myTrack = 0;

  float ptMin = theConfig.getParameter<double>("ptMinLeadingTrack");
  int particleId = theConfig.getParameter<int>("useParticleId");

//  cout <<" Analysis: "<<simTracks.size()<<" SimTracks found"<<endl;
  typedef  SimTrackContainer::const_iterator IP;
  for (IP ip=theSimTracks.begin(); ip != theSimTracks.end(); ip++) {

    const SimTrack & track = (*ip);

    if ( track.noVertex() ) continue;
    if ( track.type() == -99) continue;
    if ( abs(track.type()) != particleId) continue;

    float eta_gen = track.momentum().eta();
    if ( fabs(eta_gen) > 2.1 ) continue;

    float pt_gen = track.momentum().perp();
    if (pt_gen < ptMin) continue;

    if (vertex(&track)->position().perp() > 0.1) continue;

    myTrack = &track;
    ptMin = pt_gen;
  }

  return myTrack;
}

//----------------------------------------------------------------------------------------
const SimVertex * Analysis::vertex(const SimTrack * track) const
{
  if (!track) return 0;
  const SimVertex & vtx = theSimVertices[track->vertIndex()];
  return &vtx;
}


//----------------------------------------------------------------------------------------
void Analysis::init(const edm::Event& ev, const edm::EventSetup& es, TrackerHitAssociator * ass) 
{
  theEvent = &ev;
  theSetup = &es;
  theSimTracks.clear();
  Handle<SimTrackContainer> simTk;
  ev.getByLabel("g4SimHits",simTk);
  theSimTracks = *(simTk.product());
  cout <<" Analysis has: " << theSimTracks.size()<<" tracks"<<endl;

  Handle<SimVertexContainer> simVc;
  ev.getByLabel("g4SimHits", simVc);
  theSimVertices = *(simVc.product());
  cout <<" Analysis has: " << theSimVertices.size()<<" vertices"<<endl;

  theAssociator = ass;
//  if (theAssociator) delete theAssociator;
}

//----------------------------------------------------------------------------------------
void Analysis::checkAlgoEfficiency1(const SeedingLayerSets &layersSets, const OrderedSeedingHits& candidates)
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

//-----------------------------------------------------------------------------------------
void Analysis::checkAlgoEfficiency2(const SeedingLayerSets &layersSets, const OrderedSeedingHits& candidates)
{
  typedef  SimTrackContainer::const_iterator IP;
  for (IP ip=theSimTracks.begin(); ip != theSimTracks.end(); ip++) {

    const SimTrack & track = (*ip);
    bool selected = select(track);
    if (!selected) continue;
    float eta_gen = track.momentum().eta();
    float pt_gen = track.momentum().perp();

    for (SeedingLayerSets::const_iterator ils=layersSets.begin(); ils!=layersSets.end(); ils++) {
      const SeedingLayers & layers = (*ils);
      AnalysisOrderedHitsInLayers hitsInLayers(layers, *theEvent, *theSetup);

//      if (!hitsInLayers.onlySingleHits()) continue;

      vector<SeedingHitSet> layerHitSets;
      while ( hitsInLayers.next() ) { 

        SeedingHitSet layerHitSet = hitsInLayers.getHitSet();
        unsigned int  nmatched = matchedHits(track.trackId(), layerHitSet.hits());
        if (nmatched == layerHitSet.size() ) layerHitSets.push_back(layerHitSet);
//        cout <<"matched hits: " << nmatched << endl;
//        cout <<" Layer Hits : " <<endl;
//        for (int i=0; i<layerHitSet.size();i++) cout << print(layerHitSet.hits()[i])<<endl;
      }

      bool matched = false;
      if (layerHitSets.size() < 1) continue;
      if (layerHitSets.size() > 1) continue;

      for (vector<SeedingHitSet>::const_iterator it = layerHitSets.begin();
           it != layerHitSets.end(); it++) {
        const SeedingHitSet & layerHitSet = *it;
        unsigned int nSets = candidates.size();
        for  (unsigned int ic= 0; ic <nSets; ic++) {
           SeedingHitSet candidHits =  candidates[ic];
           if ( compareHitSets(layerHitSet, candidates[ic]) ) matched = true;
         }
      }

      // pt efficiency
      if (fabs(eta_gen) < 2.1) {
        hEffAlgoPt_D->Fill(pt_gen);
        if(matched) hEffAlgoPt_N->Fill(pt_gen);
      }
      // eta efficiency
      if (pt_gen > 3.0) {
        hEffAlgoEta_D->Fill(eta_gen);
        if(matched) hEffAlgoEta_N->Fill(eta_gen);
      }
    }
  }
}

//----------------------------------------------------------------------------------------
void Analysis::checkEfficiency( const reco::TrackCollection & tracks)
{
  typedef  SimTrackContainer::const_iterator IP;

  typedef vector<ctfseeding::SeedingHit> RecHits;

  for (IP ip=theSimTracks.begin(); ip != theSimTracks.end(); ip++) {
    const SimTrack & track = (*ip); 
    bool selected = select(track);
    if (!selected) continue;
    float eta_gen = track.momentum().eta();
    float pt_gen = track.momentum().perp();

    bool matched = false;
    typedef reco::TrackCollection::const_iterator IT;
    for (IT it=tracks.begin(); it!=tracks.end(); it++) {
//      float pt_rec = (*it).pt();
      float eta_rec = (*it).momentum().eta();
      if ( fabs(eta_gen-eta_rec) < 0.05) matched = true;
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
    const TrackingRecHit * hit = *(it);
    typedef vector<SimHitIdpr> SimTrackIds;
    SimTrackIds simTrackIds = theAssociator->associateHitId(*hit);
    bool ok = false;
    for (SimTrackIds::const_iterator ist = simTrackIds.begin(); ist != simTrackIds.end(); ist++) {
      if ( (*ist).first == trackId) ok = true;
    }
    if (ok) nmatched++;
  }
  return nmatched;
}

//----------------------------------------------------------------------------------------
bool Analysis::compareHitSets(const SeedingHitSet& hits1, const SeedingHitSet& hits2) const
{
  unsigned int size = hits1.size();
  if (size != hits2.size() ) return false;
  for (unsigned int i = 0; i < size; ++i) {
    const TrackingRecHit* trh1 = hits1[i];
    const TrackingRecHit* trh2 = hits2[i];
    if (trh1 != trh2) return false;
  }
  return true; 
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
void Analysis::print(const SimTrack & track) 
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
//----------------------------------------------------------------------------------------
std::string Analysis::print(const SeedingHit & hit) 
{
  ostringstream str; 
  str <<"r="<<hit.r() <<" phi="<<hit.phi()<<" z="<<hit.z();
  return str.str();
}

