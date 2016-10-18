#*******************************************************************************
 #
 #  Filename    : cmd_list.sh
 #  Description : For memo only! Make executable
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************

./00_gen_dataset.py -i settings/muon80X_datasets.json      -o MuonData80X.txt
./00_gen_dataset.py -i settings/electron80X_datasets.json  -o ElectronData80X.txt
./00_gen_dataset.py -i settings/mc80X_datasets.json        -o MC80X.txt


./01_run_baseline_selection.py -i MuonData80X.txt     -m Muon
./01_run_baseline_selection.py -i ElectronData80X.txt -m Electron
./01_run_baseline_selection.py -i MC80X.txt           -m Muon
./01_run_baseline_selection.py -i MC80X.txt           -m Electron

./02_retrieve_files.py         -i MuonData80X.txt      -m Muon
./02_retrieve_files.py         -i ElectronData80X.txt  -m Electron
./02_retrieve_files.py         -i MC80X.txt            -m Muon
./02_retrieve_files.py         -i MC80X.txt            -m Electron

./03_run_massreco.py   -i MuonData80X.txt      -m MuonSignal
./03_run_massreco.py   -i ElectronData80X.txt  -m ElectronSignal
./03_run_massreco.py   -i MC80X.txt            -m MuonSignal
./03_run_massreco.py   -i MC80X.txt            -m ElectronSignal

./03_run_massreco.py   -i MuonData80X.txt      -m MuonControl
./03_run_massreco.py   -i ElectronData80X.txt  -m ElectronControl
./03_run_massreco.py   -i MC80X.txt            -m MuonControl
./03_run_massreco.py   -i MC80X.txt            -m ElectronControl
