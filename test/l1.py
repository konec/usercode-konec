import FWCore.ParameterSet.Config as cms
import sys
process = cms.Process("Analysis")

# max events
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1))

# input file
reference = 'rfio:/castor/cern.ch/user/k/konec/cmssw31x/'
filenames = reference+sys.argv[2]+'.root'
process.source = cms.Source("PoolSource", fileNames =  cms.untracked.vstring( filenames ) )

# import of standard configurations
process.load('Configuration/StandardSequences/Services_cff')
process.load('Configuration/StandardSequences/GeometryExtended_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
process.load('Configuration/StandardSequences/RawToDigi_cff')
process.load('Configuration/StandardSequences/Reconstruction_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load('FWCore/MessageService/MessageLogger_cfi')

# Other statements
process.GlobalTag.globaltag = 'IDEAL_31X::All'

process.load("RecoMuon.TrackerSeedGenerator.TSGFromL1_cff")
from RecoMuon.TrackerSeedGenerator.TSGFromL1_cfi import *

process.l1seeding = cms.EDFilter("L1Seeding",
  FitterPSet = hltL3TrajectorySeedFromL1.FitterPSet,
  OrderedHitsFactoryPSet =  hltL3TrajectorySeedFromL1.OrderedHitsFactoryPSet,
  AnalysisPSet = cms.PSet( useParticleId = cms.int32(13), ptMinLeadingTrack = cms.double(0.9))
)

# Path and EndPath definitions
process.p=cms.Path(process.RawToDigi*process.pixeltrackerlocalreco*process.l1seeding)
