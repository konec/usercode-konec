import FWCore.ParameterSet.Config as cms

process = cms.Process("MYRUN")

process.load("EventFilter.RPCRawToDigi.RPCSQLiteCabling_cfi")
process.RPCCabling.connect = 'sqlite_file:RPCEMap.db'
process.load("EventFilter.RPCRawToDigi.rpcUnpacker_cfi")


process.load("DQM.RPCMonitorClient.RPCMonitorRaw_cfi")
process.load("DQM.RPCMonitorClient.RPCMonitorLinkSynchro_cfi")
process.rpcMonitorLinkSynchro.dumpDelays = cms.untracked.bool(True)

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
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1))

process.source = cms.Source ("PoolSource",fileNames = cms.untracked.vstring(
#'file:out.root'
#'/store/data/Commissioning09/Cosmics/RAW/v1/000/077/023/6C75241D-3A09-DE11-BE1B-001D09F29597.root'
# 'file:/disk00/work/data/run121964/ZeroBias.root'
#  'file:/disk00/konec/work/data/skimMuons_collisions_6to9Dec_RECO.root'
# 'file:/disk00/work/data/run121993/Cosmics.root'
# 9  '/store//data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/970/C25D74CA-F8E5-DE11-A151-003048D2C020.root'
# 7 '/store//data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/970/1497FE11-F1E5-DE11-8FCC-001D09F28D4A.root'
#14 '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/970/1E985F6D-F2E5-DE11-B742-0030487D0D3A.root'
#18'/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/970/2E543228-F3E5-DE11-A825-000423D98E6C.root'
#'/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/970/406212E4-F0E5-DE11-A09C-000423D99AAA.root'
#17'/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/970/685BA528-F3E5-DE11-8F2F-0030487C6062.root'
#3'/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/970/689CEDE7-F0E5-DE11-9D0A-001617C3B66C.root'
'/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/970/809B636D-F2E5-DE11-AB8B-001D09F23174.root'

))

# correct output file
process.MessageLogger = cms.Service("MessageLogger",
    #debugModules = cms.untracked.vstring('rpcMonitorLinkSynchro'),
    debugModules = cms.untracked.vstring('rpcunpacker'),
    #debugModules = cms.untracked.vstring('*'),
    destinations = cms.untracked.vstring('cout'),
    cout = cms.untracked.PSet( threshold = cms.untracked.string('DEBUG'))
)

process.out = cms.OutputModule("PoolOutputModule",
    fileName =  cms.untracked.string('file:out.root'),
    outputCommands = cms.untracked.vstring("keep *")
)

process.filter = cms.EDFilter("FilterL1")

process.p = cms.Path(process.l1GtUnpack*process.filter*process.rpcunpacker
*process.rpcMonitorRaw *process.rpcMonitorLinkSynchro
*process.l1trpctf*process.dqmEnv*process.dqmSaver)
#process.ep = cms.EndPath(process.out)
