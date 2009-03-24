import FWCore.ParameterSet.Config as cms

process = cms.Process("MYRUN")

process.load("EventFilter.RPCRawToDigi.RPCSQLiteCabling_cfi")
process.load("EventFilter.RPCRawToDigi.rpcUnpacker_cfi")

process.load("DQM.RPCMonitorClient.RPCMonitorRaw_cfi")
process.load("DQM.RPCMonitorClient.RPCMonitorLinkSynchro_cfi")
#process.rpcMonitorLinkSynchro.dumpDelays = cms.untracked.bool(True)

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

# set maxevents; -1 -> take all
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10000))

process.source = cms.Source ("PoolSource",fileNames = cms.untracked.vstring(
#'file:out.root'
'/store/data/Commissioning09/Cosmics/RAW/v1/000/077/023/6C75241D-3A09-DE11-BE1B-001D09F29597.root'
))

# correct output file
process.MessageLogger = cms.Service("MessageLogger",
    debugModules = cms.untracked.vstring('rpcMonitorLinkSynchro'),
    #debugModules = cms.untracked.vstring('*'),
    destinations = cms.untracked.vstring('cout'),
    cout = cms.untracked.PSet( threshold = cms.untracked.string('DEBUG'))
)

process.out = cms.OutputModule("PoolOutputModule",
    fileName =  cms.untracked.string('file:out.root'),
    outputCommands = cms.untracked.vstring("keep *")
)

process.cscFilter = cms.EDFilter("CSCFilter")

process.p = cms.Path(process.l1GtUnpack*process.rpcunpacker*process.rpcMonitorRaw*process.rpcMonitorLinkSynchro*process.l1trpctf*process.dqmEnv*process.dqmSaver)
#process.ep = cms.EndPath(process.out)
