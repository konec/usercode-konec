import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1))
process.source = cms.Source("PoolSource", fileNames =  cms.untracked.vstring(
     'file:data/cmssw20x/SingleMu_10m.root')
)

process.MessageLogger = cms.Service("MessageLogger",
    debugModules = cms.untracked.vstring('*'),
    destinations = cms.untracked.vstring('cout'),
    cout = cms.untracked.PSet( threshold = cms.untracked.string('INFO'))
)

process.load("Configuration.StandardSequences.Geometry_cff")
#process.load("Geometry.TrackerSimData.trackerSimGeometryXML_cfi")
#process.load("Geometry.TrackerGeometryBuilder.trackerGeometry_cfi")
#process.load("Geometry.TrackerNumberingBuilder.trackerNumberingGeometry_cfi")
process.load("Configuration.StandardSequences.FakeConditions_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

process.load("RecoLocalTracker.Configuration.RecoLocalTracker_cff")
process.load("RecoTracker.Configuration.RecoTracker_cff")
process.load("RecoPixelVertexing.PixelTrackFitting.PixelTracks_cff")

from RecoLocalTracker.Configuration.RecoLocalTracker_cff import *
#process.siPixelClusters.src = cms.InputTag('simSiPixelDigis')

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
    deltaR = cms.vdouble(1.e-4, 0.05, 0.1, 0.2, 0.5, 0.7, -1.),
    useParticleVertex = cms.bool(True),
    useParticleId = cms.int32(13),
    ptMinLeadingTrack = cms.double(0.9)   
  ),

  OrderedHitsFactoryPSet = cms.PSet(
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


process.p = cms.Path(pixeltrackerlocalreco+process.analysis)

