/*******************************************************************************
*
*  Filename    : PlotLimit_DisableNuisance.cc
*  Description : Functions for disabling nuisable parameter
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/Maths/interface/Parameter/Format.hpp"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "TstarAnalysis/Common/interface/PlotStyle.hpp"
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/DisableNuisance.hpp"

#include <boost/format.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "TH1D.h"
#include "TTree.h"


using namespace std;


extern const vector<string> unclist = {
  "",
  "simfitbg",
//  "simfitbgsys",
  "jec",
  "jetres",
  "btag",
  "pu",
  "lep",
  "pdf",
  "scale",
  "model",
  "stat",
  "lumi",
};


/******************************************************************************/

extern void
MakeNewCard( const std::string& masspoint, const std::string nuisance )
{
  const string origfilename = limnamer.TextFileName( "card", masspoint );
  const string newfilename  = limnamer.TextFileName( "card", masspoint, nuisance );
  ifstream originalfile( origfilename );
  ofstream newfile( newfilename );
  string originalline;

  cout << "Output to " << newfilename << endl;

  while( getline( originalfile, originalline ) ){
    istringstream iss( originalline );
    string firstword;
    iss >> firstword;// Getting first word only
    if( firstword.find( nuisance ) != string::npos ){
      // special case for simfitbg
      if( nuisance == "simfitbg" && firstword.find( "sys" ) != string::npos ){
        newfile << originalline << endl;
      } else {
        cout << "# Disabled line: " << originalline << endl;
      }
    } else if( firstword == "kmax" ){
      newfile << "kmax *" << endl;// Saving kmax
    } else {
      newfile << originalline << endl;
    }
  }

  originalfile.close();
  newfile.close();
}

/******************************************************************************/

extern void
MakeLimitTable( const std::map<std::string, std::pair<mgr::Parameter, mgr::Parameter> >& limitresults )
{
  const string filename = limnamer.TexFileName( "disable_nuisance" );
  ofstream limiteffect( filename  );

  boost::format tableentry( "%15s & %30s & %30s\\\\" );

  limiteffect << "\\begin{tabular}{|l|cc|}" << endl;
  limiteffect << "\\hline" << endl;
  limiteffect << tableentry % "" % "Expected Limit" % "Observed Limit" << endl;
  limiteffect << "\\hline" << endl;

  const mgr::Parameter explimit = limitresults.at( "" ).first;
  const mgr::Parameter obslimit = limitresults.at( "" ).second;

  for( const auto& resultpair : limitresults ){
    const auto& disabled  = resultpair.first;
    const auto& limitpair = resultpair.second;

    if( disabled == "" ){
      const string obslimstring = ( limnamer.CheckInput( "drawdata" ) ) ? mgr::FloatingPoint( obslimit, 1 ) : "--";
      limiteffect << tableentry
        % "Central value"
        % mgr::FloatingPoint( explimit.CentralValue(), 0 )
        % obslimstring
                  << endl;
      limiteffect << "\\hline" << endl;
    } else {
      const double explimdiff = fabs( explimit.CentralValue() - limitpair.first.CentralValue() );
      const double obslimdiff = fabs( obslimit.CentralValue() - limitpair.second.CentralValue() );

      const string explimstr = mgr::FloatingPoint( explimdiff, 1 );
      const string obslimstr = ( limnamer.CheckInput( "drawdata" ) ) ? mgr::FloatingPoint( obslimdiff, 1 ) : "--";
      limiteffect << tableentry
        % disabled
        % explimstr
        % obslimstr
                  << endl;
    }
  }

  limiteffect << "\\hline" << endl;
  limiteffect << "\\end{tabular}" << endl;
  limiteffect.close();

  cout << "Writting results to tex file: " << filename << endl;
}

/******************************************************************************/

void
MakeDeltaRPlot()
{
  /// For binned axis
  TH1D* obsplot              = new TH1D( "", "", unclist.size(), 0, unclist.size() );
  TH1D* expplot              = new TH1D( "", "", unclist.size(), 0, unclist.size() );
  TGraphAsymmErrors* exp1sig = new TGraphAsymmErrors( unclist.size() );
  TGraphAsymmErrors* exp2sig = new TGraphAsymmErrors( unclist.size() );

  const string masspoint = limnamer.GetInput<string>( "masspoint" );

  // Making list of uncertainties
  for( size_t i = 0; i < unclist.size(); ++i  ){
    const string unc         = unclist.at( i );
    const string combinefile = limnamer.RootFileName( "combine", masspoint, unc );

    // Getting the values
    TFile* file = TFile::Open( combinefile.c_str() );
    if( !file ){
      cerr << boost::format( "Cannot open file (%s), skipping sample for %s" )
        % combinefile
        % masspoint
           << endl;
    }

    double temp;
    TTree* tree = ( (TTree*)file->Get( "limit" ) );
    tree->SetBranchAddress( "limit", &temp );

    // Gettign the entries
    tree->GetEntry( 0 );
    const double exp2down = temp;
    tree->GetEntry( 1 );
    const double exp1down = temp;
    tree->GetEntry( 2 );
    const double explim = temp;
    tree->GetEntry( 3 );
    const double exp1up = temp;
    tree->GetEntry( 4 );
    const double exp2up = temp;
    tree->GetEntry( 5 );
    const double obslim = temp;

    // Setting binned content
    obsplot->SetBinContent( i+1, obslim );
    obsplot->SetBinError( i+1, 0.000000001 );

    expplot->SetBinContent( i+1, explim );
    expplot->SetBinError( i+1, 0.000000001 );

    const string label =
      ( unc == "" ) ? ( "none" ) :
      ( unc == "simfitbg" ) ? ( "bkg. (fix at fit)" ) :
      // ( unc == "simfitbgsys" ) ? ( "bkg. sys" ) :
      ( unc == "jec" ) ? "JEC" :
      ( unc == "jetres" ) ? "JER" :
      ( unc == "btag" ) ? "b-tag" :
      ( unc == "pu" ) ? "pileup" :
      ( unc == "lep" ) ? "lep" :
      ( unc == "pdf" ) ? "PDF" :
      ( unc == "scale" ) ? "QCD" :
      ( unc == "mode" ) ? "kernal approx" :
      ( unc == "stat" ) ? "selec eff." :
      unc;
    expplot->GetXaxis()->SetBinLabel( i+1, label.c_str() );


    exp1sig->SetPoint( i, i+0.5, explim );
    exp1sig->SetPointError( i, 0.5, 0.5, explim-exp1down, exp1up-explim );
    exp2sig->SetPoint( i, i+0.5, explim );
    exp2sig->SetPointError( i, 0.5, 0.5, explim-exp2down, exp2up-explim );

    delete file;
  }

  TCanvas* c = mgr::NewCanvas();
  expplot->Draw( "AXIS" );// Drawing axis only
  exp2sig->Draw( "SAME 2" );
  exp1sig->Draw( "SAME 2" );
  expplot->Draw( "SAME HIST" );

  if( limnamer.CheckInput( "drawdata" ) ){
    obsplot->Draw( "SAME" PS_DATA );
  }

  // Basic styling
  tstar::SetOneSigmaStyle( exp1sig );
  tstar::SetTwoSigmaStyle( exp2sig );

  expplot->SetLineColor( KRED );
  obsplot->SetLineColor( kBlack );

  // Axis setting
  expplot->GetYaxis()->SetTitle( "Limit (r)" );
  expplot->GetXaxis()->SetTitle( "Disabled nuisance params" );

  expplot->SetStats( 0 );
  obsplot->SetStats( 0 );
  mgr::SetSinglePad( c );
  mgr::SetAxis( expplot );
  expplot->GetXaxis()->SetTitleOffset( 1.1 );
  c->SetBottomMargin( 0.18 );

  TLegend* leg = mgr::NewLegend( 0.6, 0.7 );
  leg->AddEntry( (TObject*)NULL, "95% CL_{s} upper limit", "" );
  leg->AddEntry( obsplot,        "Observed",               "l" );
  leg->AddEntry( expplot,        "Medium expected",        "l" );
  leg->AddEntry( exp1sig,        "68% expected",           "f" );
  leg->AddEntry( exp2sig,        "95% expected",           "f" );
  leg->Draw();

  mgr::DrawCMSLabel();
  mgr::DrawLuminosity( limnamer.GetExt<double>( "era", "Lumi" ) );
  const string massline = str( boost::format( "M_{t*}=%dGeV/c^{2}" ) % GetInt( masspoint ) );

  mgr::LatexMgr latex;
  latex.SetOrigin( PLOT_X_MIN, PLOT_Y_MAX+(TEXT_MARGIN), TOP_LEFT )
  .WriteLine( limnamer.GetChannelEXT( "Root Name" ) )
  .SetOrigin( PLOT_X_TEXT_MAX, 0.7-TEXT_MARGIN, TOP_RIGHT )
  .WriteLine( massline );

  // Range setting
  const double ymax = std::max( obsplot->GetMaximum(), mgr::GetYmax( exp2sig ) );
  const double ymin = std::min( obsplot->GetMinimum(), mgr::GetYmin( exp2sig ) );
  expplot->SetMaximum( ymax*31 );
  expplot->SetMinimum( ymin/3.1 );

  c->SetLogy( kTRUE );
  mgr::SaveToPDF( c, limnamer.PlotFileName( "disable_nuisance", masspoint ) );

  delete c;
  delete obsplot;
  delete expplot;
  delete exp1sig;
  delete exp2sig;
}
