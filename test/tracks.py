import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(7))
#process.source = cms.Source("PoolSource", fileNames =  cms.untracked.vstring( 'file:DoubleMu_3_xy.root'))
process.source = cms.Source("PoolSource", fileNames =  cms.untracked.vstring( 'file:input.root'))

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
#process.GlobalTag.globaltag = 'IDEAL_31X::All'
#process.GlobalTag.globaltag = 'STARTUP31X_V1::All'
#process.GlobalTag.globaltag = 'MC_31X_V9::All'
#process.GlobalTag.globaltag ='STARTUP3X_V8N::All'
#process.GlobalTag.globaltag = 'MC_36Y_V10::All'
process.GlobalTag.globaltag = 'MC_38Y_V9::All'



process.MessageLogger = cms.Service("MessageLogger",
    #debugModules = cms.untracked.vstring('pixelVertices'),
    #debugModules = cms.untracked.vstring('pixelTracks'),
    #debugModules = cms.untracked.vstring('*'),
    debugModules = cms.untracked.vstring(''),
    destinations = cms.untracked.vstring('cout'),
    cout = cms.untracked.PSet( threshold = cms.untracked.string('DEBUG'))
)

process.load("RecoTracker.Configuration.RecoTracker_cff")
from RecoTracker.Configuration.RecoTracker_cff import *
process.load('RecoLocalTracker/Configuration/RecoLocalTracker_cff')
process.load("RecoPixelVertexing.PixelTrackFitting.PixelTracks_cff")
from RecoPixelVertexing.PixelTrackFitting.PixelTracks_cff import *
from RecoPixelVertexing.PixelTrackFitting.PixelFitterByConformalMappingAndLine_cfi import *
process.load("RecoPixelVertexing.PixelTrackFitting.KFBasedPixelFitter")


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

FitterPSet =  cms.PSet (process.KFBasedPixelFitter)

#FitterPSet = cms.PSet(
#  ComponentName = cms.string('PixelFitterByConformalMappingAndLine'),
#  fixImpactParameter = cms.double(0.),
#  ComponentName = cms.string('PixelFitterByHelixProjections'),
#  ComponentName = cms.string('KFBasedPixelFitter'),
#  propagator = cms.string('PropagatorWithMaterial'),
#  TTRHBuilder   = cms.string('TTRHBuilderWithoutAngle4PixelTriplets')
#)



process.pixelTracks.RegionFactoryPSet= cms.PSet( GBlock )
process.pixelTracks.FilterPSet.ComponentName = cms.string('none')
process.pixelTracks.CleanerPSet.ComponentName = cms.string('none')
process.pixelTracks.OrderedHitsFactoryPSet.GeneratorPSet = cms.PSet ( PixelTripletHLTGenerator )
process.pixelTracks.FitterPSet = cms.PSet(FitterPSet)


process.analysis = cms.EDAnalyzer("TrackAnalysis",
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

process.p=cms.Path(process.siPixelRecHits*process.pixelTracks*process.analysis)
#process.p=cms.Path(process.siPixelDigis*process.pixeltrackerlocalreco*process.pixelTracks*process.analysis)
#process.p=cms.Path(process.siPixelDigis*process.pixeltrackerlocalreco*process.offlineBeamSpot*process.pixelTracks*process.pixelVertices*process.analysis)
