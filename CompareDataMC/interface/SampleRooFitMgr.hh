/*******************************************************************************
 *
 *  Filename    : SampleRooFitMgr.hh
 *  Description : RooFit object manager class
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#ifndef __SAMPLE_ROOFIT_MGR_HH__
#define __SAMPLE_ROOFIT_MGR_HH__

#include "ManagerUtils/SampleMgr/interface/SampleGroup.hpp"
#include "ManagerUtils/BaseClass/interface/ConfigReader.hpp"
#include "ManagerUtils/Maths/interface/Parameter.hpp"
#include <string>
#include <vector>
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooAbsPdf.h"

class SampleRooFitMgr : public mgr::SampleGroup
{
public:
   SampleRooFitMgr( const std::string&, const mgr::ConfigReader& ); // Single Sample ( for signal )
   ~SampleRooFitMgr();

   // Static member functions
   static double MinMass();
   static double MinFitMass();
   static double MaxMass();
   static RooRealVar& x() { return _x; }
   static RooRealVar& w() { return _w; }

   // Additional access functions
   RooDataSet* OriginalDataSet() { return _dataset; }

   // Making/Acessing extended dataset
   RooDataSet* MakeReduceDataSet(
      const std::string&,
      const RooCmdArg&  ,
      const RooCmdArg& = RooCmdArg() );
   RooDataSet* GetReduceDataSet( const std::string& );

   // Generic Data Adding ( for generated datasets )
   std::string MakeDataAlias( const std::string& x ) const ;
   RooDataSet* GetDataFromAlias( const std::string& x ) ;
   void AddDataSet( RooDataSet* );

   // Adding/Accessing pdfs
   std::string MakePdfAlias( const std::string& x ) const;
   void AddPdf( RooAbsPdf* );
   RooAbsPdf* GetPdfFromAlias( const std::string& x );
   RooAbsPdf* GetPdfFromName( const std::string& x );

private:
   static RooRealVar _x;
   static RooRealVar _w;
   RooDataSet* _dataset;
   std::vector<RooDataSet*> _ext_dataset;
   std::vector<RooAbsPdf*>  _pdf_list;

   void FillDataSet( mgr::SampleMgr& );
};

#endif /* end of include guard: __SAMPLE_ROOFIT_MGR_HH__ */
