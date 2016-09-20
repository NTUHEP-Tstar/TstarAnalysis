# Common

Utility class and functions used by subroutines in this analysis.

* Naming conventions for files
* Cached varaible access.

## Utility class `TstarNamer`
This class helps enforces the structures among the subpackages to reduce the number of redundant files. It also helps to maintain a uniform naming convention among the produced files according to the inputs. Based on the more general utility class [`PackagePathMgr`](https://github.com/NTUHEP-Tstar/ManagerUtils/blob/master/SysUtils/doc/PackagePathMgr) and [`OptsNamer`](https://github.com/NTUHEP-Tstar/ManagerUtils/blob/master/SysUtils/doc/OptsNamer.md)

* `MasterConfigFile()` will return the absolute path of the `SampleGroup` settings file, which will always be `TstarAnalysis/SubPackage/settings/master.json`
