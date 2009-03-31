#include "AnalysisOrderedHitsInLayers.h"


using namespace ctfseeding;
using namespace std;

typedef TransientTrackingRecHit::ConstRecHitPointer Hit;

AnalysisOrderedHitsInLayers::AnalysisOrderedHitsInLayers(
     const SeedingLayers& layers, const edm::Event& ev, const edm::EventSetup& es)
  : theFirstEvent(true), theIndexCounters(layers.size(), 0)
{
  typedef vector<SeedingLayer>::const_iterator IL;
  for (IL il = layers.begin(); il != layers.end(); il++) {
    vector<Hit> hits = (*il).hits(ev,es);
    theHitsInLayers.push_back(hits);
  }
//  cout <<"** HITS: ";
//  for (unsigned int i=0; i< theIndexCounters.size(); ++i) cout <<theHitsInLayers[i].size()<<", ";
//  cout << endl;

}

bool AnalysisOrderedHitsInLayers::onlySingleHits() const
{
  for (unsigned int i=0; i< theIndexCounters.size(); ++i) {
    if (theHitsInLayers[i].size() != 1) return false;
  }
 return true;
}

bool AnalysisOrderedHitsInLayers::increaseCounter(unsigned int depth)
{
  if (depth < 0 || depth >= theIndexCounters.size()) return false;
  theIndexCounters[depth]++;
  if (theIndexCounters[depth] > theHitsInLayers[depth].size()-1) {
    bool nextIncrease = increaseCounter(depth-1); 
    if (nextIncrease) theIndexCounters[depth] = 0; else return false;
  }
  return true;
}

bool AnalysisOrderedHitsInLayers::next() 
{

  if (theFirstEvent) {
    theFirstEvent = false;
  }
  else {
    increaseCounter(theIndexCounters.size()-1);
  }


  for (unsigned int i=0; i< theIndexCounters.size(); ++i) {
    if (theIndexCounters[i] >= theHitsInLayers[i].size()) return false;
  } 

//  cout <<"Indexes: ";
//  for (unsigned int i=0; i< theIndexCounters.size(); ++i) cout <<theIndexCounters[i]<<", ";
//  cout << endl;  

  return true;
}

SeedingHitSet AnalysisOrderedHitsInLayers::getHitSet() const
{
  SeedingHitSet set;
  for (unsigned int il = 0; il < theIndexCounters.size(); ++il) {
    const SeedingHitSet & layerHits = theHitsInLayers[il];
/// FIXME: compilation fix! DOES NOT WORK
//    set.add(layerHits[ theIndexCounters[il] ]);
  } 
  return set;
}
