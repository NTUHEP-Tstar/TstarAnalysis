/*******************************************************************************
 *
 *  Filename    : SampleRooFitMgr.hh
 *  Description : RooFit object per SampleGroup management process
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
 *  - For basic access functions and inititializing, see SampleRooFitMgr.cc
 *  - For Pdf specialization, see SampleRooFitMgr_MakePdf.cc
 *  - For DataSet filling functions, see SampleRooFitMgr_FillSet.cc
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
#include "RooKeysPdf.h"

class SampleRooFitMgr : public mgr::SampleGroup
{
public:
   SampleRooFitMgr( const std::string&, const mgr::ConfigReader& );
   ~SampleRooFitMgr();

   // Static member functions
   static RooRealVar& x()  { return *_x; }
   static RooRealVar& w()  { return *_w; }
   static double MinMass() { return _min_mass; }
   static double MaxMass() { return _max_mass; }
   static void  InitStaticVars(const double, const double) ;
   static void  ClearStaticVars();

   // RooRealVar Access member functions
   RooRealVar*  NewVar( const std::string&, const double, const double);
   RooRealVar*  NewVar( const std::string&, const double, const double, const double );
   RooRealVar*          Var( const std::string& );
   const RooRealVar*    Var( const std::string& ) const;
   std::vector<std::string>   AvailableVarNameList()   const;
   std::vector<RooRealVar*>&        VarList()       { return _varlist; }
   const std::vector<RooRealVar*>&  VarList() const { return _varlist; }
   std::vector<RooRealVar*>   VarContains( const std::string& ) const ;
   void SetConstant(bool set=kTRUE);


   // DataSet access list
   RooDataSet* NewDataSet( const std::string& );
   void        AddDataSet( RooDataSet* );
   RooDataSet*       DataSet( const std::string& name="" ); // Should contain a default dataset
   const RooDataSet* DataSet( const std::string& name="" ) const ;
   std::vector<RooDataSet*>&        SetList()       { return _setlist; }
   const std::vector<RooDataSet*>&  SetList() const { return _setlist; }
   std::vector<std::string>  AvailableSetNameList() const;
   void RemoveDataSet( const std::string& );

   // PDF making functions
   RooAbsPdf*  NewPdf     ( const std::string& name, const std::string& type ); // Specialized functions! see SampleRooFitMgr_MakePdf.cc
   RooKeysPdf* MakeKeysPdf( const std::string& name, const std::string& dataset="" ); // Specialization for RooKeysPdf
   void        AddPdf( RooAbsPdf* );
   RooAbsPdf*       Pdf( const std::string& );
   const RooAbsPdf* Pdf( const std::string& ) const ;
   std::vector<RooAbsPdf*>&       PdfList()       { return _pdflist; }
   const std::vector<RooAbsPdf*>& PdfList() const { return _pdflist; }
   std::vector<std::string> AvailablePdfNameList() const;


private:
   static double _min_mass;
   static double _max_mass;
   static RooRealVar* _x;
   static RooRealVar* _w;
   std::vector<RooRealVar*>  _varlist;
   std::vector<RooDataSet*>  _setlist;
   std::vector<RooAbsPdf*>   _pdflist;

   // Filling functions specific to this analysis, see SampleRooFitMgr_FillSet.cc
   void definesets();
   void fillsets( mgr::SampleMgr& );
};

#endif /* end of include guard: __SAMPLE_ROOFIT_MGR_HH__ */
