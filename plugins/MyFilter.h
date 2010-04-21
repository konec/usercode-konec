#ifndef MyFilter_h
#define MyFilter_h

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/print.h"

#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTExtendedCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTReadoutCollection.h"

#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"

// #include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
// #include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "TrackingTools/GeomPropagators/interface/Propagator.h"

#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"
#include "DataFormats/GeometrySurface/interface/BoundCylinder.h"
#include "DataFormats/GeometrySurface/interface/SimpleCylinderBounds.h"
#include "DataFormats/GeometrySurface/interface/BoundDisk.h"
#include "DataFormats/GeometrySurface/interface/SimpleDiskBounds.h"

#include "DataFormats/MuonDetId/interface/RPCDetId.h"
using namespace edm;
using namespace std;

class MyFilter {
private:
  std::string theOption;
  bool theCheckL1Matching;
public: 
  MyFilter(const std::string & option, bool checkL1Matching) : theOption(option), theCheckL1Matching(checkL1Matching) {}
  virtual ~MyFilter(){}

  bool checkMatching(const TrajectoryStateOnSurface & tsos, float rpcEta, float rpcPhi, const edm::Event&ev, const edm::EventSetup& es) const {
    edm::ESHandle<GlobalTrackingGeometry> globalGeometry;
    es.get<GlobalTrackingGeometryRecord>().get(globalGeometry);
    edm::ESHandle<MagneticField> magField;
    es.get<IdealMagneticFieldRecord>().get(magField);
    ReferenceCountingPointer<Surface> rpc;
    if (rpcEta < -1.04)      rpc = ReferenceCountingPointer<Surface>(new  BoundDisk( GlobalPoint(0.,0.,-800.), TkRotation<float>(), SimpleDiskBounds( 300., 710., -10., 10. ) ) );
    else if (rpcEta < 1.04)  rpc = ReferenceCountingPointer<Surface>(new  BoundCylinder( GlobalPoint(0.,0.,0.), TkRotation<float>(), SimpleCylinderBounds( 500, 520, -700, 700 ) ) );
    else                     rpc = ReferenceCountingPointer<Surface>(new  BoundDisk( GlobalPoint(0.,0.,800.), TkRotation<float>(), SimpleDiskBounds( 300., 710., -10., 10. ) ) );
    edm::ESHandle<Propagator> propagator;
    es.get<TrackingComponentsRecord>().get("SteppingHelixPropagatorAny", propagator);
    TrajectoryStateOnSurface trackAtRPC =  propagator->propagate(tsos, *rpc);
    if (!trackAtRPC.isValid()) return false;
    float phi = trackAtRPC.globalPosition().phi();
    float dphi = phi-rpcPhi;
    if (dphi < -M_PI) dphi+=2*M_PI;
    if (dphi > M_PI) dphi-=2*M_PI;
    float eta = trackAtRPC.globalPosition().eta();
    float deta = eta-rpcEta;
    return ( fabs(dphi) < 0.25 && fabs(deta) < 0.2) ? true : false; 
  }

  bool checkTriggerMatching( const TrajectoryStateOnSurface & tsos,  const edm::Event&ev, const edm::EventSetup& es) const {
    edm::Handle<L1MuGMTReadoutCollection> pCollection;
    InputTag rpctfSource_("l1GtUnpack");
    ev.getByLabel(rpctfSource_,pCollection);
    L1MuGMTReadoutCollection const* gmtrc = pCollection.product();
    if (!gmtrc) return false;

    vector<L1MuGMTReadoutRecord> gmt_records = gmtrc->getRecords();
    vector<L1MuGMTReadoutRecord>::const_iterator RRItr;
    typedef vector<L1MuRegionalCand>::const_iterator ITC;
    for( RRItr = gmt_records.begin() ; RRItr != gmt_records.end() ; RRItr++ ) {
      for (int i=1; i<=2; ++i) {
        vector<L1MuRegionalCand> Cands = (i==1) ? RRItr->getBrlRPCCands() : RRItr->getFwdRPCCands();
        for(ITC it = Cands.begin() ; it != Cands.end() ; ++it ) {
          if (it->empty()) continue;
          if (checkMatching(tsos, it->etaValue(), it->phiValue(),ev,es)) return true; 
        }
      }
    }
    return false;
  } 

  bool takeIt(const RPCDetId & det, const edm::Event&ev, const edm::EventSetup& es) const {

    edm::Handle<reco::TrackCollection> trackCollection;
    ev.getByLabel(InputTag(theOption),trackCollection);

    edm::ESHandle<GlobalTrackingGeometry> globalGeometry;
    es.get<GlobalTrackingGeometryRecord>().get(globalGeometry);

    edm::ESHandle<MagneticField> magField;
    es.get<IdealMagneticFieldRecord>().get(magField);

    reco::TrackCollection tracks = *(trackCollection.product());
//  cout <<"#RECONSTRUCTED tracks: " << tracks.size() << endl;

    bool inside = false;
    typedef reco::TrackCollection::const_iterator IT;
    for (IT it = tracks.begin(); it !=tracks.end(); ++it) {
      const reco::Track & track = *it;
      if (track.pt() < 2. ) continue;

      TrajectoryStateOnSurface aTSOS = TrajectoryStateTransform().outerStateOnSurface(track, *globalGeometry, magField.product());
      if (theCheckL1Matching && !checkTriggerMatching(aTSOS, ev,es) ) continue; 

      edm::ESHandle<Propagator> propagator;
      es.get<TrackingComponentsRecord>().get("SteppingHelixPropagatorAny", propagator);

      TrajectoryStateOnSurface trackAtRPC =  propagator->propagate(aTSOS, globalGeometry->idToDet(det)->surface());
      if (!trackAtRPC.isValid()) continue;
      globalGeometry->idToDet(det)->specificSurface();
      std::cout <<" **** RPC position is (propagation to DetUnit:"<<det.rawId()<<"), position:"<<globalGeometry->idToDet(det)->position()
                              <<", r= "<<trackAtRPC.globalPosition().perp()
                              <<", z= "<<trackAtRPC.globalPosition().z()
                            <<", phi= "<<trackAtRPC.globalPosition().phi()
                            <<", eta= "<<trackAtRPC.globalPosition().eta()
              <<" localPosition: "<<trackAtRPC.localPosition()
              <<" isInside: "<< globalGeometry->idToDet(det)->surface().bounds().inside( trackAtRPC.localPosition())
              << std::endl;

      if (globalGeometry->idToDet(det)->surface().bounds().inside( trackAtRPC.localPosition())) inside = true;
    }
    return inside;
  } 

};

#endif
