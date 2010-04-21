#ifndef DQM_RPCMonitorClient_RPCMonitorLinkSynchroWithSelector_H
#define DQM_RPCMonitorClient_RPCMonitorLinkSynchroWithSelector_H

#include "DQM/RPCMonitorClient/interface/RPCMonitorLinkSynchro.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "CondFormats/RPCObjects/interface/RPCReadOutMapping.h"
#include "CondFormats/RPCObjects/interface/LinkBoardSpec.h"

#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESTransientHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESWatcher.h"
#include "CondFormats/RPCObjects/interface/RPCEMap.h"
#include "CondFormats/DataRecord/interface/RPCEMapRcd.h"

#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "MyFilter.h"
#include <map> 


class RPCMonitorLinkSynchroWithSelector : public RPCMonitorLinkSynchro {
public:
  RPCMonitorLinkSynchroWithSelector(const edm::ParameterSet& cfg) : RPCMonitorLinkSynchro(cfg), theCabling(0) { std::cout <<"KUKU" << std::endl;}
  virtual ~RPCMonitorLinkSynchroWithSelector(){ delete theCabling;}

  virtual  const RPCRawSynchro::ProdItem& select(const RPCRawSynchro::ProdItem & vItem, const edm::Event &ev, const edm::EventSetup &es){

    if (theMapWatcher.check(es)) {
      delete theCabling;
      edm::ESTransientHandle<RPCEMap> readoutMapping;
      es.get<RPCEMapRcd>().get(readoutMapping);
      theCabling = readoutMapping->convert();
      LogTrace("") << "RPCMonitorLinkSynchroWithSelector - record has CHANGED!!, read map, VERSION: " << theCabling->version();
    }

    static RPCRawSynchro::ProdItem selected;
    selected.clear();

    for(RPCRawSynchro::ProdItem::const_iterator it = vItem.begin(); it != vItem.end(); ++it) {
      const LinkBoardElectronicIndex & path = it->first;
      const  std::vector<FebConnectorSpec> & febs = theCabling->location(path)->febs();
      std::map<uint32_t,bool> dets;
      for (std::vector<FebConnectorSpec>::const_iterator iif = febs.begin(); iif != febs.end(); ++iif) dets[iif->rawId()] = true; 

      bool takeIt = false;
      std::cout <<"SIZE OF DETS IS: " << dets.size() << std::endl;
      for ( std::map<uint32_t,bool>::const_iterator im = dets.begin(); im != dets.end(); ++im) {
        RPCDetId rpcDet(im->first);
        if (MyFilter("globalMuons",false).takeIt(rpcDet,ev,es)) takeIt = true;
        if (MyFilter("generalTracks",true).takeIt(rpcDet,ev,es)) takeIt = true;
      }
      if (takeIt) selected.push_back(*it);
    }
    return selected;

  }
private:
  edm::ESWatcher<RPCEMapRcd> theMapWatcher;
  RPCReadOutMapping * theCabling;
};
#endif

