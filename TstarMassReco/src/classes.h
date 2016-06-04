#include "TstarAnalysis/TstarMassReco/interface/RecoResult.hh"
#include "DataFormats/Common/interface/Wrapper.h"

namespace TstarAnalysis_TstarMassReco {
   struct dictionaryobjects {
      FitParticle ftptcl;
      RecoResult rcrst;
      edm::Wrapper<RecoResult>  rcrst_wp;
   };
}
