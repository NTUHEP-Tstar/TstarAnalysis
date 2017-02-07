# MassRecoCompare

Analysis code for comparing the performance of mass reconstruction schemes.


## CompareMethod

Making plots for comparing between different reconstruction method. Histogram that are compared are defined by the class [`CompareHistMgr`](interface/CompareHistMgr.hpp) (A simple `HistMgr` class)


## CompareKeys

Compare the RooKeysPdf object for different `rho` values. Datasets and RooFit variables are defined using the class [`KeysCompMgr`](interface/KeysCompMgr.hpp) (A `RooFitMgr` class)
