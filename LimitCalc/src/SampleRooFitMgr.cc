/*******************************************************************************
 *
 *  Filename    : SampleRooFitMgr.cc
 *  Description : implementation of roofit manager class
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/RootFormat/interface/RecoResult.hpp"

#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "DataFormats/FWLite/interface/Handle.h"

#include <cstdio>

using namespace std;
using namespace mgr;
//------------------------------------------------------------------------------
//   Static member function delcarations
//------------------------------------------------------------------------------
double SampleRooFitMgr::_min_mass = 0 ;
double SampleRooFitMgr::_max_mass = 0 ;
RooRealVar* SampleRooFitMgr::_x    = NULL;
RooRealVar* SampleRooFitMgr::_w    = NULL;

void SampleRooFitMgr::InitRooVars( const double min, const double max )
{
   ClearRooVars();
   _x = new RooRealVar( "x" , "M_{t+g}"      , min   , max , "GeV/c^{2}" );
   _w = new RooRealVar( "w" , "event_weight" , -1000., 1000, "");
   _min_mass = min;
   _max_mass = max;
}

void SampleRooFitMgr::ClearRooVars()
{
   if( _x != NULL) { delete _x; }
   if( _w != NULL) { delete _w; }
}


//------------------------------------------------------------------------------
//   Constructor and desctructor
//------------------------------------------------------------------------------
SampleRooFitMgr::SampleRooFitMgr( const string& name, const ConfigReader& cfg ):
   Named( name ),
   SampleGroup(name, cfg )
{
   SampleRooFitMgr::x().setBins(40); // 40 bins for plotting
   _dataset = new RooDataSet(
      Name().c_str(), Name().c_str(),
      RooArgSet(x(),w()),
      RooFit::WeightVar(w())
   );
   for( auto& sample : SampleList() ){
      FillDataSet( *sample );
   }
}

SampleRooFitMgr::~SampleRooFitMgr()
{
   delete _dataset;
   for( auto& dataset : _ext_dataset ){
      delete dataset;
   }
   for( auto& datahist: _binned_dataset ){
      delete datahist;
   }
   for( auto& pdf : _pdf_list){
      delete pdf;
   }
   delete _x;
   delete _w;
}

//------------------------------------------------------------------------------
//   Filling original dataset
//------------------------------------------------------------------------------
void SampleRooFitMgr::FillDataSet( mgr::SampleMgr& sample )
{
   fwlite::Handle<LHEEventProduct>  lheHandle;
   fwlite::Handle<RecoResult>  chiHandle;
   double sample_weight =  1. ;

   if( !sample.IsRealData() ){
      sample_weight = sample.GetSampleWeight();
   }

   unsigned i = 0 ;
   for( sample.Event().toBegin() ; !sample.Event().atEnd() ; ++sample.Event() ){
      printf( "\rSample [%s|%s], Event[%6u/%6llu]..." ,
         Name().c_str(),
         sample.Name().c_str(),
         ++i ,
         sample.Event().size() );
      fflush(stdout);

      chiHandle.getByLabel( sample.Event() , "tstarMassReco" , "ChiSquareResult" , "TstarMassReco" );
      if( !sample.IsRealData() ){
         lheHandle.getByLabel( sample.Event() , "externalLHEProducer" );
      }

      double tstarMass = chiHandle->TstarMass() ;
      double event_weight = 1.0 ;
      if( lheHandle.isValid() && lheHandle->hepeup().XWGTUP <= 0 ) { event_weight = -1.; }
      double weight = event_weight * sample_weight ;
      if( MinMass() <=tstarMass && tstarMass <= MaxMass()  &&
          -1000.    <= weight   && weight    <= 1000. ){
         x() = tstarMass;
         _dataset->add( RooArgSet(x()) , weight );
      }
   }
   cout << "Done!" << endl;
}

//------------------------------------------------------------------------------
//   Dataset augmentation
//------------------------------------------------------------------------------
RooDataSet* SampleRooFitMgr::MakeReduceDataSet(
   const string& name ,
   const RooCmdArg& arg1,
   const RooCmdArg& arg2 )
{
   const string new_name = Name() + name;
   _ext_dataset.push_back( (RooDataSet*) _dataset->reduce(
      RooFit::Name(new_name.c_str()),
      RooFit::Title(new_name.c_str()),
      arg1,
      arg2 ) );
   return _ext_dataset.back();
}

RooDataSet* SampleRooFitMgr::GetReduceDataSet( const string& name )
{
   for( auto dataset : _ext_dataset ){
      if( dataset->GetName() == MakeDataAlias(name) ){
         return dataset;
      }
   }
   return (RooDataSet*)(_dataset);
}

void SampleRooFitMgr::AddDataSet( RooDataSet* x )
{
   _ext_dataset.push_back( x );
}

void SampleRooFitMgr::AddDataHist( RooDataHist* x )
{
   _binned_dataset.push_back( x );
}

RooDataSet* SampleRooFitMgr::GetDataFromAlias( const string& name )
{
   return GetReduceDataSet(name);
}

std::string SampleRooFitMgr::MakeDataAlias( const string& name ) const
{
   return Name() + name;
}

void SampleRooFitMgr::RemoveDataSet( RooDataSet* target )
{
   for( auto x = _ext_dataset.begin() ; x != _ext_dataset.end() ; ++ x  ){
      if( (*x) == target ){
         _ext_dataset.erase(x);
         return;
      }
   }
}

//------------------------------------------------------------------------------
//   PDF Related functions
//------------------------------------------------------------------------------
void SampleRooFitMgr::AddPdf( RooAbsPdf* x ){
   _pdf_list.push_back(x);
}

RooAbsPdf* SampleRooFitMgr::GetPdfFromName( const string& name )
{
   for( auto pdf : _pdf_list ){
      if( pdf->GetName() == name ){
         return pdf;
      }
   }
   return NULL;
}

RooAbsPdf* SampleRooFitMgr::GetPdfFromAlias( const string& alias )
{
   for( auto pdf : _pdf_list ){
      if( pdf->GetName() == MakePdfAlias(alias) ){
         return pdf;
      }
   }
   return NULL;
}

string SampleRooFitMgr::MakePdfAlias( const string& alias ) const
{
   return Name() + alias + "_pdf";
}
