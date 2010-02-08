import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100))
process.source = cms.Source("PoolSource", fileNames =  cms.untracked.vstring(
  'file:/disk00/work/data/cmssw336/MinBias_STARTUP3X_V8N_900GeV.root'
#   'file:SingleMu_0m075_xy.root'
#  'file:/disk00/work/data/cmssw336/Mupm_4.00.root'
#   'file:DoubleMu_3m_xy.root'
#   'file:DoubleMu_3m_00.root'
#  'file:SingleMu_1m_xy.root'
#  'file:MinBias.root'
#  'rfio:/castor/cern.ch/user/k/konec/cmssw31x/SingleMu_10m00.root' 
#'/store/relval/CMSSW_3_1_0_pre4/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/IDEAL_30X_v1/0003/3AA6EEA4-3B16-DE11-B35F-001617C3B654.root'
))


# import of standard configurations
process.load('Configuration/StandardSequences/Services_cff')
process.load('Configuration/StandardSequences/MixingNoPileUp_cff')
process.load('Configuration/StandardSequences/GeometryExtended_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
process.load('Configuration/StandardSequences/RawToDigi_cff')
process.load('Configuration/StandardSequences/Reconstruction_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load('FWCore/MessageService/MessageLogger_cfi')
process.MessageLogger = cms.Service("MessageLogger",
    debugModules = cms.untracked.vstring('pixelVertices'),
    #debugModules = cms.untracked.vstring('*'),
    destinations = cms.untracked.vstring('cout'),
    cout = cms.untracked.PSet( threshold = cms.untracked.string('DEBUG'))
)


# Other statements
#process.GlobalTag.globaltag = 'IDEAL_31X::All'
#process.GlobalTag.globaltag = 'STARTUP31X_V1::All'
#process.GlobalTag.globaltag = 'MC_31X_V9::All'
process.GlobalTag.globaltag ='STARTUP3X_V8N::All'


process.load("RecoTracker.Configuration.RecoTracker_cff")
from RecoTracker.Configuration.RecoTracker_cff import *
process.load('RecoLocalTracker/Configuration/RecoLocalTracker_cff')
process.load("RecoPixelVertexing.PixelTrackFitting.PixelTracks_cff")
from RecoPixelVertexing.PixelTrackFitting.PixelTracks_cff import *
from RecoPixelVertexing.PixelTrackFitting.PixelFitterByConformalMappingAndLine_cfi import *

GBlock= cms.PSet(
  ComponentName = cms.string('GlobalRegionProducer'),
  RegionPSet = cms.PSet(
     precise = cms.bool(True),
     ptMin = cms.double(0.875),
     originHalfLength = cms.double(15.9),
     originRadius = cms.double(0.2),
     originXPos = cms.double(0.0),
     originYPos = cms.double(0.0),
     originZPos = cms.double(0.0)
  ) 
)

BBlock = cms.PSet(
  ComponentName = cms.string('GlobalRegionProducerFromBeamSpot'),
  RegionPSet = cms.PSet(
    precise = cms.bool(True),
    nSigmaZ = cms.double(3.0),
    originRadius = cms.double(0.2),
    ptMin = cms.double(0.8),
    beamSpot = cms.InputTag("offlineBeamSpot")
  )
)


PixelTripletLowPtGenerator = cms.PSet (
   ComponentName = cms.string('PixelTripletLowPtGenerator'),
   checkClusterShape       = cms.bool(False),
   checkMultipleScattering = cms.bool(True),
   nSigMultipleScattering  = cms.double(5.0),
   maxAngleRatio = cms.double(10.0),
   rzTolerance   = cms.double(0.2),
   TTRHBuilder   = cms.string('TTRHBuilderWithoutAngle4PixelTriplets')
)

PixelTripletNoTipGenerator = cms.PSet(
  ComponentName = cms.string('PixelTripletNoTipGenerator'),
  extraHitPhiToleranceForPreFiltering = cms.double(0.3),
  extraHitRPhitolerance = cms.double(0.000),
  extraHitRZtolerance = cms.double(0.030),
  nSigma = cms.double(3.0),
  chi2Cut = cms.double(25.)
)

PixelTripletHLTGenerator = cms.PSet (
  ComponentName = cms.string('PixelTripletHLTGenerator'),
  useFixedPreFiltering = cms.bool(False),
  phiPreFiltering = cms.double(0.3),
  useBending = cms.bool(True),
  extraHitRPhitolerance = cms.double(0.032),
  useMultScattering = cms.bool(True),
  extraHitRZtolerance = cms.double(0.037),
  maxTriplets = cms.uint32(10000) 
)

FitterPSet = cms.PSet(
#  ComponentName = cms.string('TrackFitter'),
#  ComponentName = cms.string('PixelFitterByConformalMappingAndLine'),
  ComponentName = cms.string('PixelFitterByHelixProjections'),
  TTRHBuilder   = cms.string('TTRHBuilderWithoutAngle4PixelTriplets')
)


process.pixelTracks.RegionFactoryPSet= cms.PSet( BBlock ) 
process.pixelTracks.FilterPSet.ComponentName = cms.string('none') 
process.pixelTracks.OrderedHitsFactoryPSet.GeneratorPSet = cms.PSet ( PixelTripletHLTGenerator )
process.pixelTracks.FitterPSet = cms.PSet(FitterPSet)
process.pixelVertices.Verbosity = cms.int32(2)


process.analysis = cms.EDFilter("TrackAnalysis",
  TrackCollection = cms.string("pixelTracks"),
#  TrackCollection = cms.string("preFilterZeroStepTracks"),
  AssociatorPSet = cms.PSet(
    associateStrip = cms.bool(False),
    associatePixel = cms.bool(False),
    associateRecoTracks = cms.bool(False),
     MinHitFraction = cms.double(0.5),
     ROUList = cms.vstring("TrackerHitsTIBLowTof","TrackerHitsTIBHighTof",
        "TrackerHitsTIDLowTof","TrackerHitsTIDHighTof",
        "TrackerHitsTOBLowTof","TrackerHitsTOBHighTof",
        "TrackerHitsTECLowTof","TrackerHitsTECHighTof",
        "TrackerHitsPixelBarrelLowTof","TrackerHitsPixelBarrelHighTof",
        "TrackerHitsPixelEndcapLowTof","TrackerHitsPixelEndcapHighTof")
  ),
  AnalysisPSet = cms.PSet(
     useParticleId = cms.int32(13),
     ptMinLeadingTrack = cms.double(0.05)
  )
)
#process.p=cms.Path(process.RawToDigi*process.pixeltrackerlocalreco*process.pixelTracks*process.pixelVertices*process.analysis) 
process.p=cms.Path(process.siPixelRecHits*process.pixelTracks*process.analysis) 
#process.p=cms.Path(process.RawToDigi*process.trackerlocalreco*process.pixelTracks*process.analysis) 
#process.p=cms.Path(process.RawToDigi*process.trackerlocalreco*process.offlineBeamSpot*process.globalPixelSeeds*newTrackCandidateMaker*preFilterZeroStepTracks*process.analysis) 
