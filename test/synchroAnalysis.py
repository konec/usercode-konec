import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")


process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1))
process.source = cms.Source("PoolSource", fileNames =  cms.untracked.vstring( 
#'file:/disk00/work/data/run147647/E29BE225-32D4-DF11-BA24-001D09F24E39.root'),
#'file:/disk00/work/data/run158335/502449EB-DE3B-E011-B733-0030487CD77E.root'),
#'file:/disk00/work/data/run160413/983CBA22-C44D-E011-B124-00304879BAB2.root'),
#'file:/disk00/work/data/run160413/F8D81AA3-BE4D-E011-8DB2-001D09F24399.root'),
'file:/disk00/work/data/run160943/DAC0F150-1A53-E011-99B4-001D09F2514F.root'),
#'file:/disk00/work/data/Commissioning-MuonDPG_skim-May27thSkim_v2/1E1A93A9-B96D-DF11-848B-003048D47A2E.root'),
#'file:/disk00/work/data/Commissioning-MuonDPG_skim-May27thSkim_v2/B60EB18B-AA6D-DF11-9E3E-003048D476C6.root'),
#'file:/disk00/work/data/Commissioning-MuonDPG_skim-May27thSkim_v2/96984B02-996D-DF11-BB4F-003048D460AE.root'),
inputCommands=cms.untracked.vstring( 'keep *', 'drop *_hltL1GtObjectMap_*_*')
)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
#process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.GeometryExtended_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
#process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.gtDigis.UnpackBxInEvent = cms.int32(3)
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'GR_R_311_V2::All'
#process.GlobalTag.globaltag = 'START311_V2::All'
#process.GlobalTag.globaltag = 'DESIGN311_V1::All'
#process.GlobalTag.globaltag = 'MC_311_V2::All'
#process.GlobalTag.globaltag = 'MC_39Y_V7::All'

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
# mesage logger
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
    useFirstHitOnly = cms.untracked.bool(False),
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
  rpcMatcherPSet =  cms.PSet( maxDeltaEta = cms.double(0.5), maxDeltaPhi = cms.double(0.5)), 
  dtcscMatcherPSet = cms.PSet( maxDeltaEta = cms.double(0.1), maxDeltaPhi = cms.double(0.1))
)

#print process.dumpPython()

process.p = cms.Path( 
#  process.filterBX*
#  process.primaryVertexFilter*
  process.gtDigis*
  process.filterL1* 
  process.muonRPCDigis*
  process.l1RpcEmulDigis*
  process.rpcFEDIntegrity*process.rpcMonitorRaw*
  process.l1compare*process.l1demon*
  process.l1trpctf*
  process.linkSynchroAnalysis
  )
#  process.efficiencyTree)
