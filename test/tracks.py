import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10))
process.source = cms.Source("PoolSource", fileNames =  cms.untracked.vstring(
#     'file:data/SingleMuL1_10m.root'
#'file:SingleMuPt10_cfi_GEN_SIM_DIGI_L1_DIGI2RAW_RAW2DIGI_RECO.root'
'file:SingleMu_10m.root'
)
)

process.MessageLogger = cms.Service("MessageLogger",
    debugModules = cms.untracked.vstring('*'),
    destinations = cms.untracked.vstring('cout'),
    cout = cms.untracked.PSet( threshold = cms.untracked.string('INFO'))
)

#process.load("Configuration.StandardSequences.Geometry_cff")
#process.load("Geometry.TrackerSimData.trackerSimGeometryXML_cfi")
#process.load("Geometry.TrackerGeometryBuilder.trackerGeometry_cfi")
#process.load("Geometry.TrackerNumberingBuilder.trackerNumberingGeometry_cfi")
#process.load("Configuration.StandardSequences.MagneticField_cff")

process.load('Configuration/StandardSequences/DigiToRaw_cff')
process.load('Configuration/StandardSequences/RawToDigi_cff')
process.load('Configuration/StandardSequences/GeometryIdeal_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
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
newTrackCandidateMaker.src = cms.InputTag('globalPixelSeeds')
#newTrackCandidateMaker.src = cms.InputTag('hltL3TrajectorySeedFromL1')


#process.load("RecoMuon.TrackerSeedGenerator.TSGFromL1_cff")
#process.load("RecoPixelVertexing.PixelTrackFitting.PixelTracks_cff")
#process.load("RecoPixelVertexing.Configuration.RecoPixelVertexing_cff")

#from RecoPixelVertexing.PixelTriplets.PixelTripletHLTGenerator_cfi import *
#from RecoPixelVertexing.PixelTrackFitting.PixelTracks_cfi import *
#from RecoTracker.TkTrackingRegions.GlobalTrackingRegion_cfi import *
#from RecoPixelVertexing.PixelTriplets.PixelTripletLargeTipGenerator_cfi import *
#process.pixelTracks.OrderedHitsFactoryPSet.GeneratorPSet = cms.PSet( PixelTripletLargeTipGenerator )


#process.pixelTracks.OrderedHitsFactoryPSet.GeneratorPSet.useFixedPreFiltering = cms.bool(True)
#process.pixelTracks.RegionFactoryPSet.RegionPSet.ptMin = cms.double(1.00)
#process.pixelTracks.RegionFactoryPSet.RegionPSet.originRadius = cms.double(0.001)
#process.pixelTracks.RegionFactoryPSet.RegionPSet.originHalfLength = cms.double(0.001)

process.analysis = cms.EDFilter("TrackAnalysis",
#  TrackCollection = cms.string("pixelTracks"),
#  TrackCollection = cms.string("generalTracks"),
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
     ptMinLeadingTrack = cms.double(0.9)
  )
)


#process.p = cms.Path(pixeltrackerlocalreco+process.pixelTracks+process.analysis)
#process.p = cms.Path(trackerlocalreco+process.recopixelvertexing*ckftracks+process.analysis)
#process.p=cms.Path(trackerlocalreco+process.hltL3TrajectorySeedFromL1*newTrackCandidateMaker*preFilterZeroStepTracks*process.analysis)
#process.p=cms.Path(process.trackerlocalreco*globalPixelSeeds*newTrackCandidateMaker*preFilterZeroStepTracks*process.analysis)
process.p=cms.Path(process.trackerlocalreco*globalPixelSeeds*newTrackCandidateMaker*preFilterZeroStepTracks*process.analysis)


