import FWCore.ParameterSet.Config as cms

process = cms.Process('Gen-Sim-Digi-L1Reco-Raw')
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.MixingNoPileUp_cff')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.Generator_cff')
#process.load('Configuration.StandardSequences.VtxSmearedEarly10TeVCollision_cff')
#process.load('Configuration/StandardSequences/VtxSmearedGauss_cff')
process.load('Configuration/StandardSequences/VtxSmearedFlat_cff')
#process.load('Configuration/StandardSequences/Sim_cff')
process.load('Configuration.StandardSequences.SimIdeal_cff')
process.load('Configuration.StandardSequences.Digi_cff')
process.load('Configuration.StandardSequences.SimL1Emulator_cff')
process.load('Configuration.StandardSequences.DigiToRaw_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('Configuration.EventContent.EventContent_cff')
#process.GlobalTag.globaltag = 'MC_36Y_V2::All'
process.GlobalTag.globaltag = 'START36_V3::All'


process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10))

process.source = cms.Source("EmptySource")

process.output = cms.OutputModule("PoolOutputModule",
    outputCommands = process.FEVTDEBUGHLTEventContent.outputCommands,
    fileName = cms.untracked.string('output.root'),
    SelectEvents = cms.untracked.PSet( SelectEvents = cms.vstring('generation_step'))
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
    psethack = cms.string('double mu'),
    AddAntiParticle = cms.bool(True),
    firstRun = cms.untracked.uint32(1)
)

process.VtxSmeared.MinX = cms.double( 0.0)
process.VtxSmeared.MinY = cms.double( 0.0)
process.VtxSmeared.MinZ = cms.double( -2.5)
process.VtxSmeared.MaxX = cms.double( 0.0)
process.VtxSmeared.MaxY = cms.double( 0.0)
process.VtxSmeared.MaxZ = cms.double( -2.5)


# Path and EndPath definitions
process.generation_step = cms.Path(process.pgen)
process.simulation_step = cms.Path(process.psim)
process.digitisation_step = cms.Path(process.pdigi)
process.L1simulation_step = cms.Path(process.SimL1Emulator)
process.digi2raw_step = cms.Path(process.DigiToRaw)
process.raw2digi_step = cms.Path(process.RawToDigi)
process.L1Reco_step = cms.Path(process.L1Reco)
process.endjob_step = cms.Path(process.endOfProcess)
process.out_step = cms.EndPath(process.output)


process.schedule = cms.Schedule(process.generation_step,process.simulation_step,process.digitisation_step,process.L1simulation_step,process.digi2raw_step)
process.schedule.extend([process.raw2digi_step,process.L1Reco_step,process.endjob_step,process.out_step])
# special treatment in case of production filter sequence
for path in process.paths:
    getattr(process,path)._seq = process.generator*getattr(process,path)._seq



