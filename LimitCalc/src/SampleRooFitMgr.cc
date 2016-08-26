/*******************************************************************************
 *
 *  Filename    : SampleRooFitMgr.cc
 *  Description : implementation of roofit manager class
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"

using namespace std;
using namespace mgr;
//------------------------------------------------------------------------------
//   Static member function delcarations
//------------------------------------------------------------------------------
double SampleRooFitMgr::_min_mass = 0 ;
double SampleRooFitMgr::_max_mass = 0 ;
RooRealVar* SampleRooFitMgr::_x   = NULL;
RooRealVar* SampleRooFitMgr::_w   = NULL;

void SampleRooFitMgr::InitStaticVars( const double min, const double max )
{
   ClearStaticVars();
   _x = new RooRealVar( "x" , "M_{t+g}"      , min   , max , "GeV/c^{2}" );
   _w = new RooRealVar( "w" , "event_weight" , -1000., 1000, "");
   _min_mass = min;
   _max_mass = max;
}

void SampleRooFitMgr::ClearStaticVars()
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
   definesets();
   for( auto& sample : SampleList() ){
      fillsets( *sample );
   }
}

SampleRooFitMgr::~SampleRooFitMgr()
{
   for( auto& var : _varlist ) {
      printf("%30s %8.4lf %8.4f\n", var->GetName(), var->getVal(), var->getError() );
      fflush(stdout);
      delete var;
   }
   for( auto& set : _setlist ) { delete set; }
   for( auto& pdf : _pdflist ) { delete pdf; }
}

//------------------------------------------------------------------------------
//   Variable access functions
//------------------------------------------------------------------------------
RooRealVar*  SampleRooFitMgr::NewVar( const std::string& name, const double min, const double max )
{
   if( Var(name) ){ // If it already exists
      RooRealVar* var = Var(name);
      var->setConstant(kFALSE);
      var->setMax(max);
      var->setMin(min);
      return var;
   } else {
      const string varname = Name()+name;
      RooRealVar* var = new RooRealVar( varname.c_str(), varname.c_str(), min, max );
      _varlist.push_back( var );
      return var;
   }
}

RooRealVar*  SampleRooFitMgr::NewVar( const std::string& name, const double init, const double min, const double max )
{
   if( Var(name) ){ // If it already exists
      RooRealVar* var = Var(name);
      var->setConstant(kFALSE);
      var->setMax(max);
      var->setMin(min);
      *var = init;
      return var;
   } else {
      const string varname = Name()+name;
      RooRealVar* var = new RooRealVar( varname.c_str(), varname.c_str(), init, min, max );
      _varlist.push_back( var );
      return  var;
   }
}

RooRealVar* SampleRooFitMgr::Var( const std::string& name )
{
   for( auto& var : _varlist ){
      if( (Name()+name)  == var->GetName() ){
         return var;
      }
   }
   return NULL;
}
const RooRealVar* SampleRooFitMgr::Var( const std::string& name ) const
{
   for( const auto& var : _varlist ){
      if( (Name()+name)  == var->GetName() ){
         return var;
      }
   }
   return NULL;
}

vector<string>  SampleRooFitMgr::AvailableVarNameList()   const
{
   vector<string> ans;
   for( const auto& var : _varlist ){
      string name = var->GetName();
      name.erase( 0 , Name().length() );
      ans.push_back( name );
   }
   return ans;
}

vector<RooRealVar*> SampleRooFitMgr::VarContains( const string& substring) const
{
   vector<RooRealVar*> ans;
   for( const auto& var : _varlist ){
      string varname = var->GetName();
      if( varname.find(substring)!= string::npos ){
         ans.push_back(var);
      }
   }
   return ans;
}

void SampleRooFitMgr::SetConstant( bool x )
{
   for( auto& var : _varlist ){
      var->setConstant(x);
   }
}

//------------------------------------------------------------------------------
//   Dataset argument options
//------------------------------------------------------------------------------
RooDataSet* SampleRooFitMgr::NewDataSet( const std::string& name )
{
   if(DataSet(name)){
      return DataSet(name);
   } else {
      const string setname = Name() + name;
      RooDataSet* set = new RooDataSet(
         setname.c_str(), setname.c_str(),
         RooArgSet( x(), w() ),
         RooFit::WeightVar(w())
      );
      _setlist.push_back( set );
      return set;
   }
}

void  SampleRooFitMgr::AddDataSet( RooDataSet* set )
{
   const string newname = Name() + set->GetName();
   set->SetName( newname.c_str() );
   _setlist.push_back( set );
}

RooDataSet*  SampleRooFitMgr::DataSet( const std::string& name )
{
   for( auto& set : _setlist ){
      if( (Name()+name)  == set->GetName() ){
         return set;
      }
   }
   return NULL;
}

const RooDataSet*  SampleRooFitMgr::DataSet( const std::string& name ) const
{
   for( const auto& set : _setlist ){
      if( (Name()+name)  == set->GetName() ){
         return set;
      }
   }
   return NULL;
}

vector<std::string>  SampleRooFitMgr::AvailableSetNameList() const
{
   vector<string> ans;
   for( const auto& set : _setlist ){
      string name = set->GetName();
      name.erase( 0 , Name().length() );
      ans.push_back( name );
   }
   return ans;
}

void SampleRooFitMgr::RemoveDataSet( const std::string& name )
{
   for( auto x = _setlist.begin() ; x != _setlist.end() ; ++x ){
      if( (*x)->GetName() == Name() + name ) {
         _setlist.erase(x);
         return;
      }
   }
}

//------------------------------------------------------------------------------
//   PDF Access functions, for making pdf for analysis see SampleRooFitMgr_MakePdf.cc
//------------------------------------------------------------------------------
void  SampleRooFitMgr::AddPdf( RooAbsPdf*  pdf)
{
   const string newname = Name() + pdf->GetName();
   pdf->SetName( newname.c_str() );
   _pdflist.push_back( pdf );
}
RooAbsPdf*  SampleRooFitMgr::Pdf( const std::string& name )
{
   for( auto& pdf : _pdflist ){
      if( (Name()+name)  == pdf->GetName() ){
         return pdf;
      }
   }
   return NULL;
}

const RooAbsPdf*  SampleRooFitMgr::Pdf( const std::string& name ) const
{
   for( const auto& pdf : _pdflist ){
      if( (Name()+name)  == pdf->GetName() ){
         return pdf;
      }
   }
   return NULL;
}

vector<std::string>  SampleRooFitMgr::AvailablePdfNameList() const
{
   vector<string> ans;
   for( const auto& pdf : _pdflist ){
      string name = pdf->GetName();
      name.erase( 0 , Name().length() );
      ans.push_back( name );
   }
   return ans;
}
