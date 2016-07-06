/*******************************************************************************
 *
 *  Filename    : TagTree.hpp
 *  Description : String tree powered by boost property tree
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
 *  Analysis specific, will not promote yet
 *
*******************************************************************************/
#ifndef TSTARANALYSIS_NAMEFORMAT_TAGTREE_HPP
#define TSTARANALYSIS_NAMEFORMAT_TAGTREE_HPP

#include "ManagerUtils/SysUtils/interface/FilenameMgr.hpp"
#include "ManagerUtils/BaseClass/interface/ConfigReader.hpp"
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <map>

class TagTree : public mgr::FilenameMgr {
public:
   TagTree( const std::string& config_file, const std::string& sub_package );
   virtual ~TagTree();

   // Static Helper function
   static int GetInt( const std::string& ) ;

   inline void SetChannel      ( const std::string& x ) { _channel_tag        = x; }
   inline void SetFitMethod    ( const std::string& x ) { _fit_method_tag     = x; }
   inline void SetFitFunc      ( const std::string& x ) { _fit_func_tag       = x; }
   inline void SetCombineMethod( const std::string& x ) { _combine_method_tag = x; }

   // Setting and file paths
   const mgr::ConfigReader& MasterConfig()    const ;
   const std::string        SettingFilePath() const ;
   const std::string        EDMFilePath()     const ;

   // Getting full names from config_file
   const std::string& GetChannel() const { return _channel_tag; }
   std::string GetChannelDataTag() const ;
   std::string GetChannelLatex()   const ;
   std::string GetChannelRoot()    const ;

   const std::string& GetFitMethod() const { return _fit_method_tag; }
   std::string GetFitMethodFull()    const ;

   const std::string& GetFitFunc()  const { return _fit_func_tag; }
   std::string GetFitFuncFull()     const ;
   std::string GetFitFuncLatex()    const ;
   std::string GetFitFuncRoot()     const ;

   const std::string& GetCombineMethod() const { return _combine_method_tag; }
   std::string GetCombineMethodFull()    const ;

   // File Names
   std::string MakeFileName(
      const std::string& extension,
      const std::string& main_tag,
      const std::string& sub_tag = "" ) const ;
   std::string TextFileName( const std::string& main_tag, const std::string& sub_tag = "" ) const ;
   std::string PlotFileName( const std::string& main_tag, const std::string& sub_tag = "" ) const ;
   std::string TexFileName ( const std::string& main_tag, const std::string& sub_tag = "" ) const ;
   std::string RootFileName( const std::string& main_tag, const std::string& sub_tag = "" ) const ;

   void SetTag( const std::string& x, const std::string& y) { _tag_map[x] = y; }
   const std::string& GetTag( const std::string& x )  { return _tag_map[x] ; }

private:
   boost::property_tree::ptree _tree;
   std::string  _channel_tag;
   std::string  _fit_method_tag;
   std::string  _fit_func_tag;
   std::string  _combine_method_tag;

   mgr::ConfigReader _master_config;

   std::map<std::string,std::string> _tag_map;

   std::string img_extension() const ;
};



#endif /* end of include guard: TSTARANALYSIS_NAMEFORMAT_TAGTREE_HPP */
