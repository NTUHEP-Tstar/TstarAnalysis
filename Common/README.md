# Common

Utility class and functions used by subroutines in this analysis the main one being the class documented below

### Utility class `TstarNamer`
This class helps enforces the structures among the subpackages to reduce the number of redundant files. It also helps to maintain a uniform naming convention among the produced files according to the inputs. Based on the more general utility class `PackagePathMgr` and `OptsNamer`

* `MasterConfigFile()` will return the absolute path of the `SampleGroup` settings file, which will always be `TstarAnalysis/SubPackage/settings/master.json`
