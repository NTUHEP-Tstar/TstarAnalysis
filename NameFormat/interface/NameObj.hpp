/*******************************************************************************
 *
 *  Filename    : NameObj.hpp
 *  Description : Forward declaration of naming objects
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#ifndef TSTARANALYSIS_NAMEFORMAT_NAMEOBJ_HPP
#define TSTARANALYSIS_NAMEFORMAT_NAMEOBJ_HPP

#include "TstarAnalysis/NameFormat/interface/TagTree.hpp"
#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>

// Naming objects for each running package
extern TagTree compare_namer;
extern TagTree massreco_namer;
extern TagTree limit_namer;

// Common settings
extern boost::program_options::options_description option_setting;
extern boost::program_options::variables_map       option_input;

extern int InitOptions( int , char* const [] );   // Initializing options and parsing
extern void InitSampleSettings( const TagTree& ); // Initializing sample settings from tag tree
extern int  CheckManditory( const std::vector<std::string>& );
extern void ShowManditory( const std::vector<std::string>& );
//------------------------------------------------------------------------------
//   EXIT_FLAGS
//------------------------------------------------------------------------------
#define PARSE_SUCESS 0
#define PARSE_HELP   1
#define PARSE_ERROR  2


#endif /* end of include guard: TSTARANALYSIS_NAMEFORMAT_NAMEOBJ_HPP */
