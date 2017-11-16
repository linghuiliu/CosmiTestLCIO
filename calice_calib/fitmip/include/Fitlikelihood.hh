#ifndef FITLIKELIHOOD_HH
#define FITLIKELIHOOD_HH
#include    <iostream>
#include    <iomanip>
#include    <fstream>
#include    <sstream>
#include    <cmath> 
#include    <cassert>
#include    <map>
#include    "TROOT.h"

#include    "TSpectrum.h"
#include    "TH1.h"
#include    "TCanvas.h"
#include    "TFile.h"
#include    "RooFit.h"
#include    "RooPlot.h"
#include    "TSpline.h"
#include    "TPaveText.h"

#include    "Langaus.hh"
#include    "Bezier.hh"


/** 
 * @brief Class for finding initial parameters of fit and providing an output of fit.
 *  Fitting function is \f$ (l\otimes g)(x)=\int_{-\infty}^{\infty}\mbox{d}y\ l(x-y)\cdot g(y)\f$,
 *  where \f$l(x)\f$ is a Landau function and \f$ g(x)\f$ is a Gaus function.
 *  The class for creation and handling with function \f$(l\otimes g)\f$, created using Fast
 *  Fourier transformation(FFT) or direct numerical computation(NC), is #Langaus.
 *  There are advantages and also disadvantages for using either FFT or NC. For functions, which can be complete 
 *  to circular functions, or which have same value for left and right corner (0) are proper for FFT. There is
 *  a problem with NC when the sigma of Gaus is going to zero (cutoff for integral range). Functions, which
 *  are not "circular-like" in fit range are improper for FFT (not precise results) and is better to use NC.
 *  If it is used NC, it is inevitable to adjust lower limit of gaus sigma via #setParlimitslo. 
 *

 Detailed description follows here.
 @author Boris Bulanek, DESY
 @date June 2010
 */

class FitLikelihood{
    public: 
        //!Constructor. 
        /*!@param h - histogram for fitting.*/
        FitLikelihood(TH1* h);

        TH1* getHistogram(){return _histogram;}
        /**  Find the peak position from stored histogram using Bezier function in class Bezier.*/
        /*!@param order - Order of Bezier function
         * @param rebin - rebining of histogram (stored e.g. by calling constructor)   */
        double findPeakInitBezier(int order, int rebin);
        Bezier* getBezier(){return _bezier;}
        double getPeakInit(){return _peakInit;}


        /**  Find the proper low value of fit range of histogram h to avoid of pedestal influence.  */
        double findRangeMin(const TH1* h);

        FitLikelihood& setPeakInit(const double iniPeak);
        
        /**  Set a fixed range \f$\left< minRange,maxRange\right>\f$. */
        FitLikelihood& setRange(double minRange,double maxRange );

        inline const double* getRange() const{ return _rangeInit;}
        
        inline double getRangeMin() const{ return _rangeMin;}

        /** Set initial values of fitting parameters (Landau sigma, Landau mean, Gaus
         *  sigma)  */
        FitLikelihood& setIniValues(const double* iniValues);

        inline double* getIniValues(){ return _iniValues;}

        /**  Set lower limits of fitting parameters (Landau sigma, Landau mean, Gaus
         *  sigma).   */
        FitLikelihood& setParlimitslo(double minSl,double minMl,double minSg);
        /**  Set upper limits of fitting parameters (Landau sigma, Landau mean, Gaus
         *  sigma).   */
        FitLikelihood& setParlimitshi(double maxSl,double maxMl,double maxSg);

        /** Get  array of minimal values of fitting parameters (Landau sigma, Landau mean, Gaus
         *  sigma). */ 
        inline const double* getParMin() const{ return _parlimitslo;}
        /** Get  array of maximal values of fitting parameters (Landau sigma, Landau mean, Gaus
         *  sigma). */ 
        inline const double* getParMax() const{ return _parlimitshi;}

        /**  Writing root output of histogram h, fitted with created object lxg of class Langaus
         *  on frame rooFrame, canvas with name nameCan. Output directory is output. */
        RooPlot* writeRooroot(const TH1* h,const Langaus* lxg);



        /** Modification of histogram stored by constructor. Value for every bin of histogram is changed by mean of
         * neighbouring bins.
         * @param rbn - rebining of datas from _histogram.
         * @param rng - mean for bin "i" is created from bins in range \f$\left<i-rng,i+rng\right>\f$.
         * @param lps - number of smoothing repetitions.
         * @return mdified histogram
         * */
        TH1* cloneHisto(int rbn ,int rng ,int lps);
    private:
        //! rough estimation of peak position. Used as initial value for Landau mean.
        double _peakInit ;
        //! minimum of range of fit. Position of minimum between zero and rough peak position estimation. Used
        //for exclusion of pedestal contribution.
        double _rangeMin;
        //! range of fit
        double  _rangeInit[2];
        //! low limit of parameters
        /*! (Landau sigma, Landau mean, Gaus sigma) */
        double  _parlimitslo[3];
        //! high limit of parameters
        /*! (Landau sigma, Landau mean, Gaus sigma) */
        double  _parlimitshi[3];
        //! obtained parameters from fit
        /*! (Landau sigma, Landau mean, Gaus sigma) */
        double  _fitValues[3];
        //! errors of obtained parameters from fit
        double  _efitValue[3];
        //! initial values of parameters
        double  _iniValues[3];
        //! obtained peak from fit function
        /*! @see findPeak() */
        double  _peak;
        //! obtained error of found peak position
        /*! @see findePeak */
        double  _epeak;
        //* histogram for fit
        TH1   *_histogram;
        
        Bezier* _bezier;

};
#endif










