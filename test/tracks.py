import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(5))
process.source = cms.Source("PoolSource", fileNames =  cms.untracked.vstring(
  'file:SingleMu.root'
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

# Other statements
#process.GlobalTag.globaltag = 'IDEAL_31X::All'
#process.GlobalTag.globaltag = 'STARTUP31X_V1::All'
process.GlobalTag.globaltag = 'MC_31X_V9::All'

process.load("RecoTracker.Configuration.RecoTracker_cff")
from RecoTracker.Configuration.RecoTracker_cff import *
process.load('RecoLocalTracker/Configuration/RecoLocalTracker_cff')
process.load("RecoPixelVertexing.PixelTrackFitting.PixelTracks_cff")

process.load("RecoTracker.IterativeTracking.FirstStep_cff")
from RecoTracker.IterativeTracking.FirstStep_cff import *
newTrackCandidateMaker.src = cms.InputTag('globalPixelSeeds')
#newTrackCandidateMaker.src = cms.InputTag('hltL3TrajectorySeedFromL1')
#newTrackCandidateMaker.src = cms.InputTag('primSeeds')
#newTrackCandidateMaker.src = cms.InputTag('seedsFromProtoTracks')

process.analysis = cms.EDFilter("TrackAnalysis",
#  TrackCollection = cms.string("pixelTracks"),
  TrackCollection = cms.string("preFilterZeroStepTracks"),
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
process.p=cms.Path(process.RawToDigi*process.trackerlocalreco*process.offlineBeamSpot*process.globalPixelSeeds*newTrackCandidateMaker*preFilterZeroStepTracks*process.analysis) 
#process.p=cms.Path(process.offlineBeamSpot*process.globalPixelSeeds*newTrackCandidateMaker*preFilterZeroStepTracks*process.analysis)
