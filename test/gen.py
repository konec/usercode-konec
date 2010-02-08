process = cms.Process('Gen-Sim-Digi-L1Reco')

# import of standard configurations
process.load('Configuration/StandardSequences/Services_cff')
process.load('FWCore/MessageService/MessageLogger_cfi')
process.load('Configuration/StandardSequences/MixingNoPileUp_cff')
process.load('Configuration/StandardSequences/GeometryIdeal_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
process.load('Configuration/StandardSequences/Generator_cff')
# process.load('Configuration/StandardSequences/VtxSmearedGauss_cff')
process.load('Configuration/StandardSequences/VtxSmearedFlat_cff')
process.load('Configuration/StandardSequences/Sim_cff')
process.load('Configuration/StandardSequences/Digi_cff')

process.load('Configuration/StandardSequences/SimL1Emulator_cff')
process.load('Configuration/StandardSequences/DigiToRaw_cff')

process.load("RecoTracker/Configuration/RecoTracker_cff")
process.load('RecoLocalTracker/Configuration/RecoLocalTracker_cff')
process.load('RecoLocalMuon/Configuration/RecoLocalMuon_cff')
process.load('RecoVertex/BeamSpotProducer/BeamSpot_cff')

process.load('Configuration/StandardSequences/EndOfProcess_cff')
process.load('Configuration/EventContent/EventContent_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag ='STARTUP3X_V8N::All'

process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.1 $'),
    annotation = cms.untracked.string('SingleMuPt10.cfi nevts:10'),
    name = cms.untracked.string('PyReleaseValidation')
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1000)
)

# Output definition
process.out = cms.OutputModule("PoolOutputModule",
   outputCommands = process.FEVTDEBUGEventContent.outputCommands,
#    outputCommands =  cms.untracked.vstring('keep *'),
    fileName = cms.untracked.string('SingleMu_10m.root'),
)

process.generator = cms.EDProducer("FlatRandomPtGunProducer",
    PGunParameters = cms.PSet(
        MaxPt = cms.double(3.0751),
        MinPt = cms.double(3.0750),
        PartID = cms.vint32(13),
        MaxEta = cms.double(2.0),
        MaxPhi = cms.double(3.14159265359),
        MinEta = cms.double(-2.0),
        MinPhi = cms.double(-3.14159265359)
    ),
    Verbosity = cms.untracked.int32(0),
    psethack = cms.string('single mu pt 10'),
    AddAntiParticle = cms.bool(True),
    firstRun = cms.untracked.uint32(1)
)

process.VtxSmeared.MinX = cms.double( 0.0)
process.VtxSmeared.MinY = cms.double( 0.0)
process.VtxSmeared.MinZ = cms.double( -0.617)
process.VtxSmeared.MaxX = cms.double( 0.0)
process.VtxSmeared.MaxY = cms.double( 0.0)
process.VtxSmeared.MaxZ = cms.double( -0.617)

# Input source
process.source = cms.Source("EmptySource")


process.p = cms.Path(process.generator*process.pgen*process.psim*process.mix*process.pdigi*process.SimL1Emulator*process.DigiToRaw*process.offlineBeamSpot*process.endOfProcess*process.out)
