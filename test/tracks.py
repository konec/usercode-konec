import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1))
process.source = cms.Source("PoolSource", fileNames =  cms.untracked.vstring(
#  'rfio:/castor/cern.ch/user/k/konec/cmssw31x/SingleMu_0m04.root',
#  'rfio:/castor/cern.ch/user/k/konec/cmssw31x/SingleMu_0m06.root',
# 'rfio:/castor/cern.ch/user/k/konec/cmssw31x/SingleMu_0m07.root',
#   'rfio:/castor/cern.ch/user/k/konec/cmssw31x/SingleMu_0m08.root',
# 'rfio:/castor/cern.ch/user/k/konec/cmssw31x/SingleMu_0m09.root',
#    'rfio:/castor/cern.ch/user/k/konec/cmssw31x/SingleMu_0m1.root' 
#   'rfio:/castor/cern.ch/user/k/konec/cmssw31x/SingleMu_0m15.root' 
#  'rfio:/castor/cern.ch/user/k/konec/cmssw31x/SingleMu_0m2.root' 
#   'rfio:/castor/cern.ch/user/k/konec/cmssw31x/SingleMu_0m3.root' 
# 'rfio:/castor/cern.ch/user/k/konec/cmssw31x/SingleMu_0m4.root' 
  'rfio:/castor/cern.ch/user/k/konec/cmssw31x/SingleMu_0m6.root' 
#    'rfio:/castor/cern.ch/user/k/konec/cmssw31x/SingleMu_1m0.root'
#'/store/relval/CMSSW_3_1_0_pre4/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/IDEAL_30X_v1/0003/3AA6EEA4-3B16-DE11-B35F-001617C3B654.root'
))

process.MessageLogger = cms.Service("MessageLogger",
    debugModules = cms.untracked.vstring('*'),
    destinations = cms.untracked.vstring('cout'),
    cout = cms.untracked.PSet( threshold = cms.untracked.string('INFO'))
)


process.load('Configuration/StandardSequences/GeometryIdeal_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
#process.load('Configuration/StandardSequences/Reconstruction_cff')

process.GlobalTag.globaltag = 'IDEAL_30X::All'

process.load("RecoTracker.Configuration.RecoTracker_cff")
from RecoTracker.Configuration.RecoTracker_cff import *
process.load('RecoLocalTracker/Configuration/RecoLocalTracker_cff')
from RecoLocalTracker.Configuration.RecoLocalTracker_cff import *
process.siPixelClusters.src = cms.InputTag("simSiPixelDigis")
process.siStripZeroSuppression. RawDigiProducersList = cms.VInputTag(
    cms.InputTag('simSiStripDigis','VirginRaw'),
    cms.InputTag('simSiStripDigis','ProcessedRaw'),
    cms.InputTag('simSiStripDigis','ScopeMode'))
process.siStripClusters.DigiProducersList = cms.VInputTag(
    cms.InputTag('simSiStripDigis','ZeroSuppressed'),
    cms.InputTag('siStripZeroSuppression','VirginRaw'),
    cms.InputTag('siStripZeroSuppression','ProcessedRaw'),
    cms.InputTag('siStripZeroSuppression','ScopeMode'))


process.load("RecoTracker.IterativeTracking.FirstStep_cff")
from RecoTracker.IterativeTracking.FirstStep_cff import *
#newTrackCandidateMaker.src = cms.InputTag('globalPixelSeeds')
#newTrackCandidateMaker.src = cms.InputTag('hltL3TrajectorySeedFromL1')
#newTrackCandidateMaker.src = cms.InputTag('primSeeds')
#newTrackCandidateMaker.src = cms.InputTag('seedsFromProtoTracks')

#process.load("RecoMuon.TrackerSeedGenerator.TSGFromL1_cff")
process.load("RecoPixelVertexing.Configuration.RecoPixelVertexing_cff")

from RecoPixelVertexing.PixelTriplets.PixelTripletHLTGenerator_cfi import *
from RecoPixelVertexing.PixelTriplets.PixelTripletLargeTipGenerator_cfi import *
from RecoPixelVertexing.PixelTriplets.PixelTripletNoTipGenerator_cfi import *
from RecoPixelVertexing.PixelTrackFitting.PixelTracks_cfi import *

process.load("RecoPixelVertexing.PixelLowPtUtilities.firstStep_cff")
from RecoPixelVertexing.PixelLowPtUtilities.firstStep_cff import * 

process.load("RecoTracker.TkSeedGenerator.SeedGeneratorFromProtoTracksEDProducer_cfi")
from RecoTracker.TkSeedGenerator.SeedGeneratorFromProtoTracksEDProducer_cfi import *
seedsFromProtoTracks.InputCollection = cms.InputTag("pixel3PrimTracks")
#seedsFromProtoTracks.useProtoTrackKinematics = cms.bool(True)


#pixelTracks.OrderedHitsFactoryPSet.GeneratorPSet = cms.PSet( PixelTripletNoTipGenerator )
#pixelTracks.FilterPSet.ComponentName = cms.string("none")
#pixelTracks.CleanerPSet.ComponentName = cms.string("none")
#pixelTracks.RegionFactoryPSet.RegionPSet.ptMin = cms.double(0.04)

#pixelTracks.OrderedHitsFactoryPSet.GeneratorPSet = cms.PSet(
#            ComponentName = cms.string('TripletGenerator'),
#            checkClusterShape       = cms.bool(False),
#            checkMultipleScattering = cms.bool(True),
#            nSigMultipleScattering  = cms.double(5.0),
#            maxAngleRatio = cms.double(10.0),
#            rzTolerance   = cms.double(0.2),
#            TTRHBuilder   = cms.string('TTRHBuilderWithoutAngle4PixelTriplets')
#        )


pixelTracks.RegionFactoryPSet = cms.PSet(
        ComponentName = cms.string('GlobalTrackingRegionWithVerticesProducer'),
        RegionPSet = cms.PSet(
            precise       = cms.bool(True),
            beamSpot      = cms.InputTag("offlineBeamSpot"),
            originRadius  = cms.double(0.2),
            sigmaZVertex  = cms.double(3.0),
            useFixedError = cms.bool(True),
            fixedError    = cms.double(0.2),

            useFoundVertices = cms.bool(False),
            VertexCollection = cms.InputTag("pixelVertices"),
            ptMin            = cms.double(0.075),
            nSigmaZ          = cms.double(3.0)
        )
    ) 

pixelTracks.FilterPSet = cms.PSet( ComponentName = cms.string('ClusterShapeTrackFilter'), useFilter     = cms.bool(True))
pixelTracks.CleanerPSet = cms.PSet( ComponentName = cms.string('TrackCleaner'))
pixelTracks.FitterPSet = cms.PSet( ComponentName = cms.string('TrackFitter'), TTRHBuilder   = cms.string('TTRHBuilderWithoutAngle4PixelTriplets'))


#from RecoTracker.TkTrackingRegions.GlobalTrackingRegion_cfi import *
#process.pixelTracks.OrderedHitsFactoryPSet.GeneratorPSet.useFixedPreFiltering = cms.bool(True)
#process.pixelTracks.RegionFactoryPSet.RegionPSet.originRadius = cms.double(0.001)
#process.pixelTracks.RegionFactoryPSet.RegionPSet.originHalfLength = cms.double(0.001)

process.analysis = cms.EDFilter("TrackAnalysis",
  TrackCollection = cms.string("pixelTracks"),
#  TrackCollection = cms.string("pixel3ProtoTracks"),
#  TrackCollection = cms.string("pixel3PrimTracks"),
#   TrackCollection = cms.string("preFilterZeroStepTracks"),
#  TrackCollection = cms.string("generalTracks"),
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

#process.p = cms.Path(trackerlocalreco*pixel3ProtoTracks*pixelVertices*pixel3PrimTracks*primSeeds*newTrackCandidateMaker*preFilterZeroStepTracks*process.analysis)
process.p = cms.Path(pixeltrackerlocalreco*pixelTracks*process.analysis)
#process.p = cms.Path(pixeltrackerlocalreco*process.offlineBeamSpot*pixelTracks*process.analysis)
#process.p=cms.Path(process.trackerlocalreco*globalPixelSeeds*newTrackCandidateMaker*preFilterZeroStepTracks*process.analysis)


