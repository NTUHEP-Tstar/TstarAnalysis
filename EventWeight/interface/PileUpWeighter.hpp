/*******************************************************************************
 *
 *  Filename    : PileUpWeighter.hpp
 *  Description : Reading contents of a file to return a pile up weight
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
 *  Input file should be in the format of a list of doubles, with the first
 *  Double corresponding to the pileup weight at 0, and so on. Ranges undefined
 *  will be regarded with a weight 0.
 *
*******************************************************************************/
#ifndef TSTARANALYSIS_EVENTWEIGHT_PILEUPWEIGHTER_HPP
#define TSTARANALYSIS_EVENTWEIGHT_PILEUPWEIGHTER_HPP

#include <vector>
#include <string>

class PileUpWeighter {
public:
   PileUpWeighter( const std::string& filename );
   virtual ~PileUpWeighter ();

   double GetWeight( const unsigned ) const ;

private:
   std::vector<double> _pileupweights;
};

#endif /* end of include guard: TSTARANALYSIS_EVENTWEIGHT_PILEUPWEIGHTER_HPP */
