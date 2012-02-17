#include "UserCode/konec/interface/Analysis.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "SimTracker/TrackerHitAssociation/interface/TrackerHitAssociator.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"

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
// #include "AnalysisOrderedHitsInLayers.h"

#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"

#include "TFile.h"
#include "TH1D.h"
#include "TProfile.h"


#include <iostream>
using namespace std;
using namespace edm;
using namespace reco;
using namespace ctfseeding;

template <class T> T sqr( T t) {return t*t;}
typedef TransientTrackingRecHit::ConstRecHitPointer Hit;

//----------------------------------------------------------------------------------------
Analysis::Analysis( const edm::ParameterSet& conf)
  : theConfig(conf), theAssociator(0)
{ 
  hEffEta_N = new TH1D("hEffEta_N","hEffEta_N",26,-2.6,2.6);
  hEffEta_D = new TH1D("hEffEta_D","hEffEta_D",26,-2.6,2.6);
  hEffAlgoEta_N = new TH1D("hEffAlgoEta_N","hEffAlgoEta_N",26,-2.6,2.6);
  hEffAlgoEta_D = new TH1D("hEffAlgoEta_D","hEffAlgoEta_D",26,-2.6,2.6);
  hEffPhi_N = new TH1D("hEffPhi_N","hEffPhi_N",63,-3.15,3.15);
  hEffPhi_D = new TH1D("hEffPhi_D","hEffPhi_D",63,-3.15,3.15);
  hEffPt_N = new TH1D("hEffPt_N","hEffPt_N",1001,0.0,100.1);
  hEffPt_D = new TH1D("hEffPt_D","hEffPt_D",1001,0.0,100.1);
  hEffLPt_N = new TH1D("hEffLPt_N","hEffLPt_N",201,0.0,2.01);
  hEffLPt_D = new TH1D("hEffLPt_D","hEffLPt_D",201,0.0,2.01);
  hEffAlgoPt_D = new TH1D("hEffAlgoPt_D","hEffAlgoPt_D",101,0.0,10.1);
  hEffAlgoPt_N = new TH1D("hEffAlgoPt_N","hEffAlgoPt_N",101,0.0,10.1);
  hPurePt_N =  new TH1D("hPurePt_N","hPurePt_N",101,0.0,10.1);
  hPurePt_D =  new TH1D("hPurePt_D","hPurePt_D",101,0.0,10.1);

  gHistos.SetOwner();

  gHistos.Add(hEffEta_N);
  gHistos.Add(hEffEta_D);
  gHistos.Add(hEffAlgoEta_N);
  gHistos.Add(hEffAlgoEta_D);
  gHistos.Add(hEffPhi_N);
  gHistos.Add(hEffPhi_D);
  gHistos.Add(hEffPt_N);
  gHistos.Add(hEffPt_D);
  gHistos.Add(hEffLPt_N);
  gHistos.Add(hEffLPt_D);
  gHistos.Add(hEffAlgoPt_D);
  gHistos.Add(hEffAlgoPt_N);
  gHistos.Add(hPurePt_N);
  gHistos.Add(hPurePt_D);

//  gHistos.Add(new TH1D("h_Pt","h_Pt",100, -1.2, 1.2));
//  gHistos.Add( new TH1D("h_PullPt","h_PullPt",100,-8.,8.));
//  gHistos.Add( new TH1F("h_Tip","h_Tip",100, -0.06, 0.06) );
//  gHistos.Add( new TH1F("h_PullTip","h_PullTip",100, -8.0, 8.0) );

  hMap["h_Pt"] = new TH1D("h_Pt","h_Pt",100, -1.2, 1.2);
  hMap["h_PullPt"] = new TH1D("h_PullPt","h_PullPt",100,-8.,8.);
  hMap["h_Tip"] = new TH1D("h_Tip","h_Tip",100, -0.1, 0.1);
  hMap["h_TipError"] = new TH1D("h_TipError","h_TipError",100, 0., 0.2);
  hMap["h_PullTip"] = new TH1D("h_PullTip","h_PullTip",100, -8.0, 8.0);
  hMap["h_Zip"] = new TH1D("h_Zip","h_Zip",100, -0.1, 0.1);
  hMap["h_ZipV"] = new TH1D("h_ZipV","h_ZipV",100, -0.08, 0.08);
  hMap["h_ZipError"] = new TH1D("h_ZipError","h_ZipError",100,0.,0.1);
  hMap["h_PullZip"] = new TH1D("h_PullZip","h_PullZip",100, -8., 8.0);
  hMap["h_Cot"] = new TH1D("h_Cot","h_Cot",100, -0.04, 0.04);
  hMap["h_PullCot"] = new TH1D("h_PullCot","h_PullCot",100, -8., 8.0);
  hMap["h_chi2"] = new TH1D("h_chi2","h_chi2",1000,0.,10.);

  for (HMap::const_iterator im = hMap.begin(); im != hMap.end(); ++im)  gHistos.Add(im->second); 


}

//----------------------------------------------------------------------------------------
Analysis::~Analysis()
{ 
  cout <<"WRITING ROOT FILE"<< std::endl;
  TFile rootFile("analysis.root","RECREATE");
  gHistos.Write();
  rootFile.Close();
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
    if ( fabs(eta_gen) > 2.5 ) continue;

    float pt_gen = track.momentum().pt();
    if (pt_gen < ptMin) continue;

    if (sqrt(vertex(&track)->position().perp2()) > 0.6) continue;

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
//  cout <<" Analysis has: " << theSimTracks.size()<<" tracks"<<endl;

  Handle<SimVertexContainer> simVc;
  ev.getByLabel("g4SimHits", simVc);
  theSimVertices = *(simVc.product());
//  cout <<" Analysis has: " << theSimVertices.size()<<" vertices"<<endl;

  Handle<GenParticleCollection> genParticles;
  ev.getByLabel("genParticles", genParticles);
//  cout <<" Analysis has: "<< genParticles->size() << " particles!" << endl;
/*
   for(size_t i = 0; i < genParticles->size(); ++ i) {
     const GenParticle & p = (*genParticles)[i];
     int id = p.pdgId();
     int st = p.status();  
     const Candidate * mom = p.mother();
     double pt = p.pt(), eta = p.eta(), phi = p.phi(), mass = p.mass();
     double vx = p.vx(), vy = p.vy(), vz = p.vz();
     int charge = p.charge();
     int n = p.numberOfDaughters();
     for(size_t j = 0; j < n; ++ j) {
       const Candidate * d = p.daughter( j );
       int dauId = d->pdgId();
       // . . . 
     }
     // . . . 
   }
*/
 

  theAssociator = ass;
//  if (theAssociator) delete theAssociator;
}

//----------------------------------------------------------------------------------------
/*
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
    float pt_gen = track.momentum().pt();
    int nHitsInLayes = 0;
    typedef vector<SeedingLayer>::const_iterator IL;
    for (IL il = layers.begin(); il != layers.end(); il++) {
      vector<Hit> hits = (*il).hits(*theEvent,*theSetup);
      int nmatched = matchedHits(track.trackId(), hits);
      if (nmatched > 0) nHitsInLayes++; 
    }
    if (nHitsInLayes!=nLayers) continue;

    bool matched = false;
    unsigned int nSets = candidates.size();
    for (unsigned int ic= 0; ic <nSets; ic++) {
      typedef SeedingHitSet::RecHits RecHits;
//      const RecHits & hits = candidates[ic].hits();
      unsigned int nmatched = matchedHits(track.trackId(),  candidates[ic]);
      if (nmatched == candidates[ic].size() ) matched = true;
//      if (nmatched == hits.size() ) matched = true;
    }

    if (fabs(eta_gen) < 2.1 && fabs(eta_gen) > 1.9) {
      hEffAlgoPt_D->Fill(pt_gen+1.e-2);
      if(matched) hEffAlgoPt_N->Fill(pt_gen+1.e-2);
      if(!matched) cout <<"INEFFICIENCY"<<endl;
    }
    if (pt_gen > 0.9) {
      hEffAlgoEta_D->Fill(eta_gen);
      if(matched) hEffAlgoEta_N->Fill(eta_gen);
    }
  }
  }
}
*/

/*
//-----------------------------------------------------------------------------------------
void Analysis::checkAlgoEfficiency2(const SeedingLayerSets &layersSets, const OrderedSeedingHits& candidates)
{
  typedef  SimTrackContainer::const_iterator IP;
  for (IP ip=theSimTracks.begin(); ip != theSimTracks.end(); ip++) {

    const SimTrack & track = (*ip);
    bool selected = select(track);
    if (!selected) continue;
    float eta_gen = track.momentum().eta();
    float pt_gen = track.momentum().pt();

    for (SeedingLayerSets::const_iterator ils=layersSets.begin(); ils!=layersSets.end(); ils++) {
      const SeedingLayers & layers = (*ils);
      AnalysisOrderedHitsInLayers hitsInLayers(layers, *theEvent, *theSetup);

//      if (!hitsInLayers.onlySingleHits()) continue;

      vector<SeedingHitSet> layerHitSets;
      while ( hitsInLayers.next() ) { 

        SeedingHitSet layerHitSet = hitsInLayers.getHitSet();
//FIXME - NOT WORK!
//        unsigned int  nmatched = matchedHits(track.trackId(), layerHitSet.hits());
//        if (nmatched == layerHitSet.size() ) layerHitSets.push_back(layerHitSet);
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
      if (fabs(eta_gen) < 2.4) {
        hEffAlgoPt_D->Fill(pt_gen+1.e-2);
        if(matched) hEffAlgoPt_N->Fill(pt_gen+1.e-2);
      }
      // eta efficiency
      if (pt_gen > 0.9) {
        hEffAlgoEta_D->Fill(eta_gen);
        if(matched) hEffAlgoEta_N->Fill(eta_gen);
      }
    }
  }
}
*/

//----------------------------------------------------------------------------------------
void Analysis::checkEfficiency( const reco::TrackCollection & tracks)
{
  typedef  SimTrackContainer::const_iterator IP;
//  math::XYZPoint bs(0.2,0.4,-2.4);
//  math::XYZPoint bs(0.0,0.0,-2.5);

//  typedef SeedingHitSet::RecHits RecHits;

  for (IP ip=theSimTracks.begin(); ip != theSimTracks.end(); ip++) {
    const SimTrack & track = (*ip); 
    bool selected = select(track);
    if (!selected) continue;
    std::cout << " vertex: "<<vertex(&track)->position() << std::endl;
    float eta_gen = track.momentum().eta();
    float pt_gen = track.momentum().pt();
    std::cout <<" pt_gen is: " << pt_gen << endl;
    math::XYZPoint bs(vertex(&track)->position().x(),
                      vertex(&track)->position().y(),
                      vertex(&track)->position().z());

    bool matched = false;
    const reco::Track * it = 0;
    typedef reco::TrackCollection::const_iterator IT;
    double pt_min = 0.;
    for (IT iit=tracks.begin(); iit!=tracks.end(); iit++) {
      float pt = (*iit).pt();
      if (pt < pt_min) continue;
      float eta_rec = (*iit).momentum().eta();
      if ( fabs(eta_gen-eta_rec) < 0.15) {
	      matched = true;
	      pt_min = pt;
	      it = &(*iit);
      }
    }
    if (!it) return;
      float pt_rec = (*it).pt();
      float eta_rec = (*it).momentum().eta();
      if ( fabs(eta_gen-eta_rec) < 0.15) matched = true;
      print(*it,bs);
//    static_cast<TH1*>(gHistos.FindObject("h_Pt"))->Fill((pt_gen - pt_rec)/pt_gen);
//    static_cast<TH1*>(gHistos.FindObject("h_PullPt"))->Fill((pt_gen - pt_rec)/(*it).ptError());
//    static_cast<TH1*>(gHistos.FindObject("h_Tip"))->Fill((*it).d0());
//    static_cast<TH1*>(gHistos.FindObject("h_PullTip"))->Fill((*it).d0()/(*it).d0Error());
      hMap["h_Pt"]->Fill((pt_gen - pt_rec)/pt_gen);
      hMap["h_PullPt"]->Fill((pt_gen - pt_rec)/(*it).ptError());
      hMap["h_Tip"]->Fill((*it).dxy(bs));
      hMap["h_TipError"]->Fill((*it).d0Error());
      hMap["h_PullTip"]->Fill((*it).dxy(bs)/(*it).d0Error());
      hMap["h_Zip"]->Fill( (*it).dz(bs) );
//      hMap["h_PullZip"]->Fill(( (*it).dz(bs)-vertex(&track)->position().z())/(*it).dzError());
      hMap["h_ZipError"]->Fill( (*it).dzError());
      hMap["h_PullZip"]->Fill( (*it).dz(bs)/(*it).dzError());

      float cosTheta = cos(it->theta());
      float sinTheta = sin(it->theta());
      float errLambda2 = sqr( it->lambdaError() );
      float dCotTheta = cosTheta/sinTheta - 1./tan(track.momentum().theta());
      hMap["h_Cot"]->Fill(dCotTheta);
      hMap["h_PullCot"]->Fill(dCotTheta / sqrt(errLambda2)/sqr(sinTheta));

      hMap["h_chi2"]->Fill((*it).chi2());

    if (fabs(eta_gen) < 2.1) {
      hEffPt_D->Fill(pt_gen+1.e-4);
      hEffLPt_D->Fill(pt_gen+1.e-4);
      if(matched) hEffPt_N->Fill(pt_gen+1.e-4);
      if(matched) hEffLPt_N->Fill(pt_gen+1.e-4);
    }
    if (pt_gen > 0.9) {
      hEffEta_D->Fill(eta_gen);
      if(matched) hEffEta_N->Fill(eta_gen);
    }
    
  }
}

//----------------------------------------------------------------------------------------
/*
void Analysis::checkEfficiency( const OrderedSeedingHits & candidates)
{
  typedef  SimTrackContainer::const_iterator IP;


  for (IP ip=theSimTracks.begin(); ip != theSimTracks.end(); ip++) {
    const SimTrack & track = (*ip); 
    bool selected = select(track);
    if (!selected) continue;
    float eta_gen = track.momentum().eta();
    float pt_gen = track.momentum().pt();
    std::cout<<"HERE"<<std::endl;
//    print(track);

    bool matched = false;
    unsigned int nSets = candidates.size();
    for (unsigned int ic= 0; ic <nSets; ic++) {
      unsigned int nmatched = matchedHits(track.trackId(), candidates[ic]); 
      if (nmatched == candidates[ic].size() ) matched = true;
      std::cout <<"nmatched: " <<nmatched<<" hits: " << candidates[ic].size() <<std::endl;
    }

    //if (fabs(eta_gen) < 1.4) {
    if (fabs(eta_gen) > 1.5 && fabs(eta_gen) < 2.4) {
      hEffPt_D->Fill(pt_gen+1.e-2);
      if(matched) hEffPt_N->Fill(pt_gen+1.e-2);
    }
    if (pt_gen > 0.9) {
      hEffEta_D->Fill(eta_gen);
      if(matched) hEffEta_N->Fill(eta_gen);
    }
    
  }
}
*/


//----------------------------------------------------------------------------------------
unsigned int Analysis::matchedHits(unsigned int trackId, const SeedingHitSet& hitset) 
{
  unsigned int nmatched = 0;
  for (unsigned int ih=0; ih < hitset.size(); ih++) {
    const TrackingRecHit * hit = hitset[ih]->hit();
    typedef vector<SimHitIdpr> SimTrackIds;
    SimTrackIds simTrackIds = theAssociator->associateHitId(*hit);
    std::cout <<" ASSOCIATION: hit:"<<hit<<" "<<hit->localPosition()<<" number of SimTrackIds: "<<simTrackIds.size()<<endl;
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
    const TrackingRecHit* trh1 = hits1[i]->hit();
    const TrackingRecHit* trh2 = hits2[i]->hit();
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
//  if ( fabs(eta_gen) > 2.0 ) return false;
  return true;
}

//----------------------------------------------------------------------------------------
void Analysis::print(const SimTrack & track) 
{
  if ( track.type() == -99) return;
  float phi_gen = track.momentum().phi();
  float pt_gen = track.momentum().pt();
  float eta_gen = track.momentum().eta();
//    HepLorentzVector vtx =(*simVtcs)[p->vertIndex()].position();
//    float z_gen  = vtx.z();
   cout <<" trackId: " <<track.trackId()
          << " pt_gen: " << pt_gen <<" eta_gen: "<<eta_gen<<" phi_gen: "<<phi_gen 
          <<" vtx: "<<track.vertIndex()<<" type: "<<track.type()
          << endl;

}

void Analysis::print(const SimVertex & vertex)
{
  std::cout <<vertex<< std::endl;
}
//----------------------------------------------------------------------------------------
//std::string Analysis::print(const SeedingHit & hit) 
//{
//  ostringstream str; 
//  str <<"r="<<hit.r() <<" phi="<<hit.phi()<<" z="<<hit.z();
//  return str.str();
//}

//----------------------------------------------------------------------------------------
void Analysis::print(const reco::Track & track, const math::XYZPoint & bs)
{
    cout << "--- RECONSTRUCTED TRACK: " << endl;
    cout << "\tmomentum: " << track.momentum()
         << "\tPT: " << track.pt()<<"+/-"<<track.ptError()<< endl;
    cout << "\treference: " << track.vertex() << endl
         << "\t zip: " <<  track.dz()<<"+/-"<<track.dzError()
         << "\t tip: " << track.d0()<<"+/-"<<track.d0Error()<< endl;
    cout <<" \t TIP(BS) "<<track.dxy(bs) << "\t DZ(BS) : "<<  track.dz(bs) << endl;
    cout << "\t chi2: "<< track.chi2()<<endl;
    
    cout << "\tcharge: " << track.charge()<< endl;

}

