/*******************************************************************************
 *
 *  Filename    : TstarNamer.hpp
 *  Description : Specilized classed based off PackagePathMgr and OptsNamer
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#ifndef TSTARANALYSIS_NAMEFORMAT_TAGTREE_HPP
#define TSTARANALYSIS_NAMEFORMAT_TAGTREE_HPP

#include "ManagerUtils/SysUtils/interface/PackagePathMgr.hpp"
#include "ManagerUtils/SysUtils/interface/OptsNamer.hpp"
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <vector>

class TstarNamer : public mgr::PackagePathMgr, public mgr::OptsNamer
{
public:
   TstarNamer( const std::string& sub_package );
   virtual ~TstarNamer();

   std::string MasterConfigFile()  const ;

   // channel Specializations: Settings channels separately for channel mixing
   void SetChannel( const std::string& );
   const std::string& GetChannel() const { return _channel_tag; }
   std::string GetChannelEDMPath()const ;
   std::string GetChannelEXT( const std::string& ) const ;

   // File making functions
   void SetNamingOptions( const std::vector<std::string>& x) { _naming_option_list =x ; }
   int  LoadOptions( const boost::program_options::options_description& desc, int argc, char* argv[] );
   std::string MakeFileName(
      const std::string& extension,
      const std::string& main_tag,
      const std::string& sub_tag = "" ) const ;
   std::string TextFileName( const std::string& main_tag, const std::string& sub_tag = "" ) const ;
   std::string PlotFileName( const std::string& main_tag, const std::string& sub_tag = "" ) const ;
   std::string TexFileName ( const std::string& main_tag, const std::string& sub_tag = "" ) const ;
   std::string RootFileName( const std::string& main_tag, const std::string& sub_tag = "" ) const ;


private:
   std::string                _channel_tag;
   std::vector<std::string>   _naming_option_list ;
};



#endif /* end of include guard: TSTARANALYSIS_NAMEFORMAT_TAGTREE_HPP */
