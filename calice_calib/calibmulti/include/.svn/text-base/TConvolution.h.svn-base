#ifndef TCONVOLUTION_HH
#define TCONVOLUTION_HH 1

#include "TObject.h"
#include "Rtypes.h"
#include "TF1.h"
#include <string>


/**
 * @brief ROOT class which generates the convolution of two functions
 *
 * The convoluted function corresponds to
 * @f[ f_{conv}(x) =  \int_{min_{i}}^{max_{i}}{f_{1}(t) \cdot f_{2}(x-t) \textrm{d}t} @f]
 * where @f$ min_{i} @f$ (@f$ max_{i} @f$) is the minimum (maximum) of the range of function @f$ f_{i} @f$.
 * The integral is calculated as the Riemann sum with selectable number of steps.
 *
 * The range of the input function @f$ f_{1} @f$ and the number of steps in the integration have to be
 * chosen reasonably to achieve a good result with a acceptable computing effort.
 * If the integral of one of the functions is easier to approximate it should be used as @f$ f_{1} @f$.
 *
 * An additional scaling parameter can be muliplied to the result of the convolution.
 * This is for fitting to data.
 *
 * @attention This class needs ROOT version 5.16 or above.
 *
 * @author Benjamin.Lutz@desy.de
 * @version 1.0
 * @date September 2008
 */
class TConvolution : public TObject {

public:

  /**
   * @brief Standard constructor. The Init() function has to be used to set the parameters.
   */
  TConvolution();
  /**
   * @brief Constructor with initialisation parameters. See Init() function for parameter description.
   */
  TConvolution(const char* name, TF1* f1, TF1* f2, bool addScale = true, Int_t stepsPerPoint = 50);
  ~TConvolution();

  /**
   * @brief Initialisation function.
   *
   * @param name          Name of the resulting convoluted function. This is the ROOT-Object-name and has to be unique.
   * @param f1            First function of the convolution
   * @param f2            Second function of the convolution
   * @param addScale      If this is true, a scale parameter is multiplied to the result of the convolution.
   *                      This allows to use normalized functions in the convolution. The ROOT TF1-parameter number will be
   *                      @f$ N_{f1} + N_{f2} + 1 @f$, where @f$ N_{x} @f$ is the number of parameters of input funciton @f$ x @f$
   * @param stepsPerPoint Number of steps in the approximation of the convolution integral.
   */
  void Init(const char* name, TF1* f1, TF1* f2, bool addScale = true, Int_t stepsPerPoint = 50);

  /**
   * @brief get the resulting function
   * @return pointer to the convoluted function
   */
  TF1* GetConvolutedFunction();

  /**
   * @brief get access to the first internal function
   * @return pointer to the internal copy of the first function
   */
  TF1* GetF1() {return _f1;};
  /**
   * @brief get access to the second internal function
   * @return pointer to the second copy of the first function
   */
  TF1* GetF2() {return _f2;};

  /**
   * This operator allows that the class can be used to initialise a ROOT TF1() function
   *
   * @param x   pointer to array of input variables (in this case pointer to x)
   * @param par pointer to array of function parameters
   * @return value of the convoluted function
   */
  Double_t operator()(Double_t *x, Double_t *par) {return convolutionFkt(x,par);};

private:

  std::string _name;

  Double_t integrand(Double_t t, Double_t x0);
  Double_t convolutionFkt(Double_t *x, Double_t *par);


  TF1* _f1;
  TF1* _f2;
  TF1* _convolution;
  Int_t _nPar1, _nPar2, _baseNpar, _stepsPerPoint, _scaleParNumber;
  Double_t _f1Rmin, _f1Rmax,_f2Rmin, _f2Rmax,_Rmin,_Rmax;
  bool _addScale;

public:
  ClassDef(TConvolution,1)  // magic ROOT stuff
    };

#endif
