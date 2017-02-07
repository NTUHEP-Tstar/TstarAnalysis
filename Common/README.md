# Common

Utility class and functions used by subroutines in this analysis. And interfacing between EDM plugin caching results and FWLite analysis code.

## EDM object/variable interfacing

* [`IDCache.hpp`](interface/IDCache.hpp): Accessing variable formats stored in various objects. Plugins generating the cached variables should be found in the [`BaseLineSelector`](../BaseLineSelector) directory.
* [`EventWeight.hpp`](interface/GetEventWeight.hpp): Getting the various weight values for a given `edm::Event`. Actual code for creating the event variable stored in the directory [`EventWeight`](../EventWeight). The plugin naming should be set in the cmsRun files defined in the directory [`RunSequence/cmsrun`](../RunSequence/cmsrun).

## FWLite analysis code helper functions


### Utility class [`TstarNamer`](interface/TstarNamer.hpp)

This class helps enforces a common output structure and naming format withing a single . Also acts as the interface class to the base classes [`PackagePathMgr`](https://github.com/NTUHEP-Tstar/ManagerUtils/blob/master/SysUtils/doc/PackagePathMgr) and [`OptNamer`](https://github.com/NTUHEP-Tstar/ManagerUtils/blob/master/Common/doc/OptNamer.md) for the analysis code.

#### Specilized functions
* `MasterConfig()` the master configuration used to initialize the `SampleGroup` settings file. The configuration will always be taken from the `TstarAnalysis/SubPackage/settings/master.json` file.

* `Channel()` function: The class will always assume that an instance of a analysis code should have an associated selection channel channel. Could be manually changed. The calss will automatically detect the `--channel` option input in the command line when the `ParseOption()` function is called.


* `CustomFileName( ext, tag1, tag2,...,tagn)`: Generates the full path string for file
   ````
   .../TstarAnalysis/SubPackage/results/<channel>/tag1_tag2_tag3_...tagn.ext
   ````

* `SetNamingOptions(opt1,opt2...)`. Set the namer class to add the input of options to the file naming.

* `OptFileName(ext,maintag,tag1,tag2,...,tagn)`. Create the full path string for the file:
   ```
   .../TstarAnalysis/SubPackage/results/<channel>/maintag_optinput1_optinput2_..._tag1_tag2_tag3_...tagn.ext
   ```


### Other helper functions

* [`InitSample.hpp`](interface/InitSample.hpp): Common `SampleGroup` and `SampleMgr` initialization from a `TstarNamer` instance.
* [`PlotStyle.hpp`](interface/NameParse.hpp): Common plotting settings for this analysis.
* [`NameParse.hpp`](interface/NameParse.hpp): Function for stripping and name like TstarM1000 to just the integer mass point (1000).
