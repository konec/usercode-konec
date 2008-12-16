import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1))
process.source = cms.Source("PoolSource", fileNames =  cms.untracked.vstring(
      'file:data/SingleMuL1_1000m_B.root' 
#     'file:data/SingleMuL1_6p.root',
#     'file:data/SingleMuL1_7p.root',
#     'file:data/SingleMuL1_8p.root',
#     'file:data/SingleMuL1_9p.root',
#     'file:data/SingleMuL1_10p.root',
#     'file:data/SingleMuL1_12p.root',
#     'file:data/SingleMuL1_15p.root',
#     'file:data/SingleMuL1_20p.root',
#     'file:data/SingleMuL1_25p.root',
#     'file:data/SingleMuL1_30p.root',
#     'file:data/SingleMuL1_40p.root',
#     'file:data/SingleMuL1_50p.root',
#     'file:data/SingleMuL1_60p.root',
#     'file:data/SingleMuL1_80p.root',
#     'file:data/SingleMuL1_100p.root'
  )
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
from RecoLocalTracker.Configuration.RecoLocalTracker_cff import *
#process.siPixelClusters.src = cms.InputTag('simSiPixelDigis')

process.load("RecoTracker.Configuration.RecoTracker_cff")
from RecoTracker.Configuration.RecoTracker_cff import *

process.load("RecoPixelVertexing.PixelTrackFitting.PixelTracks_cff")
process.load("RecoPixelVertexing.Configuration.RecoPixelVertexing_cff")


#from RecoPixelVertexing.PixelTriplets.PixelTripletHLTGenerator_cfi import *
#from RecoPixelVertexing.PixelTrackFitting.PixelTracks_cfi import *
#from RecoTracker.TkTrackingRegions.GlobalTrackingRegion_cfi import *
#from RecoPixelVertexing.PixelTriplets.PixelTripletLargeTipGenerator_cfi import *
#process.pixelTracks.OrderedHitsFactoryPSet.GeneratorPSet = cms.PSet( PixelTripletLargeTipGenerator )

#process.pixelTracks.OrderedHitsFactoryPSet.GeneratorPSet.useFixedPreFiltering = cms.bool(True)
#process.pixelTracks.RegionFactoryPSet.RegionPSet.ptMin = cms.double(1.00)
#process.pixelTracks.RegionFactoryPSet.RegionPSet.originRadius = cms.double(0.001)
#process.pixelTracks.RegionFactoryPSet.RegionPSet.originHalfLength = cms.double(0.001)

process.l1seeding = cms.EDFilter("L1Seeding",

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


#process.p = cms.Path(pixeltrackerlocalreco+process.pixelTracks+process.analysis)
#process.p = cms.Path(trackerlocalreco+process.recopixelvertexing*ckftracks+process.analysis)
process.p=cms.Path(trackerlocalreco+process.l1seeding)


