import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")
process.load('Configuration/StandardSequences/Services_cff')
process.load('FWCore/MessageService/MessageLogger_cfi')
process.load('Configuration/StandardSequences/GeometryIdeal_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')

process.load("RecoTracker.Configuration.RecoTracker_cff")
from RecoTracker.Configuration.RecoTracker_cff import *

process.load('RecoLocalTracker/Configuration/RecoLocalTracker_cff')
from RecoLocalTracker.Configuration.RecoLocalTracker_cff import *
process.siPixelClusters.src = cms.InputTag("simSiPixelDigis")

#process.GlobalTag.globaltag = 'IDEAL_30X::All'

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10))

process.source = cms.Source ("PoolSource",fileNames = cms.untracked.vstring(
#'/store/relval/CMSSW_3_1_0_pre4/RelValSingleMuPt10/GEN-SIM-DIGI-RECO/IDEAL_30X_v1/0001/189AF476-A516-DE11-8357-001A92810AA8.root',
#  'file:SingleMuPt10_cfi_GEN_SIM_DIGI_L1_DIGI2RAW_RAW2DIGI_RECO.root'
 'file:SingleMu_10m.root'
))

process.simplelayers = cms.ESProducer("SimpleLayersESProducer",
  ComponentName = cms.string('SimpleLayers'),
  layerList = cms.vstring('BPix1','BPix2'),
  BPix = cms.PSet(
    useErrorsFromParam = cms.untracked.bool(True),
    hitErrorRPhi = cms.double(0.0027),
    TTRHBuilder = cms.string('TTRHBuilderWithoutAngle4PixelPairs'),
    HitProducer = cms.string('siPixelRecHits'),
    hitErrorRZ = cms.double(0.006)
  )
)

process.analysis = cms.EDFilter("OrderedHitsAnalysis",
  RegionFactoryPSet = cms.PSet(
    ComponentName = cms.string("MyRegions"),
    RegionPSet = cms.PSet(
      originXPos = cms.double(0.0),
      originYPos = cms.double(0.0),
      originZPos = cms.double(0.0),
      originRadius = cms.double(0.1),
      originHalfLength = cms.double(0.1),
      precise = cms.bool(True)
    ),
    deltaR = cms.vdouble(-1.),
#    deltaR = cms.vdouble(1.e-4, 0.05, 0.1, 0.2, 0.5, 0.7, -1.),
    useParticleVertex = cms.bool(True),
    useParticleId = cms.int32(13),
    ptMinLeadingTrack = cms.double(0.9)
  ),

  OrderedHitsFactoryPSet = cms.PSet(
#    ComponentName = cms.string("SimpleSingleHitGenerator"),
#    SeedingLayers = cms.string("SimpleLayers")
    ComponentName = cms.string("StandardHitPairGenerator"),
    SeedingLayers = cms.string("PixelLayerPairs")
  ),

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
     ptMinLeadingTrack = cms.double(0.9)
  )
)


#process.p = cms.Path(process.siPixelClusters*process.siPixelRecHits)
process.p = cms.Path(process.pixeltrackerlocalreco*process.analysis)


