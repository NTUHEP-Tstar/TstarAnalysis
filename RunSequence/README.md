# RunSequence

Python scripts to help execute `cmsRun` over large amounts of data file with `crab` or `bsub`. All the raw `EDM` files will be linked/stored in the `Runsequence/store/EDM_Files` directory, while the analysis directories (`CompareDataMC`/`LimitCalc`/`TstarMassReco`) will link to this directory for inputing.

Instructions to run the main analysis sequence:

1. Edit the `python/Settings.py`, file to your requirements ( detailed documentations included in said file )
2. Edit the `settings/XXX_dataset.json` files, this is helped to define which datasets to run.
3. Run the command
```
./00_gen_dataset.py -i settings/XXX_dataset.json -o XXX.txt
```
: generates raw text files listing all the DAS output datasets matching the queries defined the the input json file. Double check the output file to make sure that there are no missing/redundant datasets before preceeding with the next command
4. Run the command
```
./01_run_baseline_selection.py -i XXX.txt -m YYYY
```
This will generated the crab scripts for running the baselinse selection process (defined in `cmsrun/run_baseline_selection.py`) under the YYYY mode for all the datasets listed in the `XXX.txt` file. The crab configuration python files will be stored under `store/CrabConfig`, with the expected crab working directory under `store/CrabWorkSpace/` (both should exists already as empty directories). Please manually submit the configuration files yourself.

5. Run the command
```
./02_retrieve_files.py -i XXX.txt -m YYYY
```
This is attempt to store the file created by the crab jobs in the previous step with the same argument to the directory `/store/EDM_Files/tstarbaseline`. Note that the file name will be designed as `tstarbaseline/YYYY/<dataset>_<index>.root`.

6. Run the command
```
./local_run_XXX.py -i XXX.txt -m YYYY
```
This generated the bash scripts required to runs reconstruction script defined in `cmsrun/*.py` over the the stored files from the previous command, and stored the files to `store/EDM_Files/massreco/YYYY/<dataset>_<index>.root`. Notice the scripts will be generated but not submitted. It is advised that you move to a larger working space to submit the files.

7. Run the individual analysis programs in the various analysis directories

8. Run the command `pack_plots.py`. This will extract all the `.png` files and `.tex` output files in each analysis directory, and compress them to a single file of `results.tar.gz`
