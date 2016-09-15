/*******************************************************************************
*
*  Filename    : SimFitVal.hpp
*  Description : Defining functions to be used for simultaneous fit validations
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*  For detailed implementations see RooFit_SimFitVal.cc and PlotSimFitVal.cc
*
*******************************************************************************/
#ifndef TSTARANALYSIS_LIMITCALC_SIMFITVAL_HPP
#define TSTARANALYSIS_LIMITCALC_SIMFITVAL_HPP

/*******************************************************************************
*   Main control functions to be called by main functions
*
*   ** RunGenFit - For given data, bkg, and signal.
                   Generated pseudo data and run SimFit, writes the outputs
                   to designated file.
*
*   ** PlotGenFit - For Input mass list, read the outputs from the above functions
                    and plot results.
*******************************************************************************/
extern void RunGenFit(
   SampleRooFitMgr* data,
   SampleRooFitMgr* bkg,
   SampleRooFitMgr* sig
   );

extern void PlotGenFit( const std::vector<std::string>& );

/*******************************************************************************
*   Common Helper functions - getting filename tag for input injected signal strength
*******************************************************************************/
std::string SigStrengthTag();

/*******************************************************************************
*   Helper functions for Plotting results
*
*   ** PullResults - simple transparent struct for storing the pull results
*
*   ** PlotSingleGenFit - Making the relevent plots for a single mass point
*                          includes fitting of pull ditribution.
*   ** MakePullPlot  - Fitting and plotting the pull distribution, saving fitting results as
*                      Parameter.
*
*   ** MakePullComparePlot - Making the plot of the pull distribution across multiples
*                            Mass points
*
*******************************************************************************/
struct PullResult;
PullResult PlotSingleGenFit( const std::string& masstag );
Parameter  MakePullPlot( RooDataSet&, const std::string& masstag, const std::string& tag );
void       MakePullComparePlot( TGraph*, const std::string& tag );


#endif/* end of include guard: TSTARANALYSIS_LIMITCALC_SIMFITVAL_HPP */
