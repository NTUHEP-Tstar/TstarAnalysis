/*******************************************************************************
 *
 *  Filename    : NameParse.cc
 *  Description : Implementations for NameParse functions
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include <string>

using namespace std;

int GetInt( const string& x )
{
   string ans_string;
   for( auto y : x ){
      if( y >= '0' && y <= '9'){
         ans_string.push_back(y);
      }
   }
   return stoi(ans_string);
}
