#ifndef OrbitFilter_H
#define OrbitFilter_H
// -*- C++ -*-
//
// Package:    OrbitFilter
// Class:      OrbitFilter
//
/**\class OrbitFilter OrbitFilter.cc MyAna/OrbitFilter/src/OrbitFilter.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Tomasz Maciej Frueboes
//         Created:  Sat Jan 24 15:11:35 CET 2009
// $Id$
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

//
// class declaration
//

class OrbitFilter : public edm::EDFilter {
   public:
      explicit OrbitFilter(const edm::ParameterSet&);
      ~OrbitFilter();

   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual bool filter(edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      int m_minOrbit;
      int m_maxOrbit;
      int m_orbPassed;
      // ----------member data ---------------------------
};

#endif
