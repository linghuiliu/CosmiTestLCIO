#ifndef BEZIER_HH
#define BEZIER_HH


#include    <cstdlib>
#include    <cstdio>
#include    <iomanip>
#include    <iostream>
#include    <cmath>
#include    <vector>
#include    "TH1.h"
#include    "TGraph.h"
#include    "TMultiGraph.h"
/**
 * @brief Class for creation of Bezier curve with maximum order 
 * 32. If there would be implemented function for calculating with big numbers, it could be better. 
    @author Boris Bulanek, DESY
    @date   06/23/10
    */

class Bezier{
    public:
    //! Constructor 
    /** @param x - vector of x-axis values of dataset.
     *  @param y - vector of y-axis values of dataset.
     *  @param order - order of Bezier curve.
     *  @param zero - use also zero y-values from dataset (1). 
     **/
    Bezier(std::vector<float>& x,std::vector<float>& y, int order,bool zero);
    //! Constructor
    /** Constructor for creating the Bezier curve from data extracted from TH1 histogram.
     **/
    Bezier(TH1* histogram, int rebin, int order, bool zero);
    ~Bezier();
    //! Set the precision for Bezier curve as a function.
    /*! The Bezier curve as an function \f$f(x)\f$ is evaluating the value in range 
     * \f$\left<x-\mbox{precision}, x+\mbox{precision}\right>\f$.*/
    void setPrecision(float precision);
    //! Bezier curve as a function of x
    float   evaluate(float x);
    //! Bezier curve as a function of x. More proper for finding peak.
    float   evaluate2(float x);
    //! Finding the maximum value of a Bezier function.
    float   findPeak();


    TMultiGraph* plotBezier();

    private:

    float*  getPoint(float t, unsigned from);
    float   findPeak2();

    float   evaluate(float x, unsigned where);

    
    int binomic(int n, int k);
    template<class T>
    T  signum(T n);

    TH1*           _histogram;
    unsigned        _order;
    std::vector<float>  _xV,_yV;
    bool            _zero;
    float           _point[2];
    float           _precision;
    float           _peak;
	int				_wherePeak;
};
#endif
