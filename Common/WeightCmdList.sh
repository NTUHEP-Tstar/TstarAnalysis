#!/bin/bash

ComputeGenWeightUnc -i /wk_cms2/yichen/MiniAOD/TstarM800.root   -o "results/TstarM800"  -t "M_{t*}=800GeV/c^{2} (no selection)"
ComputeGenWeightUnc -i /wk_cms2/yichen/MiniAOD/TstarM1200.root  -o "results/TstarM1200"  -t "M_{t*}=1200GeV/c^{2} (no selection)"
ComputeGenWeightUnc -i /wk_cms2/yichen/MiniAOD/TstarM1600.root  -o "results/TstarM1600"  -t "M_{t*}=1600GeV/c^{2} (no selection)"

ComputeGenWeightUnc -i /wk_cms2/yichen/TstarAnalysis/EDMStore_New/massreco/*Signal/TstarTstarToTgluonTgluon_M-800*.root  -o "results/TstarM800_signal"  -t "M_{t*}=800GeV/c^{2} (1 lep, #geq 6 jets, = 2 b jets)"
ComputeGenWeightUnc -i /wk_cms2/yichen/TstarAnalysis/EDMStore_New/massreco/*Signal/TstarTstarToTgluonTgluon_M-1200*.root  -o "results/TstarM1200_signal"  -t "M_{t*}=1200GeV/c^{2} (1 lep, #geq 6 jets, = 2 b jets)"
ComputeGenWeightUnc -i /wk_cms2/yichen/TstarAnalysis/EDMStore_New/massreco/*Signal/TstarTstarToTgluonTgluon_M-1600*.root  -o "results/TstarM1600_signal"  -t "M_{t*}=1600GeV/c^{2} (1 lep, #geq 6 jets, = 2 b jets)"

ComputeGenWeightUnc -i /wk_cms2/yichen/TstarAnalysis/EDMStore_New/toplike/*/TstarTstarToTgluonTgluon_M-800*.root  -o "results/TstarM800_toplike"  -t "M_{t*}=800GeV/c^{2} (1 lep, #geq 4 jets, = 2 b jets)"
ComputeGenWeightUnc -i /wk_cms2/yichen/TstarAnalysis/EDMStore_New/toplike/*/TstarTstarToTgluonTgluon_M-1200*.root  -o "results/TstarM1200_toplike"  -t "M_{t*}=1200GeV/c^{2} (1 lep, #geq 4 jets, = 2 b jets)"
ComputeGenWeightUnc -i /wk_cms2/yichen/TstarAnalysis/EDMStore_New/toplike/*/TstarTstarToTgluonTgluon_M-1600*.root  -o "results/TstarM1600_toplike"  -t "M_{t*}=1600GeV/c^{2} (1 lep, #geq 4 jets, = 2 b jets)"

ComputeGenWeightUnc -i /wk_cms2/yichen/TstarAnalysis/EDMStore_New/massreco/*Signal/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8*.root  -o "results/TT_Signallike"  -t "t#bar{t} (1 lep, #geq 6 jets, = 2 b jets)"

ComputeGenWeightUnc -i /wk_cms2/yichen/TstarAnalysis/EDMStore_New/toplike/*/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8*.root  -o "results/TT_Toplike"  -t "t#bar{t} (1 lep, #geq 4 jets, = 2 b jets)"
