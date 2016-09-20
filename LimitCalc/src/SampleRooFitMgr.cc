/*******************************************************************************
*
*  Filename    : SampleRooFitMgr.cc
*  Description : implementation of roofit manager class
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/Common/interface/InitSample.hpp"
#include <iostream>

using namespace std;
using namespace mgr;
// ------------------------------------------------------------------------------
//   Static member function delcarations
// ------------------------------------------------------------------------------
RooRealVar&
SampleRooFitMgr::x(){ return *StaticVar( "x" ); }

RooRealVar&
SampleRooFitMgr::w(){ return *StaticVar( "w" ); }

double
SampleRooFitMgr::MinMass(){ return x().getMin(); }

double
SampleRooFitMgr::MaxMass(){ return x().getMax(); }

void
SampleRooFitMgr::InitStaticVars( const double min, const double max )
{
   StaticNewVar( "x", "M_{t+g}",      "GeV/c^{2}", min,   max  );
   StaticNewVar( "w", "event_weight", "",          -1000, 1000 );
}


// ------------------------------------------------------------------------------
//   Constructor and destructor
// ------------------------------------------------------------------------------
SampleRooFitMgr::SampleRooFitMgr( const string& name, const ConfigReader& cfg ) :
   Named( name ),
   SampleGroup( name, cfg ),
   RooFitMgr( name )
{
   definesets();

   for( auto& sample : SampleList() ){
      // Caching sample wide variable (weight sums.. etc), see Common/src/InitSample.cc
      InitSample( *sample );

      // Fitting the defined dataset, see LimitCalc/src/SampleRooFitMgr_FillSet.cc
      fillsets( *sample );
   }
}

SampleRooFitMgr::~SampleRooFitMgr()
{
   for( const auto& name : VarNameList() ){
      RooRealVar* var = Var( name );
      printf( "%30s %8.4lf %8.4f\n", var->GetName(), var->getVal(), var->getError() );
      fflush( stdout );
   }
}


// ------------------------------------------------------------------------------
//   Dataset argument options
// ------------------------------------------------------------------------------
RooDataSet*
SampleRooFitMgr::NewDataSet( const std::string& name )
{
   if( DataSet( name ) ){
      return DataSet( name );
   } else {
      RooDataSet* set = new RooDataSet(
         name.c_str(), "",
         RooArgSet( x(), w() ),
         RooFit::WeightVar( w() )
         );
      AddDataSet( set );
      return set;
   }
}

RooDataSet*
SampleRooFitMgr::DataSet( const std::string& name )
{
   return RooFitMgr::DataSet( name );
}
