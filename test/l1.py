import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1))
process.source = cms.Source("PoolSource", fileNames =  cms.untracked.vstring( 'file:out.root' ))

# import of standard configurations
process.load('Configuration/StandardSequences/Services_cff')
process.load('FWCore/MessageService/MessageLogger_cfi')
#process.MessageLogger = cms.Service("MessageLogger",
#    debugModules = cms.untracked.vstring('*'),
#    destinations = cms.untracked.vstring('cout'),
#    cout = cms.untracked.PSet( threshold = cms.untracked.string('INFO'))
#)
process.load('Configuration/StandardSequences/MixingNoPileUp_cff')
process.load('Configuration/StandardSequences/GeometryExtended_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
process.load('Configuration/StandardSequences/RawToDigi_cff')
process.load('Configuration/StandardSequences/Reconstruction_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load('Configuration/EventContent/EventContent_cff')

process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.118 $'),
    annotation = cms.untracked.string('step2 nevts:1'),
    name = cms.untracked.string('PyReleaseValidation')
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

# Other statements
process.mix.playback = True
process.GlobalTag.globaltag = 'IDEAL_31X::All'

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

# Path and EndPath definitions
process.p=cms.Path(process.RawToDigi*process.reconstruction*process.l1seeding)
#process.raw2digi_step = cms.Path(process.RawToDigi)
#process.reconstruction_step = cms.Path(process.reconstruction)

# Schedule definition
#process.schedule = cms.Schedule(process.raw2digi_step,process.reconstruction_step)

#process.p = cms.Path(pixeltrackerlocalreco+process.pixelTracks+process.analysis)
#process.p = cms.Path(trackerlocalreco+process.recopixelvertexing*ckftracks+process.analysis)
#process.p=cms.Path(trackerlocalreco+process.l1seeding)


