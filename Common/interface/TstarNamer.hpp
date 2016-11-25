/*******************************************************************************
*
*  Filename    : TstarNamer.hpp
*  Description : Specilized classed based off PackagePathMgr and OptsNamer
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_NAMEFORMAT_TAGTREE_HPP
#define TSTARANALYSIS_NAMEFORMAT_TAGTREE_HPP

#include "ManagerUtils/BaseClass/interface/ConfigReader.hpp"
#include "ManagerUtils/SysUtils/interface/OptsNamer.hpp"
#include "ManagerUtils/SysUtils/interface/PackagePathMgr.hpp"
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <vector>

class TstarNamer :
   public mgr::PackagePathMgr,
   public mgr::OptsNamer
{
public:
   TstarNamer( const std::string& sub_package );
   virtual
   ~TstarNamer();

   const mgr::ConfigReader& MasterConfig()  const { return _master_config; }

   // channel Specializations: Settings channels separately for channel mixing
   void SetChannel( const std::string& );
   const std::string& GetChannel() const { return _channel_tag; }
   std::string GetChannelEDMPath() const;
   std::string GetChannelEXT( const std::string& ) const;

   // Options parsing and access
   int LoadOptions(
      const boost::program_options::options_description& desc,
      int argc, char* argv[]
      );


   void
   SetNamingOptions( const std::vector<std::string>& x )
   {
      _naming_option_list = x;
   }
   // PlotRootFile - Specialized file name for saving plot objects
   std::string PlotRootFile() const { return RootFileName("PLOTROOT"); }

   // turns input ( ext, {tag1,tag2,tag3...} ) into filename: tag1_tag2_tag3_..._tagn.ext
   std::string CustomFileName(
      const std::string&              extension,
      const std::vector<std::string>& taglist
      ) const;

   // For set Naming options (optinput1,optinput2) , we turn the input
   // (ext, maintag, {subtag1,subtag2} ) in to filename:
   // maintag_optinput1_optinput2_...optinputn_subtag1_subtag2....ext
   std::string MakeFileName(
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

private:
   std::string _channel_tag;
   std::vector<std::string> _naming_option_list;
   mgr::ConfigReader  _master_config;
};



#endif/* end of include guard: TSTARANALYSIS_NAMEFORMAT_TAGTREE_HPP */
