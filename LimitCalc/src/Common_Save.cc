/*******************************************************************************
*
*  Filename    : Common_Save.cc
*  Description : Common functions required by fitting methods
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/Maths/interface/Parameter.hpp"
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"

#include "RooAddPdf.h"
#include "RooDataSet.h"
#include "RooFitResult.h"
#include "RooWorkspace.h"
#include "TFile.h"

#include <boost/format.hpp>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
using namespace mgr;

/*******************************************************************************
*   Save results
*******************************************************************************/
static const string ws_name = "wspace";

void
SaveRooWorkSpace(
  RooAbsData*                data,
  const vector<RooAbsPdf*>&  pdflist,
  const vector<RooAbsReal*>& funclist
  )
{
  const string roofit_file = limnamer.RootFileName( "roofitobj" );
  RooWorkspace ws( ws_name.c_str(), ws_name.c_str() );

  cout << "Saving RooFit objects to " << roofit_file << endl;
  ws.import( *data );

  for( auto& pdf  : pdflist ){
    ws.import( *pdf  );
  }

  for( auto& func : funclist ){
    ws.import( *func );
  }

  ws.writeToFile( roofit_file.c_str() );

}

/******************************************************************************/

void
MakeCardCommon(
  ofstream&   cardfile,
  RooAbsData* data,
  RooAbsPdf*  bg_pdf,
  RooAbsPdf*  sig_pdf
  )
{
  cardfile << "imax 1" << endl;
  cardfile << "jmax *" << endl;
  cardfile << "kmax *" << endl;
  cardfile << "----------------------------------------" << endl;

  // Printing Objects
  cardfile << boost::format( "shapes %10s %15s %30s %s:%s" )
    %  "data_obs"
    %  limnamer.GetChannel()
    %  limnamer.RootFileName( "roofitobj" )
    %  ws_name
    %  data->GetName()
           << endl;

  cardfile << boost::format( "shapes %10s %15s %30s %s:%s" )
    % "bg"
    % limnamer.GetChannel()
    % limnamer.RootFileName( "roofitobj" )
    % ws_name
    % bg_pdf->GetName()
           << endl;

  cardfile << boost::format( "shapes %10s %15s %30s %s:%s" )
    % "sig"
    % limnamer.GetChannel()
    % limnamer.RootFileName( "roofitobj" )
    % ws_name
    % sig_pdf->GetName()
           << endl;
  cardfile << "----------------------------------------"  << endl;

  // Printing data correspondence
  cardfile <<  boost::format( "%12s %s" ) % "bin" % limnamer.GetChannel() << endl;
  cardfile <<  boost::format( "%12s %lg" )% "observation" % data->sumEntries() << endl;
  cardfile <<  "----------------------------------------" << endl;

  cardfile << boost::format( "%12s %15s %15s" )
    % "bin"
    % limnamer.GetChannel()
    % limnamer.GetChannel()
           << endl;

  cardfile << boost::format( "%12s %15s %15s" ) % "process"% "sig" % "bg" << endl;
  cardfile << boost::format( "%12s %15s %15s" ) % "process"% "-1" %  "1" << endl;

}

/******************************************************************************/

void
PrintNuisanceFloats(
  ofstream&        cardfile,
  const string&    nuisance_name,
  const string&    nuisance_type,
  const Parameter& sig_nuisance,
  const Parameter& bkg_nuisance
  )
{

  cardfile << boost::format( "%-20s %3s %15s %15s" )
    % nuisance_name
    % nuisance_type
    % HiggsDataCard( sig_nuisance )
    % HiggsDataCard( bkg_nuisance )
           << endl;
}

/******************************************************************************/

void
PrintFloatParam(
  ofstream&         cardfile,
  const RooRealVar* var
  )
{
  cardfile << boost::format( "%-45s %-10s %lf %lf" )
    % var->GetName()
    % "param"
    % var->getVal()
    % var->getError()
           << endl;
}

/******************************************************************************/

void
PrintFlatParam(
  ofstream&         cardfile,
  const RooRealVar* var
  )
{
  cardfile << boost::format( "%-45s %s %lf %lf" )
    % var->GetName()
    % "param"
    % var->getVal()
    % var->getMax()
           << endl;
}
