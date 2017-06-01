/*******************************************************************************
*
*  Filename    : MakeErrHist.cc
*  Description : Implementatin of functions defined in include/MakeErrHist.hpp
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/CompareDataMC/interface/MakeErrHist.hpp"
#include "TstarAnalysis/CompareDataMC/interface/SampleErrHistMgr.hpp"

#include "ManagerUtils/Maths/interface/Parameter.hpp"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "TstarAnalysis/Common/interface/NameParse.hpp"
#include "TstarAnalysis/Common/interface/PlotStyle.hpp"
#include "TstarAnalysis/CompareDataMC/interface/Compare_Common.hpp"
#include "TstarAnalysis/CompareDataMC/interface/MakeHist.hpp"
#include "TstarAnalysis/CompareDataMC/interface/SampleHistMgr.hpp"

#include <boost/format.hpp>
#include <boost/range/adaptor/reversed.hpp>
using namespace std;
using namespace mgr;

void
MakeFullComparePlot(
  SampleErrHistMgr*          datamgr,
  vector<SampleErrHistMgr*>& background,
  SampleErrHistMgr*          signalmgr,
  const string&              label
  )
{
  SetBkgColor( background );

  for( const auto& histname : histnamelist ){

    // Declaring Histograms, for construction look below.
    THStack* stack = MakeBkgStack( background, histname );
    TH1D* bkgerror = MakeBkgError( background, histname );
    TH1D* datahist = (TH1D*)datamgr->Hist( histname )->Clone();
    TH1D* bkgrel   = MakeBkgRelHist( bkgerror );
    TH1D* datarel  = MakeDataRelHist( datahist, bkgerror );
    TH1D* sighist  = (TH1D*)signalmgr->Hist( histname )->Clone();

    cout << bkgerror->Integral() << " " << datahist->Integral() << endl;

    // Scaling signal plot for clarity
    const unsigned datanum = datamgr->ExpectedYield();
    const double signum    = signalmgr->ExpectedYield();
    const double sigscale  = datanum / signum / 2.;
    if( sighist->Integral() < datahist->Integral() /4.0 ){
      sighist->Scale( sigscale );
    }

    // Legend settings
    TLegend* l = mgr::NewLegend( 0.6, 0.5 );
    l->AddEntry( datahist, datamgr->RootName().c_str(), "pe" );

    for( const auto& entry : background ){
      l->AddEntry( entry->Hist( histname ), entry->RootName().c_str(), "f" );
    }

    l->AddEntry( bkgerror, "Unc. (stat #oplus sys)",                                                                "fl" );
    l->AddEntry( sighist,  boost::str( boost::format( "%s(#times%.0lf)" )%signalmgr->RootName()%sigscale ).c_str(), "fl" );

    MakePlot(
      stack,
      bkgerror,
      datahist,
      sighist,
      bkgrel,
      datarel,
      l,
      "fullcomp",
      {histname, label}
      );
    delete l;
    delete stack;
    delete bkgerror;
    delete datahist;
    delete bkgrel;
    delete datarel;
    delete sighist;
  }

}

/******************************************************************************/

extern void
Normalize(
  SampleErrHistMgr*               data,
  std::vector<SampleErrHistMgr*>& bg
  )
{
  const double datayield = data->ExpectedYield();
  double bgyield         = 0;

  for( const auto& sample : bg ){
    bgyield += sample->ExpectedYield();
  }

  const double scale = datayield / bgyield;

  for( auto& sample : bg ){
    sample->Scale( scale );
  }
}

/******************************************************************************/

TH1D*
MakeSumHistogram(
  const vector<SampleErrHistMgr*>& samplelist,
  const string&                    histname
  )
{
  const TH1D* temphist = samplelist.front()->Hist( histname );
  const unsigned bins  = temphist->GetXaxis()->GetNbins();
  const double xmin    = temphist->GetXaxis()->GetXmin();
  const double xmax    = temphist->GetXaxis()->GetXmax();

  const string newname = samplelist.front()->Name() + histname + "sum";
  const string title   = ";" + string( temphist->GetXaxis()->GetTitle() ) + ";" + string( temphist->GetYaxis()->GetTitle() );
  TH1D* sumhist        = new TH1D( newname.c_str(), title.c_str(), bins, xmin, xmax );

  for( const auto& sample : samplelist ){
    sumhist->Add( sample->Hist( histname ) );
  }

  sumhist->SetStats( 0 );
  return sumhist;
}


/******************************************************************************/

Parameter
ExpectedYield( const vector<SampleErrHistMgr*>& samplelist )
{
  Parameter ans;

  for( const auto& group : samplelist ){
    for( const auto& sample : group->SampleList() ){
      ans += sample.CrossSection()
             * sample.SelectionEfficiency()
             * sample.PDFUncertainty()
             * sample.QCDScaleUncertainty();
    }
  }

  return ans;
}

/******************************************************************************/

void
SetBkgColor( vector<SampleErrHistMgr*>& bkg )
{
  bkg[0]->SetColor( TColor::GetColor( "#FFCC88" ) );
  bkg[1]->SetColor( TColor::GetColor( "#996600" ) );
  bkg[2]->SetColor( TColor::GetColor( "#FF3333" ) );
  bkg[3]->SetColor( TColor::GetColor( "#33EEEE" ) );
  // bkg[4]->SetColor( TColor::GetColor( "#0066EE" ) );
}

/******************************************************************************/

THStack*
MakeBkgStack(
  const vector<SampleErrHistMgr*>& samplelist,
  const string&                    histname
  )
{
  THStack* stack = new THStack( ( histname+"bkgstack" ).c_str(), "" );

  for( const auto& sample : boost::adaptors::reverse( samplelist ) ){
    stack->Add( sample->Hist( histname ), "HIST" );
  }

  return stack;
}

/******************************************************************************/

TH1D*
MakeBkgError(
  const vector<SampleErrHistMgr*>& samplelist,
  const string&                    histname
  )
{
  TH1D* central = MakeSumHistogram( samplelist, histname );
  vector<pair<TH1D*, TH1D*> > errorhistlist;

  for( const auto& err : histerrlist ){

    // rescaling object for PDF and QCD scale errors
    if( err.tag == "pdf" || err.tag == "scale " ){
      for( const auto sample : samplelist ){
        TH1D* central  = sample->Hist( histname );
        TH1D* uphist   = sample->Hist( histname + err.tag + "up" );
        TH1D* downhist = sample->Hist( histname + err.tag + "down" );
        uphist->Scale( central->Integral()/uphist->Integral() );
        downhist->Scale( central->Integral()/downhist->Integral() );
      }
    }

    // Creating the summed results.
    errorhistlist.emplace_back(
      MakeSumHistogram( samplelist, histname + err.tag + "up" ),
      MakeSumHistogram( samplelist, histname + err.tag + "down" )
      );
  }

  const Parameter expyield = ExpectedYield( samplelist );
  const Parameter normerr  = Parameter(
    1,
    expyield.RelUpperError(),
    expyield.RelLowerError()
    );

  for( int i = 1; i <= central->GetSize(); ++i ){
    const double bincont = central->GetBinContent( i );
    const double binerr  = central->GetBinError( i );

    Parameter errtot( 1, binerr/bincont, binerr/bincont );

    for( const auto pair : errorhistlist ){
      const double upbincont   = pair.first->GetBinContent( i );
      const double downbincont = pair.second->GetBinContent( i );
      const double upabserr    = std::max( 0.0, upbincont-bincont );
      const double downabserr  = std::max( 0.0, bincont-downbincont );
      errtot *= Parameter( 1, upabserr/bincont, downabserr/bincont );
    }

    // Additional errors
    errtot *= ( normerr );// cross section and selection eff
    errtot *= compnamer.MasterConfig().GetStaticParameter( "Lumi Error" );// lumierror
    errtot *= compnamer.GetChannel().find( "Electron" ) == string::npos ?  // leptonic systematic error
              compnamer.MasterConfig().GetStaticParameter( "Muon Systematic" ) :
              compnamer.MasterConfig().GetStaticParameter( "Electron Systematic" );


    if( bincont == 0 ){
      central->SetBinError( i, 0 );
    } else if( errtot.RelAvgError() >= 0.6 ){
      central->SetBinError( i, bincont * 0.6 );
    } else {
      central->SetBinError( i, bincont * errtot.RelAvgError() );
    }

  }


  for( auto& pair : errorhistlist ){
    delete pair.first;
    delete pair.second;
  }

  return central;
}

/******************************************************************************/

void
PlotErrCompare(
  const vector<SampleErrHistMgr*>& samplelist,
  const string&                    histname,
  const ErrorSource&               err
  )
{
  TH1D* central = MakeSumHistogram( samplelist, histname );
  TH1D* errup   = MakeSumHistogram( samplelist, histname + err.tag + "up" );
  TH1D* errdown = MakeSumHistogram( samplelist, histname + err.tag + "down" );

  // Normailizing plots if err is the pdf error or scale errors
  if( err.tag == "scale" || err.tag == "pdf" ){
    errup->Scale( central->Integral() / errup->Integral() );
    errdown->Scale( central->Integral() / errdown->Integral() );
  }
  // Making duplicate objects
  TH1* uprel   = mgr::DivideHist( errup, central );
  TH1* downrel = mgr::DivideHist( errdown, central );

  const double xmin = central->GetXaxis()->GetXmin();
  const double xmax = central->GetXaxis()->GetXmax();
  // Setting plot range
  const double ymax     = mgr::GetYmax( central, errup, errdown );
  const double relymax  = std::min( 2.0, std::max( uprel->GetMaximum(), downrel->GetMaximum() ) );
  const double bpaddist = std::ceil( ( relymax-1 )*10 )/10.;

  // Plotting
  TCanvas* c = mgr::NewCanvas();

  // TOPPAD
  TPad* pad1 = mgr::NewTopPad();
  pad1->Draw();
  pad1->cd();
  central->Draw( PS_AXIS );
  errup->Draw( PS_HIST PS_SAME );
  errdown->Draw( PS_HIST PS_SAME );
  central->Draw( PS_HIST PS_SAME );
  c->cd();

  // Bottom pad
  TPad* pad2      = mgr::NewBottomPad();
  TLine* line     = new TLine( xmin, 1, xmax, 1 );
  TLine* line_top = new TLine( xmin, 1+bpaddist, xmax, 1+bpaddist );
  TLine* line_bot = new TLine( xmin, 1-bpaddist, xmax, 1-bpaddist );
  pad2->Draw();
  pad2->cd();

  uprel->Draw( PS_AXIS );
  uprel->Draw( PS_HIST PS_SAME );
  downrel->Draw( PS_HIST PS_SAME );
  line->Draw( PS_SAME );
  line_top->Draw( PS_SAME );
  line_bot->Draw( PS_SAME );
  c->cd();


  // Drawing legend
  const double legxmin = 0.5;
  const double legymin = 0.7;
  TLegend* tl          = mgr::NewLegend( legxmin, legymin );
  const string label   = ( err.label != "" ) ? err.label : "1#sigma";
  tl->AddEntry( central, "Central Value",                               "l" );
  tl->AddEntry( errup,   ( err.rootname + "(+" + label + ")" ).c_str(), "l" );
  tl->AddEntry( errdown, ( err.rootname + "(-" + label + ")" ).c_str(), "l" );

  tl->Draw( PS_SAME );

  // Common styling
  central->SetLineColor( kBlack );
  errup->SetLineColor( KRED );
  errdown->SetLineColor( KBLUE );
  uprel->SetLineColor( KRED );
  downrel->SetLineColor( KBLUE );

  // accessory styling
  line->SetLineColor( kBlack );
  line->SetLineStyle( 1 );
  line->SetLineWidth( 2 );
  line_top->SetLineColor( kBlack );
  line_bot->SetLineColor( kBlack );
  line_top->SetLineStyle( 3 );
  line_bot->SetLineStyle( 3 );

  mgr::SetTopPlotAxis( central );
  mgr::SetBottomPlotAxis( uprel );

  // Setting plot range
  central->SetMaximum( ymax * 1.5 );
  uprel->SetMaximum( 1+( bpaddist+0.05 ) );
  uprel->SetMinimum( 1-( bpaddist+0.05 ) );
  uprel->GetYaxis()->SetTitle( "#frac{up,down}{Norm}" );

  mgr::LatexMgr latex;
  latex.SetOrigin( PLOT_X_MIN, PLOT_Y_MAX + ( TEXT_MARGIN/2 ), BOTTOM_LEFT )
  .WriteLine( compnamer.GetChannelEXT( "Root Name" ) );

  latex.SetOrigin( PLOT_X_TEXT_MAX, legymin - 0.02, TOP_RIGHT );
  if( compnamer.GetInput<string>( "group" ).find( "Tstar" ) != string::npos ){
    latex.WriteLine( boost::str( boost::format( "M_{t*}=%dGeV/c^{2}" )%GetInt( compnamer.GetInput<string>( "group" ) ) ) );
  } else {
    latex.WriteLine( compnamer.GetInput<string>( "group" ) );
  }

  // cleaning up
  mgr::DrawCMSLabel( SIMULATION );
  mgr::DrawLuminosity( mgr::SampleMgr::TotalLuminosity() );

  mgr::SaveToPDF( c, compnamer.PlotFileName( "errcomp", histname, err.tag ) );
  mgr::SaveToROOT(
    c,
    compnamer.PlotRootFile(),
    compnamer.OptFileName( "", "errcomp", histname, err.tag )
    );

  delete tl;
  delete uprel;
  delete downrel;
  delete central;
  delete errup;
  delete errdown;
  delete c;
}
