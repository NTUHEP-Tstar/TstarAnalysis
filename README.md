# TstarAnalysis

Main code for excited top quark analysis.

# Deploy commands
```
cmsrel CMSSW_8_0_25
cd CMSSW_8_0_25/src
cmsenv

git cms-init
git cms-merge-topic -u cms-met:fromCMSSW_8_0_20_postICHEPfilter
git cms-merge-topic cms-met:METRecipe_8020
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
# Structure overview

## Interfacing sub-packages

* [`Common`](Common): Utility functions and classes that are specific for this analysis, but is used among multiple sub routines. Helps ensures that the variables stored in the EDM files manipulation routines and FWLite readout routines are uniformed.

* [`RootFormat`](RootFormat): Custom root objects to be stored in the EDM files. Avoid altering this sub-package as much as possible.

## EDM manipulation sub-packages

* [`RunSequence`](RunSequence): Master EDM manipulation control script. For generating crab scripts and mass-processing EDM files and such.

* [`BaseLineSelector`](BaseLineSelector): EDM plugins required to strip MINIAOD files to baseline requirements and basic variable caching.

* [`AdditionalSelector`](AdditionalSelector) EDM plugins required to further select events to dedicated signal/control regions.

* [`TstarMassReco`](TstarMassReco): EDM plugins and classes for running computing mass reconstruction algorithms. Includes interfaces for reconstruction classes.

* [`EventWeight`](EventWeight): EDM plugins for applied event weights to various physical objects.

* [`ModifiedHitFit`](ModifiedHitFit): Modified version of [`TopQuarkAnalysis/TopHitFit/`] (https://github.com/cms-sw/cmssw/tree/CMSSW_8_1_X/TopQuarkAnalysis/TopHitFit) package from the official CMSSW

## FWLite readout and analysis sub-packages

* [`CompareDataMC`](CompareDataMC): Plotting classes for comparing quantities between data and Monte Carlo

* [`LimitCalc`](LimitCalc): limit determination study analysis code.

* [`MassRecoCompare`](MassRecoCompare): Generating comparison plots for determining reconstruction algorithm performance.


# General guideline in sub-packages

* `bin`: Defining the main function for binaries. Binaries should be soft-linked to the sub-package directory.

* `src`: No class should share files. Any function other than the main functions and the EDM plugins should go here.

* `plugin`: EDM plugins should typically be contained within one file.

* `python`: all python files that are not directly executable or executable by `cmsRun` should go here

* `test`: Various unit testing main function source code should go here. Please link testing binaries under the `test/bin` directory

* `interface`: Each class that is not an EDM plugin should have it's own header file. A file listing all functions in the `src` directory and listing external global variables should also be included.

* `settings`: Common settings files shard among various subroutines, should be linked to [`Common/settings`](Common/settings)

* `data`: Settings unique to sub package and are required for crab jobs.

* `results`: All results(generated text files, plots, root files) should go in here.
