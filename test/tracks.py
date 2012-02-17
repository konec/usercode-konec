import FWCore.ParameterSet.Config as cms
import sys

process = cms.Process("Analysis")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(19))

process.source = cms.Source("PoolSource", fileNames =  cms.untracked.vstring( 
#'file:data/Single_TkVpt9_r00_p_nev10000.root',
#'file:data/Single_TkVpt1.0_r00_m_nev10000.root',
# '/store/PTB_Param52/Single_TkVpt5_r07_m_nev1000.root'
# 'file:out.root'
 '/store/PTB_Param52/Single_TkVpt2.0_r08_m_nev50000.root'
))

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.GlobalTag.globaltag = 'MC_52_V1::All'

#process.load("TrackingTools.MaterialEffects.Propagators_cff")

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
#from RecoPixelVertexing.PixelTrackFitting.PixelTracks_cff import *
#from RecoPixelVertexing.PixelTrackFitting.PixelFitterByConformalMappingAndLine_cfi import *
process.load("RecoPixelVertexing.PixelTrackFitting.KFBasedPixelFitter")


BBlock = cms.PSet(
  ComponentName = cms.string('GlobalRegionProducerFromBeamSpot'),
  RegionPSet = cms.PSet(
    precise = cms.bool(True),
    originRadius = cms.double(0.2),
    nSigmaZ = cms.double(3.0),
    ptMin = cms.double(0.5),
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
     originXPos = cms.double(0.25),
     originYPos = cms.double(0.4),
     originZPos = cms.double(0.0)
  )
)


#FitterPSet =  cms.PSet (process.PixelFitterByHelixProjections)
FitterPSet =  cms.PSet (process.KFBasedPixelFitter)
#FitterPSet.propagator = cms.string('PropagatorWithMaterial')
#FitterPSet.propagator = cms.string('PropagatorWithMaterialOpposite')
#FitterPSet.propagator = cms.string('AnyDirectionAnalyticalPropagator')
#FitterPSet =  cms.PSet (process.PixelFitterByHelixProjections)

#FitterPSet = cms.PSet(
#  ComponentName = cms.string('PixelFitterByConformalMappingAndLine'),
#  fixImpactParameter = cms.double(0.),
#  ComponentName = cms.string('PixelFitterByHelixProjections'),
#  ComponentName = cms.string('KFBasedPixelFitter'),
#  propagator = cms.string('PropagatorWithMaterial'),
#  TTRHBuilder   = cms.string('TTRHBuilderWithoutAngle4PixelTriplets')
#)



process.pixelTracks.RegionFactoryPSet= cms.PSet( BBlock )
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
     ptMinLeadingTrack = cms.double(0.5)
  )
)


process.p=cms.Path(process.siPixelDigis*process.pixeltrackerlocalreco*process.offlineBeamSpot*process.pixelTracks*process.analysis)
