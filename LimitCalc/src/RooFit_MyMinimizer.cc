/*******************************************************************************
*
*  Filename    : RooFit_Common.cc
*  Description : Common routines regarding RooFit objects
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/RootMgr/interface/RooFitUtils.hpp"
#include "TstarAnalysis/LimitCalc/interface/MyMinimizer.hpp"

#include "Math/CholeskyDecomp.h"
#include "RooNLLVar.h"

using namespace std;

/*******************************************************************************
*   Default constructor setting up additional options
*******************************************************************************/
MyMinimizer::MyMinimizer( RooAbsReal* nll ) :
  RooMinimizer( *nll ),
  _nll( nll )
{
  // Output message options
  setVerbose( kFALSE );
  setEvalErrorWall( kFALSE );
  setPrintLevel( -1 );
  setPrintEvalErrors( -1 );

  // Running default settings
  optimizeConst( kTRUE );
  setEps( 0.01 );
  setMaxIterations( 10000 );
  setMaxFunctionCalls( 10000 );
}

/******************************************************************************/

MyMinimizer::~MyMinimizer()
{

}

/*******************************************************************************
*   Rewritting fitting function:
*   Based on RooAbsPdf.fitTo() but with default options set down
*******************************************************************************/
RooFitResult*
MyMinimizer::RunFit( const int weightflag )
{
  // Calling the minimization function
  minimize( "Minuit2", "Migrad" );
  hesse();

  // Reevaluating error for weighted datasets
  if( weightflag == WEIGHTED && getNPar() > 0 ){
    // Make list of RooNLLVar components of FCN
    RooArgSet*  argset = _nll->getComponents();
    vector<RooNLLVar*> nllComponents = mgr::FromRooArgSet<RooNLLVar>( argset );

    // Calculated corrected errors for weighted likelihood fits
    RooFitResult* rw = save();

    for( auto component : nllComponents ){
      if( component )
      component->applyWeightSquared( kTRUE );
    }

    hesse();
    RooFitResult* rw2 = save();

    for( auto component : nllComponents ){
      if( component )
      component->applyWeightSquared( kFALSE );
    }

    // Apply correction matrix
    const TMatrixDSym& matV = rw->covarianceMatrix();
    TMatrixDSym matC        = rw2->covarianceMatrix();
    using ROOT::Math::CholeskyDecompGenDim;
    CholeskyDecompGenDim<Double_t> decomp( matC.GetNrows(), matC );
    if( decomp ){
      // replace C by its inverse
      decomp.Invert( matC );

      // the class lies about the matrix being symmetric, so fill in the
      // part above the diagonal
      for( int i = 0; i < matC.GetNrows(); ++i ){
        for( int j = 0; j < i; ++j ){
          matC( j, i ) = matC( i, j );
        }
      }

      matC.Similarity( matV );
      // C now contiains V C^-1 V
      // Propagate corrected errors to parameters objects
      applyCovarianceMatrix( matC );
    }

    delete rw;
    delete rw2;
  }

  minos();
  RooFitResult* ans = save();
  return ans;
}
