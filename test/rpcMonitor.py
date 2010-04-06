import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1))
process.source = cms.Source("PoolSource", fileNames =  cms.untracked.vstring( 
'file:/disk00/work/data/run132440.muon/208A5482-4D3C-DF11-97EA-00E08178C0FB.root',
'file:/disk00/work/data/run132440.muon/BC463F24-4A3C-DF11-998A-00E08178C085.root',
'file:/disk00/work/data/run132440.muon/963F1CB1-443C-DF11-B5F1-003048673F1C.root',
'file:/disk00/work/data/run132440.muon/86912F13-4C3C-DF11-9CEB-00E0817918BF.root'
#'file:/disk00/work/data/run132440.tmf/run132440_158_167.root',
#'file:/disk00/work/data/run132440.tmf/run132440_168_177.root',
#'file:/disk00/work/data/run132440.tmf/run132440_178_187.root',
#'file:/disk00/work/data/run132440.tmf/run132440_188_197.root',
#'file:/disk00/work/data/run132440.tmf/run132440_198_207.root',
#'file:/disk00/work/data/run132440.tmf/run132440_208_217.root',
#'file:/disk00/work/data/run132440.tmf/run132440_218_227.root',
#'file:/disk00/work/data/run132440.tmf/run132440_228_237.root',
#'file:/disk00/work/data/run132440.tmf/run132440_238_247.root',
#'file:/disk00/work/data/run132440.tmf/run132440_248_257.root',
#'file:/disk00/work/data/run132440.tmf/run132440_258_267.root',
#'file:/disk00/work/data/run132440.tmf/run132440_268_277.root',
#'file:/disk00/work/data/run132440.tmf/run132440_278_287.root',
#'file:/disk00/work/data/run132440.tmf/run132440_288_297.root',
#'file:/disk00/work/data/run132440.tmf/run132440_298_307.root',
#'file:/disk00/work/data/run132440.tmf/run132440_308_317.root',
#'file:/disk00/work/data/run132440.tmf/run132440_318_327.root',
#'file:/disk00/work/data/run132440.tmf/run132440_328_337.root',
#'file:/disk00/work/data/run132440.tmf/run132440_338_347.root',
#'file:/disk00/work/data/run132440.tmf/run132440_348_357.root' 
))


# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
#process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.GeometryExtended_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
#process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
#process.GlobalTag.globaltag = 'MC_36Y_V2::All'
process.GlobalTag.globaltag = 'START36_V3::All'
#process.GlobalTag.globaltag = 'GR10_E_V4::All'

process.load("EventFilter.RPCRawToDigi.rpcUnpacker_cfi")

process.load("DQM.RPCMonitorClient.RPCFEDIntegrity_cfi")
process.load("DQM.RPCMonitorClient.RPCMonitorRaw_cfi")
process.rpcMonitorRaw.writeHistograms = cms.untracked.bool(True)
process.rpcMonitorRaw.histoFileName = cms.untracked.string("histos1.root")

process.load("DQM.RPCMonitorClient.RPCMonitorLinkSynchro_cfi")
process.rpcMonitorLinkSynchro.dumpDelays = cms.untracked.bool(True)
process.rpcMonitorLinkSynchro.writeHistograms = cms.untracked.bool(True)
process.rpcMonitorLinkSynchro.useFirstHitOnly = cms.untracked.bool(True)
process.rpcMonitorLinkSynchro.histoFileName = cms.untracked.string("histos2.root")

process.filter = cms.EDFilter("SynchroFilter")

process.load("EventFilter.L1GlobalTriggerRawToDigi.l1GtUnpack_cfi")
process.l1GtUnpack.DaqGtInputTag = cms.InputTag("source")
process.load("DQM.L1TMonitor.L1TRPCTF_cfi")
process.l1trpctf.rpctfSource =  cms.InputTag("l1GtUnpack")

process.load("L1TriggerConfig.L1GtConfigProducers.L1GtConfig_cff")
process.load("L1TriggerConfig.GMTConfigProducers.L1MuGMTParametersConfig_cff")
process.load("L1TriggerConfig.L1ScalesProducers.L1MuTriggerScalesConfig_cff")
process.load("L1TriggerConfig.L1ScalesProducers.L1MuTriggerPtScaleConfig_cff")
process.load("L1TriggerConfig.L1ScalesProducers.L1MuGMTScalesConfig_cff")

process.load("DQMServices.Core.DQM_cfg")
process.load("DQMServices.Components.test.dqm_onlineEnv_cfi")
process.dqmSaver.convention = 'Online'
process.dqmEnv.subSystemFolder = 'R2DTEST'

process.MessageLogger = cms.Service("MessageLogger",
    #debugModules = cms.untracked.vstring('pixelVertices'),
    #debugModules = cms.untracked.vstring('pixelTracks'),
    #debugModules = cms.untracked.vstring('rpcunpacker'),
    #debugModules = cms.untracked.vstring('*'),
    debugModules = cms.untracked.vstring(''),
    destinations = cms.untracked.vstring('cout'),
    cout = cms.untracked.PSet( threshold = cms.untracked.string('INFO'))
)

process.p = cms.Path(
     process.l1GtUnpack
    *process.filter
    *process.rpcunpacker
    *process.rpcFEDIntegrity
    *process.rpcMonitorRaw
    *process.rpcMonitorLinkSynchro
    *process.l1trpctf
*process.dqmEnv*process.dqmSaver)

#process.p=cms.Path(process.siPixelDigis)
#process.p=cms.Path(process.rpcunpacker)
#process.p=cms.Path(process.RawToDigi)
#process.p=cms.Path(process.rpcunpacker*process.analysis)
#process.p=cms.Path(process.RawToDigi*process.pixeltrackerlocalreco*process.pixelTracks*process.analysis)
#process.p=cms.Path(process.siPixelDigis*process.pixeltrackerlocalreco*process.offlineBeamSpot*process.pixelTracks*process.pixelVertices*process.analysis)
