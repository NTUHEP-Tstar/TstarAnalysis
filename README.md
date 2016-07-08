# TstarAnalysis

Main code for excited top quark analysis.

# Deploy commands
```
cmsrel CMSSW_7_6_4
cd CMSSW_7_6_4/src

git clone https://github.com/NTUHEP-Tstar/TstarAnalysis.git
git clone https://github.com/NTUHEP-Tstar/ManagerUtils.git

scram b
```

You will also need to install a version of the [Higgs Combine Package](https://twiki.cern.ch/twiki/bin/viewauth/CMS/SWGuideHiggsAnalysisCombinedLimit#For_end_users_that_don_t_need_to) elsewhere and expose it to the TstarAnalysis code, here we go by the please note to remember the following setup for your Higgs Combine Package setup:

  * The `SCRAM_ARCH` used 
  * The CMSSW version
  * The version of the higgs combine package used 
  * Where you installed you CMSSW 

Here we are going to give a sample installation script using the recommended version of Higgs Combine package so far

```
cd /your/install/path
export SCRAM_ARCH=slc6_amd64_gcc491
cmsrel CMSSW_7_4_7
cd CMSSW_7_4_7/src 
cmsenv
git clone https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit.git HiggsAnalysis/CombinedLimit -b v6.2.1
scram b 
```
Next go to your `TstarAnalysis` directory and alter the `NameFormat/settings/higgs_combine_settings.json` to be something like:
```
{
   "Scram Arch" : "slc6_amd64_gcc491",
   "Store Path" : "/your/install/path",
   "CMSSW Version": "CMSSW_7_4_7",
   "Higgs Combine Version" : "v6.2.1"
}
```
