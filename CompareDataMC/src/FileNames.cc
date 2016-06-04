/*******************************************************************************
 *
 *  Filename    : FileNames.cc
 *  Description : Implementations for getting the files save locations
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include <string>
#include <cstdlib>

using namespace std;

//------------------------------------------------------------------------------
//   Static constants
//------------------------------------------------------------------------------
static const string base_dir = getenv( "CMSSW_BASE");
static const string work_dir = base_dir + "/src/TstarAnalysis/CompareDataMC";
static string channel_name;

void SetChannelType( const string& x ) { channel_name = x; }
string GetChannel() { return channel_name; }

string GetChannelPlotLabel(){
   if( channel_name == "MuonSignal" ){
      return "#mu signal channel";
   }else if( channel_name == "ElectronSignal") {
      return "e signal channel";
   } else {
      return "Unknown channel";
   }
}

string GetJsonFile() { return work_dir + "/data/" + channel_name + ".json"; }
string ResultsDir() { return work_dir + "/results/" + channel_name + "/" ; }
string GetTexSummaryFile() { return ResultsDir() + "summary.tex" ; }
string GetKinematicPlotFile( const string& plot_name ) { return ResultsDir() + plot_name + ".png"; }

//------------------------------------------------------------------------------
//   RooFit Related functions
//------------------------------------------------------------------------------
static string method_name ;
static string fit_func;

void SetMethod( const string& x ) { method_name = x ; }

string GetMethod() { return method_name; }

string GetMethodLabel()
{
   if ( method_name == "Template") {
      return "MC template";
   } else if( method_name == "SimFit" ){
      return "simultaneous fit";
   } else if( method_name == "BiasTemplate" ){
      return "bias check/template";
   } else if( method_name == "BiasSimFit" ){
      return "bias check/simultaneous fit";
   } else {
      return "";
   }
}

void SetFitFunc( const string& x ) { fit_func = x ; }

string GetFitFunc() { return fit_func; }

string GetFitFuncTag()
{
   if( fit_func == "Fermi" ){
      return "Fermi function";
   } else if( fit_func == "Exo" ){
      return "EXO function";
   } else {
      return "";
   }
}


string GetFitFuncFormula()
{
   if( fit_func == "Fermi" ){
      return "f(m) = N #left(1 + exp#left( #frac{m-a}{b} #right)#right)^{-1}";
   } else if ( fit_func == "Exo" ){
      return "f(m) = N #left( (1-m)^{p_{1}} #right) / #left( m^{(p_{2} + p_{3} ln(m))} #right)";
   }
   return "";
}

string GetFitResults()
{
   char buffer[1024];
   sprintf( buffer, "%s/fitresults_%s_%s.root",
      ResultsDir().c_str(),
      GetMethod().c_str(),
      GetFitFunc().c_str()
   );
   return buffer;
}

string GetFitCompare( const string& x )
{
   char buffer[1024];
   sprintf( buffer, "%s/fitresults_%s_%s.png",
      ResultsDir().c_str(),
      GetFitFunc().c_str(),
      x.c_str()
   );
   return buffer;
}

string GetCardFile( const string& masspoint )
{
   char buffer[1024];
   sprintf( buffer, "%s/card_%s_%s_%s.txt" ,
      ResultsDir().c_str(),
      GetMethod().c_str(),
      GetFitFunc().c_str(),
      masspoint.c_str()
   );
   return buffer;
}

string GetRooObjFile()
{
   char buffer[1024];
   sprintf( buffer, "%s/roofitobj_%s_%s.root",
      ResultsDir().c_str(),
      GetMethod().c_str(),
      GetFitFunc().c_str()
   );
   return buffer;
}

string GetRooObjPlot( const string& tags )
{
   char buffer[1024];
   sprintf( buffer, "%s/roofitplots_%s_%s_%s.png",
      ResultsDir().c_str(),
      GetMethod().c_str(),
      GetFitFunc().c_str(),
      tags.c_str()
   );
   return buffer ;
}

//------------------------------------------------------------------------------
//   HC Related
//------------------------------------------------------------------------------
static string hc_method;

void SetHCMethod( const string& x ) { hc_method = x; }

string GetHCMethod() { return hc_method; }


string HCRawOutputFile( int mass )
{
   char buffer[1024];
   sprintf( buffer , "%s/higgsCombineTest.%s.mH%d.root",
      getenv("PWD"),
      GetHCMethod().c_str(),
      mass
   );
   return buffer;
}

string HCStoreFile( int mass )
{
   char buffer[1024];
   sprintf( buffer , "%s/higgscombine_%s_%s_%s_%d.root" ,
      ResultsDir().c_str(),
      GetMethod().c_str(),
      GetFitFunc().c_str(),
      GetHCMethod().c_str(),
      mass  );
   return buffer;
}

string LimitPlotFile()
{
   char buffer[1024];
   sprintf( buffer , "%s/limit_%s_%s_%s.png",
      ResultsDir().c_str(),
      GetMethod().c_str(),
      GetFitFunc().c_str(),
      GetHCMethod().c_str()
   );
   return buffer;
}

//------------------------------------------------------------------------------
//   Misc. Functions
//------------------------------------------------------------------------------
string data_set_name = "";

int GetSignalMass( const string& name )
{
   string ans_string;
   for( auto y : name ){
      if( y >= '0' && y <= '9'){
         ans_string.push_back(y);
      }
   }
   return stoi(ans_string);
}

string GetDataSetName()
{
   return data_set_name;
}

void SetDataSetName( const string& x )
{
   data_set_name = x;
}
