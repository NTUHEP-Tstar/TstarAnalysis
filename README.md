# TstarAnalysis

Main code for excited top quark analysis.

# Deploy commands
```
cmsrel CMSSW_7_6_4
cd CMSSW_7_6_4/src

git clone https://github.com/NTUHEP-Tstar/TstarAnalysis.git
git clone https://github.com/NTUHEP-Tstar/ManagerUtils.git

git clone https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit.git HiggsAnalysis/CombinedLimit -b v6.2.1

scram b
```
