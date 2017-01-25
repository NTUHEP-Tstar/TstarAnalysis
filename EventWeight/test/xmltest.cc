/*******************************************************************************
*
*  Filename    : xmltest.cc
*  Description : xml parser testing
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "ManagerUtils/Common/interface/ConfigReader.hpp"
#include <iostream>

using namespace std;
namespace bpt = boost::property_tree;

int
main( int argc, char const* argv[] )
{
  bpt::ptree mytree;

  read_xml( "test.xml", mytree );

  mgr::PrintPTree( mytree );

  cout << "Done printing tree" <<endl;

  for( const auto& it : mytree ){
    cout << it.first << " >> " << it.second.get<string>( "<xmlattr>.name" ) << endl;

    for( const auto& secit : it.second ){
      if( secit.first == "weight" ){
        cout << "\t" << secit.second.get<string>( "<xmlattr>.id" )
             << ">>"  << secit.second.get_value<std::string>();
      }
    }
  }

  return 0;
}
