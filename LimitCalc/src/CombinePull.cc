/*******************************************************************************
*
*  Filename    : CombinePull.
*  Description : implementing combine pull Functions
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/CombinePull.hpp"
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"

#include "ManagerUtils/Common/interface/ConfigReader.hpp"
#include "ManagerUtils/Maths/interface/Parameter.hpp"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "ManagerUtils/SysUtils/interface/HiggsCombineSubmitter.hpp"
#include "ManagerUtils/SysUtils/interface/PathUtils.hpp"
#include "TstarAnalysis/Common/interface/PlotStyle.hpp"

#include <boost/format.hpp>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

#include "RooFitResult.h"
#include "RooRealVar.h"
#include "TFile.h"

using namespace std;
using mgr::Parameter;

/******************************************************************************/

void
MakeCombinePull()
{
  const mgr::ConfigReader higgscfg( limnamer.SettingsDir() / "higgs_combine_settings.json" );

  const string cardfile = limnamer.TextFileName( "card", {} );

  const string scriptfilename = limnamer.TextFileName( "combpull", {} );

  const string storeroot = limnamer.RootFileName( "combpull", {} );

  cout << cardfile << endl;


  ofstream scriptfile( scriptfilename );

  scriptfile << "#!/bin/bash" << endl;

  scriptfile << boost::format( "cd %s/%s/src" )
    % higgscfg.GetStaticString( "Store Path" )
    % higgscfg.GetStaticString( "CMSSW Version" )
             << endl;

  scriptfile <<  "eval `scramv1 runtime -sh 2> /dev/null`" << endl;

  scriptfile << boost::format( "combine -M MaxLikelihoodFit %s" ) % cardfile << endl;

  scriptfile << boost::format( "cp mlfit.root %s" ) % storeroot << endl;

  scriptfile.close();

  system( ( "chmod +x " + scriptfilename ).c_str() );
  system( scriptfilename.c_str() );

  system( "sleep 5" );
}

/******************************************************************************/

static Parameter
MakeVarPull( const RooArgList& post, const RooArgList& pre, const string& name )
{
  RooRealVar* postvar = (RooRealVar*)( post.find( name.c_str() ) );
  RooRealVar* prevar  = (RooRealVar*)( pre.find( name.c_str() ) );

  if( postvar && prevar ){
    const double preval  = prevar->getVal();
    const double preerr  = prevar->getError();
    const double postval = postvar->getVal();
    const double posterr = postvar->getError();
    const double pull    = ( postval - preval ) / preerr;
    const double pullerr = posterr / preerr;
    return Parameter( pull, pullerr, pullerr );
  } else {
    return Parameter( 0, 0, 0 );
  }
}

void
MakePullPlot()
{
  TFile* file               = TFile::Open( limnamer.RootFileName( "combpull", {} ).c_str(), "READ" );
  const RooArgList& postfit = ( (RooFitResult*)( file->Get( "fit_s" ) ) )->floatParsFinal();
  const RooArgSet& prefit   = *( (RooArgSet*)( file->Get( "nuisances_prefit" ) ) );

  // Object nameing settgs
  const string datatag = limnamer.GetChannelEXT( "Data Prefix" )
                         + limnamer.GetExtName( "era", "Data Postfix" );

  vector<pair<string, Parameter> > pulllist;

  // Fitting paramter varibles
  const string avarname = datatag + limnamer.InputStr( "mass" ) + "simfitbga";
  const string bvarname = datatag + limnamer.InputStr( "mass" ) + "simfitbgb";

  pulllist.emplace_back( "Bkg. parm 1", MakeVarPull( postfit, prefit, avarname ) );
  pulllist.emplace_back( "Bkg. parm 2", MakeVarPull( postfit, prefit, bvarname ) );

  // Shape uncertainties
  for( size_t i = 0; i < uncsource.size(); ++i ){
    boost::format shapename( "%s%s%u_%s%s%s" );
    const string upname = str( shapename % limnamer.InputStr( "mass" ) % "keymastercoeff" % ( 2*i )
      % uncsource.at( i ) % "Up"     %"key" );
    const string dwname = str( shapename %limnamer.InputStr( "mass" ) % "keymastercoeff" % ( 2*i+1 )
      % uncsource.at( i ) % "Down" %"key" );

    const Parameter uppull = MakeVarPull( postfit, prefit, upname );
    const Parameter dwpull = MakeVarPull( postfit, prefit, dwname );

    const Parameter pull(
      uppull.CentralValue() + dwpull.CentralValue(),
      ( uppull.AbsUpperError() + dwpull.AbsUpperError() ) /2,
      ( uppull.AbsUpperError() + dwpull.AbsUpperError() ) /2
      );

    pulllist.emplace_back( uncsource.at( i ), pull );
  }

  // Normalization disabled for now.
  // for( const auto& unc : uncsource ){
  //   if( unc != "model" )
  //   pulllist.emplace_back( unc + " norm." , MakeVarPull(postfit,prefit,unc) );
  // }

  // Misc options
  pulllist.emplace_back( "lep sys.", MakeVarPull( postfit, prefit, "lepsys" ) );
  pulllist.emplace_back( "lumi.",    MakeVarPull( postfit, prefit, "lumi" ) );
  pulllist.emplace_back( "stat.",    MakeVarPull( postfit, prefit, "statunc" ) );

  for( const auto& mypair : pulllist ){
    cout << mypair.first << " " << mypair.second << endl;
  }

  delete file;


  // Beginning plotting
  TCanvas* c = mgr::NewCanvas();

  TH1D* pullhist = new TH1D( "pull", "pull", pulllist.size(), 0, pulllist.size() );

  for( size_t i = 0; i < pulllist.size(); ++i ){
    const string name    = pulllist.at( i ).first;
    const Parameter pull = pulllist.at( i ).second;
    pullhist->GetXaxis()->SetBinLabel( i+1, name.c_str() );
    pullhist->SetBinContent( i+1, pull.CentralValue() );
    pullhist->SetBinError( i+1, pull.AbsAvgError() );
  }

  mgr::SetSinglePad( c );
  c->SetBottomMargin( 1.5 * PLOT_Y_MIN );

  const double ymax = std::max( mgr::GetYmax( pullhist ) , 2.0 );
  mgr::SetAxis( pullhist );
  tstar::SetDataStyle( pullhist );
  pullhist->Draw( PS_DATA );
  pullhist->SetTitle( "" );
  pullhist->SetStats( 0 );
  pullhist->GetYaxis()->SetTitle( "Post-fit pull" );
  pullhist->SetMaximum( ymax * 1.2 );
  pullhist->SetMinimum( ymax * -1.2 );

  mgr::DrawCMSLabel( PRELIMINARY );
  mgr::DrawLuminosity( limnamer.GetExtDouble( "era", "Lumi" ) );

  const double xmin = 0;
  const double xmax = pulllist.size();
  TLine* cen        = new TLine( xmin, 0, xmax, 0 );
  TLine* midtop     = new TLine( xmin, 0.5, xmax, 0.5 );
  TLine* top        = new TLine( xmin, 1, xmax, 1 );
  TLine* midbot     = new TLine( xmin, -0.5, xmax, -0.5 );
  TLine* bot        = new TLine( xmin, -1, xmax, -1 );
  cen->Draw();
  midtop->Draw();
  top->Draw();
  midbot->Draw();
  bot->Draw();
  pullhist->Draw( PS_DATA PS_SAME );

  cen->SetLineColor( kGray );
  cen->SetLineStyle( 1 );
  cen->SetLineWidth( 3 );
  midtop->SetLineColor( KBLUE );
  midbot->SetLineColor( KBLUE );
  midtop->SetLineWidth( 3 );
  midbot->SetLineWidth( 3 );
  midtop->SetLineStyle( 2 );
  midbot->SetLineStyle( 2 );
  top->SetLineColor( KRED );
  bot->SetLineColor( KRED );
  top->SetLineWidth( 3 );
  bot->SetLineWidth( 3 );
  top->SetLineStyle( 3 );
  bot->SetLineStyle( 3 );

  mgr::LatexMgr latex;
  const string masstag = str( boost::format( "t*=%dGeV/c^{2}" ) % GetInt( limnamer.InputStr( "mass" ) ) );
  latex.SetOrigin( PLOT_X_TEXT_MIN, PLOT_Y_TEXT_MAX )
  .WriteLine( limnamer.GetExtName( "channel", "Root Name" ) )
  .WriteLine( masstag );

  mgr::SaveToPDF( c, limnamer.PlotFileName( "combpull", {} ) );

  delete c;
  delete pullhist;
}