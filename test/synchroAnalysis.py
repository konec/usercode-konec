import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")


process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1))
process.source = cms.Source("PoolSource", fileNames =  cms.untracked.vstring( 
  'file:/disk00/work/CMSSW_4_2_3.RPC/jobs_gen/SingleMu_2.root')
)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryExtended_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
#process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'MC_42_V12::All'

#
# privare rpc r2d
#
#process.load("EventFilter.RPCRawToDigi.RPCSQLiteCabling_cfi")
#process.RPCCabling.connect = 'sqlite_file:RPCEMap2.db'
#process.es_prefer_RPCEMap = cms.ESPrefer("PoolDBESSource","RPCCabling");

process.load("L1TriggerConfig.L1GtConfigProducers.L1GtConfig_cff")
process.load("L1TriggerConfig.GMTConfigProducers.L1MuGMTParametersConfig_cff")
process.load("L1TriggerConfig.L1ScalesProducers.L1MuTriggerScalesConfig_cff")
process.load("L1TriggerConfig.L1ScalesProducers.L1MuTriggerPtScaleConfig_cff")
process.load("L1TriggerConfig.L1ScalesProducers.L1MuGMTScalesConfig_cff")

#
# rpc emulator 
#
process.load("L1TriggerConfig.RPCTriggerConfig.L1RPCConfig_cff")
process.load("L1TriggerConfig.RPCTriggerConfig.RPCConeDefinition_cff")
process.load("L1TriggerConfig.RPCTriggerConfig.RPCHwConfig_cff")
process.l1RPCHwConfig.firstBX = cms.int32(0)
process.l1RPCHwConfig.lastBX = cms.int32(0)
process.load("L1Trigger.RPCTrigger.RPCConeConfig_cff")
process.load("L1Trigger.RPCTrigger.l1RpcEmulDigis_cfi")
process.l1RpcEmulDigis.nBxToProcess = cms.int32(5)


#
# message logger
#
process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger = cms.Service("MessageLogger",
  debugModules = cms.untracked.vstring(),
  destinations = cms.untracked.vstring('cout'),
  cout = cms.untracked.PSet( threshold = cms.untracked.string('DEBUG'))
)
process.MessageLogger.debugModules.append('muonRPCDigis')
process.MessageLogger.debugModules.append('rpcMonitorLinkSynchro')
process.MessageLogger.debugModules.append('linkSynchroAnalysis')

#
# DQM modules
#
process.load("DQMServices.Core.DQM_cfg")
process.load("DQMServices.Components.DQMEnvironment_cfi")
process.DQMStore = cms.Service("DQMStore")

process.load("DQM.L1TMonitor.L1TDEMON_cfi")
process.l1demon.disableROOToutput = cms.untracked.bool(False)
process.load("L1Trigger.HardwareValidation.L1HardwareValidation_cff")
process.l1compare.RPCsourceEmul = cms.InputTag("l1RpcEmulDigis")
process.l1compare.COMPARE_COLLS = cms.untracked.vuint32(0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0)
process.l1compare.VerboseFlag   = 0
process.l1compare.DumpMode      = -1

process.load("DQM.L1TMonitor.L1TRPCTF_cfi")
process.l1trpctf.rpctfRPCDigiSource =  cms.InputTag("muonRPCDigis","DQM",)
process.l1trpctf.rpctfSource =  cms.InputTag("gtDigis")

process.load("DQM.RPCMonitorClient.RPCFEDIntegrity_cfi")
process.load("DQM.RPCMonitorClient.RPCMonitorRaw_cfi")
process.rpcMonitorRaw.writeHistograms = cms.untracked.bool(True)
process.rpcMonitorRaw.histoFileName = cms.untracked.string("rawMonitor.root")

# process.load("RecoMuon.GlobalTrackingTools.GlobalMuonRefitter_cff")
process.load("RecoMuon.GlobalTrackingTools.GlobalTrackQuality_cfi")
process.load("RecoMuon.MuonIdentification.refitMuons_cfi")
process.load("TrackingTools.TrackRefitter.globalMuonTrajectories_cff")



#process.filterBX = cms.EDFilter("FilterBX", beamBX = cms.vuint32(1,100) )

process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
  vertexCollection = cms.InputTag('offlinePrimaryVertices'),
  minimumNDOF = cms.uint32(4) ,
  maxAbsZ = cms.double(15),	
  maxd0 = cms.double(2)	
)

process.filterL1 = cms.EDFilter("FilterL1", l1MuReadout = cms.InputTag("gtDigis"))

process.linkSynchroAnalysis =  cms.EDAnalyzer("LinkSynchroAnalysis",
  writeHistograms = cms.untracked.bool(True),
  histoFileName = cms.untracked.string("synchroAnalysis.root"),
  linkMonitorPSet = cms.PSet(
    useFirstHitOnly = cms.untracked.bool(True),
    dumpDelays = cms.untracked.bool(False)
  ),
#  synchroSelectorL1Muon = cms.PSet(
#    l1MuReadout = cms.InputTag("gtDigis"),
#    systems = cms.vstring("rpcf","rpcb","dt","csc"),
#    maxDeltaEta = cms.double(0.2), 
#    maxDeltaPhi = cms.double(0.3),
#    checkBX0  = cms.bool(True) 
#  ),
  synchroSelectorMuon = cms.PSet( 
    muonColl= cms.string("muons"),
    minPt = cms.double(2.),  
    maxTIP = cms.double(0.5),
    maxEta = cms.double(1.55),
    beamSpot = cms.InputTag("offlineBeamSpot"),
    checkUniqueRecHitMatching = cms.bool(True)
  ),
#  synchroSelectorTrack =  cms.PSet( 
#    collection = cms.string("generalTracks"),
#    minPt = cms.double(2.),
#    maxTIP = cms.double(0.5),
#    beamSpot = cms.InputTag("offlineBeamSpot"),
#    checkUniqueRecHitMatching = cms.bool(True),
#    l1MuReadout = cms.InputTag("gtDigis"),
#    maxDeltaEta = cms.double(0.2),
#    maxDeltaPhi = cms.double(0.25),
#    rpcMatcherPSet =  cms.PSet( maxDeltaEta = cms.double(0.2), maxDeltaPhi = cms.double(0.25))
#  )
)

process.efficiencyTree = cms.EDAnalyzer("EfficiencyTree",
  histoFileName = cms.string("efficiencyHelper.root"),
  treeFileName = cms.string("efficiencyTree.root"),
  muonColl = cms.string("muons"),
  trackColl = cms.string("generalTracks"),
  beamSpot = cms.InputTag("offlineBeamSpot"),
  minPt = cms.double(2.),
  maxTIP = cms.double(0.5),
  maxEta = cms.double(1.55),
  l1MuReadout = cms.InputTag("gtDigis"),
  l1MuReadoutEmu = cms.InputTag("gtDigis"),
  rpcMatcherPSet =  cms.PSet( maxDeltaEta = cms.double(0.5), maxDeltaPhi = cms.double(0.5)), 
  dtcscMatcherPSet = cms.PSet( maxDeltaEta = cms.double(0.1), maxDeltaPhi = cms.double(0.1))
)

#print process.dumpPython()

process.p = cms.Path( 
#  process.filterBX*
#  process.primaryVertexFilter*
  process.gtDigis*
#  process.filterL1* 
#  process.muonRPCDigis*
#  process.l1RpcEmulDigis*
#  process.rpcFEDIntegrity*process.rpcMonitorRaw*
#  process.l1compare*process.l1demon*
#  process.l1trpctf*
#  process.linkSynchroAnalysis
  process.globalMuons*
  process.efficiencyTree
  )
