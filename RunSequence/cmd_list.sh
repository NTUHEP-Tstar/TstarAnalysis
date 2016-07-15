#*******************************************************************************
 #
 #  Filename    : cmd_list.sh
 #  Description : For memo only! Make executable
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************

./00_gen_dataset.py -i settings/muon_datasets.json      -o MuonList.txt
./00_gen_dataset.py -i settings/electron_datasets.json  -o ElectronList.txt
./00_gen_dataset.py -i settings/mc_datasets.json        -o MCList.txt


./01_run_baseline_selection.py -i MuonList.txt     -m MuonSignal
./01_run_baseline_selection.py -i ElectronList.txt -m ElectronSignal
./01_run_baseline_selection.py -i MCList.txt       -m MuonSignal
./01_run_baseline_selection.py -i MCList.txt       -m ElectronSignal

./02_retrieve_files.py -i MuonList.txt     -m MuonSignal
./02_retrieve_files.py -i ElectronList.txt -m ElectronSignal
./02_retrieve_files.py -i MCList.txt       -m MuonSignal
./02_retrieve_files.py -i MCList.txt       -m ElectronSignal


./03_run_massreco.py -i MuonList.txt      -m MuonSignal
./03_run_massreco.py -i ElectronList.txt  -m ElectronSignal
./03_run_massreco.py -i MCList.txt        -m MuonSignal
./03_run_massreco.py -i MCList.txt        -m ElectronSignal
