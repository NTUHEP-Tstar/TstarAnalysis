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
    ( "inputfiles,i",  opt::value<vector<string> >()->required(), "Which files to uses" )
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


      for( const auto& weightinfo : evthandle.ref().weights() ){
        const unsigned thisid   = stoi( weightinfo.id );
        const double thisweight = weightinfo.wgt / centralweight;
        if( find( pdfidlist.begin(), pdfidlist.end(), thisid ) != pdfidlist.end() ){
          pdfweightsum[thisid] += thisweight;
        }
        if( find( qcdidlist.begin(), qcdidlist.end(), thisid ) != qcdidlist.end() ){
          qcdweightsum[thisid] += thisweight;
        }
      }
    }
  }

  double sumsq = 0 ;
  double sum = 0;
  for( const auto weightsum : pdfweightsum ){
    cout << weightsum.first << ": " << weightsum.second << endl;
    sumsq += weightsum.second * weightsum.second ;
    sum += weightsum.second;
  }
  double size = pdfweightsum.size();
  cout << "RMS: " << sqrt ( (sumsq - ((sum*sum)/(size)) )/(size-1) ) << endl;

  //
  for( const auto weightsum : qcdweightsum ){
    cout << weightsum.first << ": " << weightsum.second << endl;
  }

  return 0;
}
