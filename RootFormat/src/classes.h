#include "TstarAnalysis/RootFormat/interface/FitParticle.hpp"
#include "TstarAnalysis/RootFormat/interface/RecoResult.hpp"
#include "DataFormats/Common/interface/Wrapper.h"

namespace TstarAnalysis_TstarMassReco {
        struct dictionaryobjects {
                std::map<tstar::Momentum_Label,TLorentzVector>  stdmp_tv;
                FitParticle ftptcl;
                RecoResult rcrst;
                edm::Wrapper<RecoResult>  rcrst_wp;
        };
}
