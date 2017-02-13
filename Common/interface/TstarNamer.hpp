/*******************************************************************************
*
*  Filename    : TstarNamer.hpp
*  Description : Specilized classed based off PackagePathMgr and OptsNamer
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*  The SampleMgr and SampleGroup settings files is expected to be
*  in the directory: Package/SubPackage/settings/master.json
*
*  The option extension naming file is expected to be in the directory
*  Package/SubPackage/settings/name_settings.json
*
*  The generated files names will be in the directory
*  Package/SubPackage/results/<channel>/
*
*******************************************************************************/
#ifndef TSTARANALYSIS_COMMON_TSTARNAMER_HPP
#define TSTARANALYSIS_COMMON_TSTARNAMER_HPP

#include "ManagerUtils/Common/interface/ConfigReader.hpp"
#include "ManagerUtils/Common/interface/OptNamer.hpp"
#include "ManagerUtils/SysUtils/interface/PackagePathMgr.hpp"
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <vector>

class TstarNamer :
  public mgr::PackagePathMgr,
  public mgr::OptNamer
{
public:
  TstarNamer( const std::string& sub_package );
  virtual
  ~TstarNamer();

  const mgr::ConfigReader&
  MasterConfig()  const { return _masterconfig; }

  // Additional option parsing
  void SetNamingOptions( const std::vector<std::string>& x ) { _namingoptionlist = x; }
  void AddCutOptions( const std::string& x ) { _cutoptionlist.push_back(x); }
  int  ParseOptions( int argc, char** argv );

  // Options specializations: "channel"
  void               SetChannel( const std::string& );
  const std::string& GetChannel() const;
  std::string        GetChannelEDMPath() const;
  std::string        GetChannelEXT( const std::string& ) const;


  // turns input ( ext, {tag1,tag2,tag3...} ) into filename: tag1_tag2_tag3_..._tagn.ext
  std::string CustomFileName(
    const std::string&              extension,
    const std::vector<std::string>& taglist
    ) const;

  // For set Naming options (optinput1,optinput2) , we turn the input
  // (ext, maintag, {subtag1,subtag2} ) in to filename:
  // maintag_optinput1_optinput2_...optinputn_subtag1_subtag2....ext
  std::string OptFileName(
    const std::string&              extension,
    const std::string&              main_tag,
    const std::vector<std::string>& subtaglist
    ) const;

  // Specicalization for common prefix files
  std::string TextFileName(
    const std::string&              maintag,
    const std::vector<std::string>& subtaglist = {} ) const;
  std::string PlotFileName(
    const std::string&              maintag,
    const std::vector<std::string>& subtaglist = {} ) const;
  std::string TexFileName(
    const std::string&              maintag,
    const std::vector<std::string>& subtaglist = {} ) const;
  std::string RootFileName(
    const std::string&              maintag,
    const std::vector<std::string>& subtaglist = {} ) const;

  // PlotRootFile - Specialized file name for saving plot objects
  std::string
  PlotRootFile() const { return RootFileName( "PLOTROOT" ); }

  // Variadic interfacing functions
  template<typename ... TS>
  void SetNamingOptions( const std::string&, TS ... args );

  template<typename ... TS>
  std::string CustomFileName( const std::string&, const std::string&, TS ... args ) const ;

  template<typename ... TS>
  std::string OptFileName( const std::string&, const std::string&, const std::string&, TS ... args ) const ;

  template<typename ... TS>
  std::string TextFileName( const std::string&, const std::string&, TS ... args ) const ;

  template<typename ... TS>
  std::string PlotFileName( const std::string&, const std::string&, TS ... args ) const;

  template<typename ... TS>
  std::string TexFileName( const std::string&, const std::string&, TS ... args ) const ;

  template<typename ... TS>
  std::string RootFileName( const std::string&, const std::string&, TS ... args ) const ;

private:
  std::string _channel;
  std::vector<std::string> _namingoptionlist;
  std::vector<std::string> _cutoptionlist;
  mgr::ConfigReader _masterconfig;
};

#include "TstarAnalysis/Common/src/TstarNamer.ipp"


#endif/* end of include guard: TSTARANALYSIS_NAMEFORMAT_TAGTREE_HPP */
