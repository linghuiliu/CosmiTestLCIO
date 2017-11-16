#ifndef LANGAUS_HH
#define LANGAUS_HH
#include    <iostream>
#include    <iomanip>
#include    <fstream>
#include    <sstream>
#include    <map>
#include    <exception>
#include    <stdexcept>
#include    <cmath>

#include    "TROOT.h"
#include    "RooFit.h"
#include    "RooAbsReal.h"
#include    "RooRealVar.h"
#include    "RooFFTConvPdf.h"
#include    "RooNumConvPdf.h"
#include    "RooLandau.h"
#include    "RooGaussian.h"
#include    "RooPoisson.h"
#include    "RooDataHist.h"
#include    "RooAbsPdf.h"
#include    "RooFitResult.h"
#include    "RooPlot.h"

/**
 * @brief Class for handling with a fitting function defined as 
 * \f$ (l\otimes g)(x)=\int_{-\infty}^{\infty}\mbox{d}y\ l(x-y)\cdot g(y)\f$,
 * where \f$l(x)\f$ is a Landau function and \f$ g(x)\f$ is a Gaus function.

 Detailed description follows here.
 @author Boris Bulanek, DESY
 @date June 2010
 */


class Langaus{
    public: 
        Langaus();
        //! Constructor (Initialization of range, initial parameters, low and high limits of parameters)
        /*! @see _fitrange
         *  @see _startValues
         *  @see _parlimitslo
         *  @see _parlimitshi*/
        Langaus(const double* fitrange,const double* startValues,const double* parlimitslo, const double* parlimitshi, bool useFFT);
        ~Langaus(void);

        RooAbsReal  *getT(void)        {return _t;       }
        RooAbsReal  *getMg(void)       {return _mg;      }
        RooAbsReal  *getSg(void)       {return _sg;      }
        RooAbsReal  *getMl(void)       {return _ml;      }
        RooAbsReal  *getSl(void)       {return _sl;      }
        RooLandau   *getLandau(void)   {return _landau;  }
        RooGaussian *getGauss(void)    {return _gauss;   }

        double       getPeak()         {return _peak;    }
        double       getEpeak()        {return _ePeak;   }
        double       getLogL()         {return _logL;    }
        double       getChiSquare();

        double       getMlerror(void)  {return _mlerror; }
        double       getSlerror(void)  {return _slerror; }
        double       getSgerror(void)  {return _sgerror; }

        Langaus&     setMl(double ml);
        Langaus&     setSl(double sl);
        Langaus&     setSg(double sg);

        //! Set range of fit.
        Langaus& setFitRange(double *fitrange){ _fitrange = fitrange; return *this; }
        //! Set start values (Landau sigma, Landau mean, Gaus sigma).
        Langaus& setStartValues(double  *startValues){_startValues=startValues; return *this; }
        //! Set low limits of parameters.
        Langaus& setParlimitslo(double  *parlimitslo){_parlimitslo=parlimitslo; return *this; }
        //! Set high limits of parameters.
        Langaus& setParlimitshi(double  *parlimitshi){_parlimitshi=parlimitshi; return *this; }


        /** Return RooFit object for function \f$(l\otimes g)(x)\f$ computed via Fast Fourier transformation */
        inline RooFFTConvPdf *getLangaus() const       {return this->_lxg;}
        /** Return RooFit object for function \f$(l\otimes g)(x)\f$ computed numericaly */
        inline RooNumConvPdf *getLangausNum() const { return this->_lxgNum;}
        /** Return RooFit object for function \f$(l\otimes g)(x)\f$ without regarding on computation type */
        inline RooAbsReal* getLangausBase() const{
            if(_useFFT==1) return dynamic_cast<RooAbsReal*>(_lxg);
            return dynamic_cast<RooAbsReal*>(_lxgNum);
        }


        /** Obtain datas from histogram.*/
        Langaus& setData(TH1 *h);

        /** Get data object created with setData.*/
        RooDataHist   *getData();

        /** Find peak position. */
        double         findPeak();
        /** Find peak position for given parameters. Precision of finding the peak position is given by number
         * of points in Fast Fourier transformation (FFT) and by ratio of width of "binning" to range of fit for
         * direct numerical computation.
         * The number of points for FFT can be adjusted by setFFTCache and ratio by setNumRatio.
         * @param par - pointer to array of three parameters:
         * par[0]=Landau mean, par[1]=Landau sigma, par[2]=Gaus sigma
         * */
        double         findPeak(const double *par);



        /**Calculate error of peak position \f$A(x|\rho)\f$, where \f$\rho\f$ are parameters of
         * function. 
         * The error is calculated with the formula
         * \f$ \sigma^2_{peak}=\sum_{i,j} \frac{dA(x=peak|\rho)}{d\rho_i} \mbox{cor}(\rho_i,\rho_j)\cdot
         * {\sigma_{\rho}}_i\cdot{\sigma_{\rho}}_j   \frac{dA(x=peak|\rho)}{d\rho_j}\f$.
         * The correlation factors \f$\mbox{cor}(\rho_i,\rho_j)\f$ are obtained by fit. 
         * @see getCorrelation(const char*, const char*)
         */
        double          findePeak();


        /**  Get RooFitResult object by fitting of RooDataHist. */
        RooFitResult*   Fit();

        /**  Make whole fit of dataset with numerical finding of peak position and propagated error estimation.
         *   @see findPeak()
         *   @see Fit()
         *   @param h - fitted histogram
         *   @param fitparams - found parameters: Landau sigma, Landau mean, Gaus sigma
         *   @param fiterrors - found errors of fitparams
         *   @param peak - found peak position.
         *   @param epeak - found error of peak position.
         *   100000, numerical calculation: 0.00001 )
         * */
        void            langaufit(const TH1* h,  double* fitparams, double* fiterrors, double& peak ,double& epeak );


        /** Obtain fit correlation factors between parameters (Landau mean, Landau sigma, Gaus sigma) obtained
         * by fit.  
         * @param par1, par2 - parameters of fitting function given as: "ml"(Landau mean), "sl"(Landau sigma), "sg"(Gaus sigma).
         * @return Correlation between parameters given as inputs. If inputs are wrong, return value is 0.
         * */
        double          getCorrelation(const char* par1,const char* par2);


    private:
        /** Sampling of fit range for Fast Fourier transformation (number of points used for
         * computation of function).  */
        static int FFT_CACHE;
        /** Precision of finding peak by number given like ratio of width of "binning" to range of fit for
         * direct numerical computation. */
        static float NUM_RATIO;
        /** Sampling of interval, where is iterative finding peak. */
        static int NUM_SAMPLING;

        /** Precision of numerical computation of convolution integral. (Not documented in RooFit.)*/
        static float EPS_REL;
        /** Precision of numerical computation of convolution integral. (Not documented in RooFit.)*/
        static float EPS_ABS;

        /*! Set the method of convolution function computing (Fast Fourier transformation, Numerical computation) */
        bool _useFFT;
        //! range of fit
        /*! \f$\left<\mbox{\_fitrange[0]},\mbox{\_fitrange[1]}\right>\f$ */
        const double        *_fitrange;

        //! starting values
        /*! (Landau sigma, Landau mean, Gaus sigma) */
        const double        *_startValues;
        //! low limit of parameters
        /*! (Landau sigma, Landau mean, Gaus sigma) */
        const double        *_parlimitslo;
        //! high limit of parameters
        /*! (Landau sigma, Landau mean, Gaus sigma) */
        const double        *_parlimitshi;

        //! position of most probable value
        double         _peak;
        //! error of most probable value
        double         _ePeak;
        //! \f$ -\log(L)\f$, where L is a likelihood function
        double         _logL;

        RooRealVar    *_t;
        RooRealVar    *_mg;
        RooRealVar    *_sg;
        RooRealVar    *_ml;
        RooRealVar    *_sl;
        RooLandau     *_landau;
        RooGaussian   *_gauss;
        RooFFTConvPdf *_lxg;
        RooNumConvPdf *_lxgNum;
        RooDataHist   *_data;
        RooArgSet     *_var;
        RooFitResult  *_r;

        double _mlerror,_slerror,_sgerror;
};
#endif
