import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1))
process.source = cms.Source("EmptySource")

process.load("DQMServices.Core.DQM_cfg")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'START36_V3::All'

process.merger =  cms.EDAnalyzer("LinkSynchroMerger",
  writeHistograms = cms.untracked.bool(True),
  histoFileName = cms.untracked.string("merge.root"),
  linkMonitorPSet = cms.PSet(
    useFirstHitOnly = cms.untracked.bool(True),
    dumpDelays = cms.untracked.bool(True)
  ),
  preFillLinkSynchroFileNames=cms.untracked.vstring(
       'jobs/run132440-442/out.txt',
       'jobs/run132471-478/out.txt',
       'jobs/run132569-572/out.txt',
       'jobs/run132596-599/out.txt',
       'jobs/run132601-602/out.txt',
       'jobs/run132605-605/out.txt',
       'jobs/run132645-654/out.txt',
       'jobs/run132656-661/out.txt',
       'jobs/run132662-959/out.txt',
       'jobs/run132960-961/out.txt',
       'jobs/run132965-968/out.txt',
       'jobs/run133029-046/out.txt',
       'jobs/run133081-082/out.txt',
       'jobs/run133089-172/out.txt',
       'jobs/run133242-289/out.txt',
       'jobs/run133320-320/out.txt',
       'jobs/run133321-324/out.txt',
       'jobs/run133336-446/out.txt',
       'jobs/run133448-448/out.txt',
       'jobs/run133450-483/out.txt' 
  )
)


process.MessageLogger = cms.Service("MessageLogger",
    debugModules = cms.untracked.vstring('merger'),
    destinations = cms.untracked.vstring('cout'),
    cout = cms.untracked.PSet( threshold = cms.untracked.string('DEBUG'))
)


process.p = cms.Path(process.merger)
