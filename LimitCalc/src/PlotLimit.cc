/*******************************************************************************
*
*  Filename    : PlotLimit.cc
*  Description : Macros For plotting confidence level limits
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/Common/interface/ConfigReader.hpp"
#include "ManagerUtils/Common/interface/STLUtils.hpp"
#include "ManagerUtils/Maths/interface/Intersect.hpp"
#include "ManagerUtils/Maths/interface/Parameter.hpp"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleMgrLoader.hpp"

#include "TstarAnalysis/Common/interface/PlotStyle.hpp"
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/Limit.hpp"

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "TFile.h"
#include "TTree.h"

using namespace std;
using namespace mgr;

/*******************************************************************************
*   Defining plotting control flow
*******************************************************************************/
pair<Parameter, Parameter>
MakeLimitPlot( const std::string& additionaltag )
{
  const map<double, double> xsec = GetXsectionMap();

  TGraph* onesiggraph = MakeCalcGraph( xsec, additionaltag, explim, onesig_up, onesig_down );
  TGraph* twosiggraph = MakeCalcGraph( xsec, additionaltag, explim, twosig_up, twosig_down );
  TGraph* expgraph    = MakeCalcGraph( xsec, additionaltag, explim, skip, skip );
  TGraph* obsgraph    = MakeCalcGraph( xsec, additionaltag, obslim, skip, skip );
  TGraph* theorygraph = MakeTheoryGraph( xsec );

  const double y_max = mgr::GetYmax( twosiggraph, obsgraph );
  const double y_min = mgr::GetYmin( twosiggraph, obsgraph );
  const double x_max = obsgraph->GetX()[obsgraph->GetN()-1];
  const double x_min = obsgraph->GetX()[0];

  // ----- Setting Styles  --------------------------------------------------------
  tstar::SetOneSigmaStyle( onesiggraph );
  tstar::SetTwoSigmaStyle( twosiggraph );
  tstar::SetExpLimitStyle( expgraph );
  tstar::SetObsLimitStyle( obsgraph );
  tstar::SetTheoryStyle( theorygraph );

  // Making object for plotting
  TCanvas* c1 = mgr::NewCanvas();
  mgr::SetSinglePad( c1 );

  TMultiGraph* mg = new TMultiGraph();

  mg->Add( twosiggraph );
  mg->Add( onesiggraph );
  mg->Add( theorygraph );
  mg->Add( expgraph );
  if( limnamer.CheckInput( "drawdata" ) ){
    mg->Add( obsgraph );
  }

  mg->Draw( "AL3" );
  mgr::SetAxis( mg );
  mg->GetXaxis()->SetTitle( "t* Mass (GeV/c^{2})" );// MUST Be after draw!!
  mg->GetYaxis()->SetTitle( "#sigma(pp#rightarrow t*#bar{t}*) (pb)" );// https://root.cern.ch/root/roottalk/roottalk09/0078.html
  mg->GetXaxis()->SetLimits( x_min-50, x_max+50 );
  mg->SetMaximum( y_max*300. );
  mg->SetMinimum( y_min*0.3 );

  // Making legend
  const double legend_x_min = 0.55;
  const double legend_y_min = 0.65;
  TLegend* l                = mgr::NewLegend( legend_x_min, legend_y_min );
  l->AddEntry( obsgraph,    "95% CL_{s} Limit (Obs.)", "l" );
  l->AddEntry( expgraph,    "Expected limit",          "l" );
  l->AddEntry( onesiggraph, "68% expected",            "f" );
  l->AddEntry( twosiggraph, "95% expected",            "f" );
  l->AddEntry( theorygraph, "Theory",                  "lf" );
  l->Draw();


  // Additional titles settings
  mgr::DrawCMSLabel();
  mgr::DrawLuminosity( limnamer.GetExt<double>( "era", "Lumi" ) );

  // Writing additional text
  const string rootlabel = limnamer.GetChannelEXT( "Root Name" );
  const string fitmethod = limnamer.GetExt<string>( "fitmethod", "Full Name" );
  const string funcname  = limnamer.GetExt<string>( "fitfunc", "Full Name" );
  boost::format explimfmt( "Expected Lim (95%% CL.) = %s GeV/c^{2}" );
  boost::format obslimfmt( "Observed Lim (95%% CL.) = %s GeV/c^{2}" );

  mgr::LatexMgr latex;
  latex.SetOrigin( PLOT_X_TEXT_MIN, PLOT_Y_TEXT_MAX, TOP_LEFT )
  .WriteLine( rootlabel )
  .WriteLine( fitmethod )
  .WriteLine( funcname );

  // Cannot have Latex style spacing '\,'
  const Parameter explim_err = GetInterSect( theorygraph, twosiggraph );
  const Parameter obslim_err = GetInterSect( theorygraph, obsgraph );
  const string explim_str    = FloatingPoint( explim_err, 0, false );
  const string obslim_str    = FloatingPoint( obslim_err.CentralValue(), 0, false );

  latex.SetOrigin( PLOT_X_TEXT_MAX, legend_y_min-TEXT_MARGIN, TOP_RIGHT );
  latex.WriteLine( boost::str( explimfmt % explim_str ) );
  if( limnamer.CheckInput( "drawdata" ) ){
    latex.WriteLine( boost::str( obslimfmt % obslim_str ) );
  }


  // ----- Saving and cleaning up  ------------------------------------------------
  c1->SetLogy( kTRUE );
  c1->Update();

  mgr::SaveToROOT( c1, limnamer.PlotRootFile(), limnamer.PlotFileName( "limit", additionaltag ) );
  mgr::SaveToPDF( c1, limnamer.PlotFileName( "limit", additionaltag ) );
  delete onesiggraph;
  delete twosiggraph;
  delete obsgraph;
  delete expgraph;
  delete theorygraph;
  delete mg;
  delete l;
  delete c1;

  return make_pair( explim_err, obslim_err );
}


/*******************************************************************************
*   Filling out map from master file
*******************************************************************************/
map<double, double>
GetXsectionMap()
{
  const string theoryfile = limnamer.SubPackageDir() / "data/excitedtoppair13TeV.dat";
  if( !boost::filesystem::exists( theoryfile ) ){
    cerr << "Error! File containing theoretical cross section doesn't exists!" << endl;
    cerr << "Check file: " << theoryfile << endl;
    throw std::runtime_error( "File not found" );
  }

  ifstream xsec_file( theoryfile );
  string line;
  map<double, double> ans;
  double energy, mass, xsec_value;

  while( getline( xsec_file, line ) ){
    istringstream linestream( line );
    linestream >> energy >> mass >> xsec_value;
    ans[mass] = xsec_value * 1000.;
  }

  xsec_file.close();
  return ans;
}


/*******************************************************************************
*   Object generation functions
*******************************************************************************/
TGraphAsymmErrors*
MakeTheoryGraph( const map<double, double>& xsec )
{
  // Making the PDF/QCD scale error Graph objects for linear interpolation
  const vector<string> siglist = limnamer.MasterConfig().GetStaticStringList( "Signal List" );
  TGraph* errorupgraph         = new TGraph( siglist.size() );
  TGraph* errordowngraph       = new TGraph( siglist.size() );

  for( size_t i = 0; i < siglist.size(); ++i ){
    const auto& sig   = siglist.at( i );
    const double mass = GetInt( sig );

    mgr::SampleMgr mgr( sig );
    mgr::LoadCacheFromFile( mgr, limnamer.CustomFileName( "txt", sig ) );

    const mgr::Parameter pdferr = mgr::Parameter( mgr.SelectedEventCount(),
      mgr.GetCacheDouble( "PDFup" ) - mgr.SelectedEventCount(),
      mgr.SelectedEventCount() - mgr.GetCacheDouble( "PDFdown" )
      ).NormParam();
    const mgr::Parameter scaleerr = mgr::Parameter( mgr.SelectedEventCount(),
      mgr.GetCacheDouble( "scaleup" ) - mgr.SelectedEventCount(),
      mgr.SelectedEventCount() - mgr.GetCacheDouble( "scaledown" )
      ).NormParam();
    const mgr::Parameter toterr = pdferr * scaleerr;

    errorupgraph->SetPoint( i, mass, toterr.RelUpperError() );
    errordowngraph->SetPoint( i, mass, toterr.RelLowerError() );
    cout << toterr.RelLowerError() << " " << toterr.RelUpperError() << endl;
  }


  // Making the main theoretical curve line
  TGraphAsymmErrors* graph = new TGraphAsymmErrors();

  for( const auto& point : xsec ){
    const double mass     = point.first;
    if( mass < 650 || mass > 1650 ) { continue; }
    const double thisxsec = point.second;
    const double xsecup   = thisxsec * errorupgraph->Eval( mass );
    const double xsecdown = thisxsec * errordowngraph->Eval( mass );

    const int newbin = graph->GetN();
    graph->SetPoint( newbin, mass, thisxsec );
    graph->SetPointError(
      newbin,
      0, 0,// No X error for now
      xsecdown,
      xsecup
      );
    // cout << mass << " " <<thisxsec << " " << xsecup << " " << xsecdown << endl;
  }

  for( int i = 0 ; i < graph->GetN() ; ++i ){
    cout << graph->GetX()[i] << " " << graph->GetY()[i]  << " "
         << graph->GetErrorXhigh(i) << " " << graph->GetErrorXlow(i) << " "
         << graph->GetErrorYhigh(i) << " " << graph->GetErrorYlow(i) << endl;
  }


  delete errorupgraph;
  delete errordowngraph;

  return graph;
}

/******************************************************************************/
extern const int explim      = 2;
extern const int obslim      = 5;
extern const int onesig_up   = 3;
extern const int onesig_down = 1;
extern const int twosig_up   = 4;
extern const int twosig_down = 0;
extern const int skip        = -1;


extern TGraph*
MakeCalcGraph(
  const std::map<double, double>& xsec,
  const string& additionaltag,
  const int centralentry,
  const int uperrorentry,
  const int downerrorentry
  )
{
  const vector<string> siglist = limnamer.MasterConfig().GetStaticStringList( "Signal List" );
  TGraphAsymmErrors* graph     = new TGraphAsymmErrors( siglist.size() );

  unsigned bin = 0;

  for( const auto& sig : siglist ){
    const double mass     = GetInt( sig );
    const double expxsec  = xsec.at( mass );
    const string filename = limnamer.RootFileName( "combine", sig, additionaltag );

    // Geting contents of higgs combine output file
    TFile* file = TFile::Open( filename.c_str() );
    if( !file ){
      fprintf( stderr, "Cannot open file (%s), skipping sample for %s\n",
        filename.c_str(),
        sig.c_str() );
      continue;
    }

    double temp;
    TTree* tree = ( (TTree*)file->Get( "limit" ) );
    tree->SetBranchAddress( "limit", &temp );

    // Gettign the entries
    tree->GetEntry( centralentry );
    const double central = temp * expxsec;

    if( uperrorentry != skip ){ tree->GetEntry( uperrorentry ); }
    const double errorup = ( uperrorentry != skip ) ?
                           temp*expxsec - central :  0;

    if( downerrorentry != skip ){ tree->GetEntry( downerrorentry ); }
    const double errordown = ( downerrorentry != skip ) ?
                             central - temp*expxsec :  0;

    // closing file
    file->Close();

    graph->SetPoint( bin, mass, central );
    graph->SetPointError( bin, 50, 50, errordown, errorup );

    //printf( "%lf %lf %lf %lf\n", mass, central, errorup, errordown );
    ++bin;
  }

  return graph;
}


/*******************************************************************************
*   Function for calculating intersect points
*******************************************************************************/

Parameter
GetInterSect( const TGraph* graph, const TGraph* errgraph )
{
  double y             = 0;
  double explimit      = errgraph->GetX()[0];
  double explimit_up   = errgraph->GetX()[0];
  double explimit_down = errgraph->GetX()[0];

  for( int i = 0; i < graph->GetN()-1; ++i ){
    for( int j = 0; j < errgraph->GetN()-1; ++j ){

      const double graphmass     = graph->GetX()[i];
      const double graphmassnext = graph->GetX()[i+1];
      const double graphval      = graph->GetY()[i];
      const double graphvalnext  = graph->GetY()[i+1];

      const double errgraphmass     = errgraph->GetX()[j];
      const double errgraphmassnext = errgraph->GetX()[j+1];
      const double errgraphval      = errgraph->GetY()[j];
      const double errgraphvalnext  = errgraph->GetY()[j+1];
      const double hierr            = errgraph->GetErrorYhigh( j ) + errgraphval;
      const double hierrnext        = errgraph->GetErrorYhigh( j+1 ) + errgraphvalnext;
      const double loerr            = errgraphval - errgraph->GetErrorYlow( j );
      const double loerrnext        = errgraphvalnext - errgraph->GetErrorYlow( j+1 );

      Intersect(
        graphmass, graphval,
        graphmassnext, graphvalnext,
        errgraphmass, errgraphval,
        errgraphmassnext, errgraphvalnext,
        explimit, y
        );

      Intersect(
        graphmass, graphval,
        graphmassnext, graphvalnext,
        errgraphmass, hierr,
        errgraphmassnext, hierrnext,
        explimit_up, y
        );

      Intersect(
        graphmass, graphval,
        graphmassnext, graphvalnext,
        errgraphmass, loerr,
        errgraphmassnext, loerrnext,
        explimit_down, y
        );
    }
  }

  return Parameter( explimit, explimit_down- explimit, explimit_up - explimit );
}
