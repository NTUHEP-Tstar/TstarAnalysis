# CompareDataMC

Making comparisons between Data and Monte Carlo by dumping file content information.

## Helper classes

* [`SampleHistMgr`](interface/SampleHistMgr.hpp) a simple joint class of `SampleGroup` and `HistMgr`. Meant for quickly adding histograms that would be useful for cross checking.

* [`SampleErrHistMgr`](interface/SampleErrHistMgr.hpp) a joint class of `SampleGroup` and `HistMgr` meant for more rigorous error comparison of plots.

* [`SampleTableMgr`](interface/SampleTableMgr.hpp) an extended class of `SampleGroup` to store additional sample wide strings and doubles for generating summary tables.


## Analysis binary executables

### FillHistograms.
Filling the histograms and sample wide event weight sums to storage files. The addition of this binary is because to the addition of a top-like selection cross checks, a filling that includes all samples consumes too much memory.

### KinematicCompare
Takes the histograms defined in the `SampleHistMgr` class and display the kinematic distribution of data, background mc and signal sample.

### FullCompare
Takes the histograms defined in the `SampleErrHistMgr` class and displays the kinematic distribution of data, background MC ans signal with additional error calculations for the background MC.

### ErrorCompare
Takes the error histograms defined in the `SampleErrHistMgr` class and display the kinematic distribution of of a single group with error displayed.

### MakeSummaryTable
Generating a summary of selection results in Latex tabular format.
