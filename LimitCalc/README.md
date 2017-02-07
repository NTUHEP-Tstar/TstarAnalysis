# LimitCalc

Analysis code for running limit related parts of the analysis.

## Main helper class: [`SampleRooFitMgr`](interface/SampleRooFitMgr.hpp)
Defines the global [`RooRealVar` instances](src/SampleRooFitMgr.cc) and how the various data sets should be [filled](src/SampleRooFitMgr_FillSet.cc). Also defines how the background function will be defined as a [`RooAbsPdf` object](src/SampleRooFitMgr_MakePDF.cc).

## Main binary files.
### MakeRooFit
Code for creating the objects required to run higgs combine package. `RooDataSet`, `RooAbsPdf` and datacards. Also responsible for making the plots.

For list of fitting method, read [`settings/naming_settings.json`](../Common/settings/name_settings.json).
For list of fitting functions, read [`settings/naming_settings.json`](../Common/settings/name_settings.json) or [`src/SampleRooFitMgr_MakePDF.cc.cc`](src/SampleRooFitMgr_MakePDF.cc)

### PlotLimit
Takes output of `MakeRooFit` and runs the Higgs Combine Package to determine the limit. Also Plots the results.

### PlotCombinePull
Takes the output for `MakeRooFit` and runs the higgs combine package to determine the difference between the pre-fit and post-fit nuisance parameter values.

### RunValGenFit
Runs the SimFit pseudo experiment code for a various number of times, one can also specify the number of injected signal events.

### PlotValGenFit
Reads the results of the `RunValGenFit` to create a pull distribution plots.

### MergeCards
Binary file for merging data cards for different channel.


## Helper scripts

### genCommandList.sh.
Generates the commands required for running most of the `MakeRooFit` and `PlotLimit` calls.

### genValSimScripts.py
Creates standalone script files for running the RunValGenFit command (since it requires a lot of time.)
