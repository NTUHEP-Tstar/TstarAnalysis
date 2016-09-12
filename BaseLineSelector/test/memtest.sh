#!/bin/bash

# simple script for testing memory usage
# valgrind --leak-check=full cmsRun run_test.py maxEvents=10 Mode=MuonSignal sample="/store/mc/RunIISpring16MiniAODv2/WJetsToLNu_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v3/10000/B45B5B3D-8F5D-E611-88A2-FA163EA68EDC.root","/store/mc/RunIISpring16MiniAODv2/WJetsToLNu_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v3/10000/B61729BA-865D-E611-B0D0-02163E00F4E0.root","/store/mc/RunIISpring16MiniAODv2/WJetsToLNu_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v3/10000/BA6CEB25-875D-E611-9D1A-0025905B8580.root"


/usr/bin/time -f "%M(Kb) memory\n%S(sec) CPU time\n" \
cmsRun run_test.py maxEvents=-1 Mode=MuonSignal HLT="TriggerResults::HLT" \
sample="/store/mc/RunIISpring16MiniAODv2/WJetsToLNu_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v3/10000/B45B5B3D-8F5D-E611-88A2-FA163EA68EDC.root"\
#,"/store/mc/RunIISpring16MiniAODv2/WJetsToLNu_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v3/10000/B61729BA-865D-E611-B0D0-02163E00F4E0.root"\
#,"/store/mc/RunIISpring16MiniAODv2/WJetsToLNu_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v3/10000/BA6CEB25-875D-E611-9D1A-0025905B8580.root"
