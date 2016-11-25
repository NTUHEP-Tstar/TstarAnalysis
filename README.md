# TstarAnalysis

Main code for excited top quark analysis.

# Deploy commands
```
cmsrel CMSSW_8_0_20_patch1
cd CMSSW_8_0_20_patch1/src

git cms-init
git cms-merge-topic -u cms-met:CMSSW_8_0_X-METFilterUpdate
git cms-merge-topic ikrav:egm_id_80X_v1

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
Next go to your `TstarAnalysis` directory and alter the `Common/settings/higgs_combine_settings.json` to be something like:
```
{
   "Scram Arch" : "slc6_amd64_gcc491",
   "Store Path" : "/your/install/path",
   "CMSSW Version": "CMSSW_7_4_7",
   "Higgs Combine Version" : "v6.2.1"
}
```


### General structure (in alphabetical order)

* [`AdditionalSelector`](AdditionalSelector) All EDM plugins required to further select events to dedicated signal/control regions.

* [`BaseLineSelector`](BaseLineSelector): All EDM plugins and functions required to strip MINIAOD files to baseline requirements and basic variable caching

* [`Common`](Common): Utility functions and classes that are specific for this analysis, but is used among multiple sub routines.

* [`CompareDataMC`](CompareDataMC): Plotting classes for comparing quantities between data and Monte Carlo

* [`EventWeight`](EventWeight): All EDM classes required for caching event weighting information to events.

* [`LimitCalc`](LimitCalc): Preparing objects to pass to [Higgs Combine Package], and also plot results into graphs. Includes validation sub-processes

* [`ModifiedHitFit`](ModifiedHitFit): Modified version of [`TopQuarkAnalysis/TopHitFit/`] (https://github.com/cms-sw/cmssw/tree/CMSSW_8_1_X/TopQuarkAnalysis/TopHitFit) package from the official CMSSW

* [`RootFormat`](RootFormat): All classes with dictionaries for ROOT goes here.

* [`RunSequence`](RunSequence): All instances where running over large numbers of datasets is placed here. Includes utility scripts for crab/lfn job submissions.

* [`TstarMassReco`](TstarMassReco): Algorithms for computing mass reconstruction. Includes interfaces for reconstruction classes and cmsRun


### General guideline in subpackages

* [`bin`]: Defining the main function for binaries. Binaries should be softlink to the subdirectory

* [`src`]: No class should share files. Any function other than the main functions and the EDM plugins should go here.

* [`plugin`]: All plugins should be contained within one file.

* [`python`]: all python files that are not directly executable or executable by `cmsRun` should go here

* [`test`]: Various unit testing main function source code should go here. Please link testing binaries under the `test` directory

* [`interface`]: Each class that is not an EDM plugin should have it's own header file. A file listing all functions in the `src` directory and listing external global variables should also be included.

* [`settings`]: Common settings files shard among various subroutines, should be linked to [`Common/settings`](Common/settings)

* [`data`]: Settings unique to sub package

* [`samples`]: Soft link to EDM storage directory `RunSequence/store/EDM_Files`

* [`results`]: All results(generated text files, plots, root files) should go in here.
