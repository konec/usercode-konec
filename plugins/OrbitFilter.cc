#include "OrbitFilter.h"

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
OrbitFilter::OrbitFilter(const edm::ParameterSet& iConfig):
m_minOrbit( iConfig.getParameter<int>("minOrbit") ),
m_maxOrbit( iConfig.getParameter<int>("maxOrbit") ),
m_orbPassed(0)
{
   //now do what ever initialization is needed

}


OrbitFilter::~OrbitFilter()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
OrbitFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   static int orbit;

  orbit = iEvent.orbitNumber();

  bool ret = (orbit>=m_minOrbit && orbit<=m_maxOrbit  );
  if (ret) {
    LogDebug("OrbitFilter") << "Passed: " <<  m_orbPassed++ << std::endl;
  }

  return ret;

}

// ------------ method called once each job just before starting event loop  ------------
void 
OrbitFilter::beginJob(const edm::EventSetup&)
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
OrbitFilter::endJob() {
}

