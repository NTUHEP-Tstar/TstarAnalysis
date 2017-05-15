/*******************************************************************************
*
*  Filename    : ComputeGenWeightUnc.cc
*  Description : Main function for looping through EDM files to compute GenWeight
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"

#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "TstarAnalysis/Common/interface/GetEventWeight.hpp"

#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <map>

#include "TFile.h"


using namespace std;
namespace opt = boost::program_options;

int
main( int argc, char const* argv[] )
{
  opt::variables_map args;
  opt::options_description desc( "Options for ComputeGenWeightUnc" );
  desc.add_options()
    ( "inputfiles,i",  opt::value<vector<string> >()->required(), "Which EDM files to uses as input" )
    ( "title,t",         opt::value<string>()->required(), "title of sample" )
    ( "outputfile,o",    opt::value<string>()->required(), "prefix to plot files" )
  ;

  opt::store( opt::parse_command_line( argc, argv, desc ), args );
  opt::notify( args );

  const auto filelist = args["inputfiles"].as<vector<string> >();
  const auto idgroup  = GetPdfIdGrouping( filelist.front() );

  std::map<unsigned, double> pdfweightsum;
  std::map<unsigned, double> qcdweightsum;
  unsigned counter = 0;

  for( const auto& file : filelist ){
    fwlite::Event ev( TFile::Open( file.c_str() ) );

    for( ev.toBegin(); !ev.atEnd(); ++ev, ++counter ){
      cout << "\rEvent[" << counter<<  "]..." << flush;

      fwlite::Handle<LHEEventProduct> evthandle;
      evthandle.getByLabel( ev, "externalLHEProducer" );

      const auto pdfidlist = ( idgroup.front().front() == 1 ) ?
                             GetMadGraphIdList( ev, idgroup ) :
                             GetPowHegIdList( ev, idgroup );
      const auto qcdidlist = ( idgroup.front().front() == 1 ) ?
                             GetMadGraphScaleIdList( ev ) :
                             GetPowHegScaleIdList( ev );
      const double centralweight = ( idgroup.front().front() == 1 ) ?
                                   evthandle.ref().weights().front().wgt :
                                   evthandle.ref().originalXWGTUP();

      const double otherweights = GetEventWeight( ev );

      for( const auto& weightinfo : evthandle.ref().weights() ){
        const unsigned thisid   = stoi( weightinfo.id );
        const double thisweight = weightinfo.wgt / centralweight;
        if( find( pdfidlist.begin(), pdfidlist.end(), thisid ) != pdfidlist.end() ){
          pdfweightsum[thisid] += thisweight * otherweights;
        }
        if( find( qcdidlist.begin(), qcdidlist.end(), thisid ) != qcdidlist.end() ){
          qcdweightsum[thisid] += thisweight * otherweights;
        }
      }
    }
  }

  {
    double sumsq = 0;
    double sum   = 0;

    for( const auto weightsum : pdfweightsum ){
      cout << weightsum.first << ": " << weightsum.second << endl;
      sumsq += weightsum.second * weightsum.second;
      sum   += weightsum.second;
    }

    boost::format pdffmt( "PDF>> first:%lf, mean:%lf, rms:%lf, relerr:%lf" );
    const double size  = pdfweightsum.size();
    const double first = pdfweightsum.begin()->second;
    const double mean  = sum / size;
    const double rms   = sqrt( ( sumsq - ( ( sum*sum )/( size ) ) )/( size-1 ) );
    cout << pdffmt % first % mean % rms % ( rms/mean )
         << endl;

    TH1D* pdfplot = new TH1D( "", ";PDF variation (arbitray index);Sum of weights (normalized to mean)", pdfweightsum.size(), 0, pdfweightsum.size() );
    size_t index  = 0;

    for( const auto weightsum : pdfweightsum ){
      pdfplot->SetBinContent( index+1, weightsum.second/mean );
      index++;
    }

    TCanvas* c = mgr::NewCanvas();
    pdfplot->Draw( "HIST" );
    pdfplot->SetMaximum( std::max( pdfplot->GetMaximum(), 2.0 )*1.1 );
    pdfplot->SetMinimum( std::min( pdfplot->GetMinimum(), 0.0 )*0.9 );
    pdfplot->SetStats( 0 );

    mgr::SetAxis( pdfplot );
    mgr::SetSinglePad( c );
    mgr::DrawCMSLabel( SIMULATION );

    mgr::LatexMgr latex;
    latex.SetOrigin( PLOT_X_TEXT_MIN, PLOT_Y_TEXT_MAX, TOP_LEFT )
    .WriteLine( args["title"].as<string>() )
    .WriteLine( str( boost::format( "standard devaition (normalized):%lf" )%( rms/mean ) ) );

    mgr::SaveToPDF( c, args["outputfile"].as<string>() + "_pdfunc.pdf" );
  }
  {
    double minval = 1e20;
    double maxval = 0;
    double sum    = 0;
    double sumsq  = 0;

    for( const auto weightsum : qcdweightsum ){
      cout << weightsum.first << ": " << weightsum.second << endl;
      minval = std::min( minval, weightsum.second );
      maxval = std::max( maxval, weightsum.second );
      sum   += weightsum.second;
      sumsq += weightsum.second * weightsum.second;
    }

    const double size = qcdweightsum.size();
    // const double mean = sum / qcdweightsum.size();
    const double rms  = sqrt( ( sumsq - ( ( sum*sum )/( size ) ) )/( size-1 ) );

    boost::format qcdfmt( "QCD>> first:%lf, envelope:%lf/%lf, relenv:%lf/%lf(%lf)" );
    const double first = qcdweightsum.begin()->second;
    cout << qcdfmt % first
      % maxval % minval
      % ( ( maxval/first )-1 )
      % ( 1-( minval/first ) )
      %  ( ( ( maxval/first )-1 )/2 +  ( 1-( minval/first ) )/2 )
         << endl;

    TH1D* qcdplot = new TH1D( "", ";Scale variation (arbitray index);Sum of weights (normalized to mean)", qcdweightsum.size(), 0, qcdweightsum.size() );
    size_t index  = 0;

    for( const auto weightsum : qcdweightsum ){
      qcdplot->SetBinContent( index+1, weightsum.second/first );
      const string label =
        ( weightsum.first == 1 || weightsum.first == 1001 ) ? "#mu_{R}=1 #mu_{F}=1" :
        ( weightsum.first == 2 || weightsum.first == 1002 ) ? "#mu_{R}=1 #mu_{F}=2" :
        ( weightsum.first == 3 || weightsum.first == 1003 ) ? "#mu_{R}=1 #mu_{F}=0.5" :
        ( weightsum.first == 4 || weightsum.first == 1004 ) ? "#mu_{R}=2 #mu_{F}=1" :
        ( weightsum.first == 5 || weightsum.first == 1005 ) ? "#mu_{R}=2 #mu_{F}=2" :
        ( weightsum.first == 6 || weightsum.first == 1006 ) ? "#mu_{R}=2 #mu_{F}=0.5" :
        ( weightsum.first == 7 || weightsum.first == 1007 ) ? "#mu_{R}=0.5 #mu_{F}=1" :
        ( weightsum.first == 8 || weightsum.first == 1008 ) ? "#mu_{R}=0.5 #mu_{F}=2" :
        ( weightsum.first == 9 || weightsum.first == 1009 ) ? "#mu_{R}=0.5 #mu_{F}=0.5" :
        "";
      qcdplot->GetXaxis()->SetBinLabel( index+1, label.c_str() );
      index++;
    }

    TCanvas* c = mgr::NewCanvas();
    qcdplot->Draw( "HIST" );
    qcdplot->SetMaximum( std::max( qcdplot->GetMaximum(), 2.0 )*1.1 );
    qcdplot->SetMinimum( std::min( qcdplot->GetMinimum(), 0.0 )*0.9 );
    qcdplot->SetStats( 0 );

    mgr::SetAxis( qcdplot );
    mgr::SetSinglePad( c );
    c->SetBottomMargin(0.2);
    c->SetRightMargin(0.1);
    qcdplot->GetXaxis()->SetTitleOffset(1.5);
    mgr::DrawCMSLabel( SIMULATION );

    mgr::LatexMgr latex;
    latex.SetOrigin( PLOT_X_TEXT_MIN, PLOT_Y_TEXT_MAX, TOP_LEFT )
    .WriteLine( args["title"].as<string>() )
    .WriteLine( str( boost::format( "Max (normalized):%.3lf" )%( maxval/first ) ) )
    .WriteLine( str( boost::format( "Min (normalized):%.3lf" )%( minval/first ) ) )
    .WriteLine( str( boost::format( "S.D. (normalized):%.3lf" )%( rms/first ) ) );

    mgr::SaveToPDF( c, args["outputfile"].as<string>() + "_qcdunc.pdf" );
  }
  return 0;
}
