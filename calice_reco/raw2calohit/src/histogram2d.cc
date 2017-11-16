/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * PlotMM Widget Library
 * Copyright (C) 2004   Andy Thaller
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the LGPL
 *****************************************************************************/
/* ported from qwt */
#ifdef HAVE_CALICEGUI
#include <glibmm/refptr.h>

#include <gdkmm/drawable.h>

#include <plotmm/doubleintmap.h>
#include "histogram2d.h"
#include <plotmm/paint.h>
#include <plotmm/supplemental.h>
#include <iostream>

using namespace PlotMM;

//! Initialize data members
void Histogram2D::init(const Glib::ustring &title)
{
    Curve::init(title);
    zMax_=-DBL_MAX;
    zMin_=DBL_MAX;
    scaleZrange_=true;
    draw_options_ = HISTOGRAM2D_BOX;
}

//! Copy the contents of a curve into another curve
void Histogram2D::copy(const Histogram2D &c)
{
    Curve::copy(c);

    hist_ = c.hist_;
    draw_options_ = c.draw_options_;
    xBinning_ = c.xBinning_;
    yBinning_ = c.yBinning_;
    zMin_ = c.zMin_;
    zMax_ = c.zMax_;
    scaleZrange_ = c.scaleZrange_;
}

//! Destructor
Histogram2D::~Histogram2D()
{
}

/*!
  \brief Ctor
  \param title title of the curve   
*/
Histogram2D::Histogram2D(const Glib::ustring &title)
    : Curve(title),
      zMin_(0),
      zMax_(0),
      scaleZrange_(true)
{
}

//! Copy Constructor
Histogram2D::Histogram2D(const Histogram2D &c)
    : Curve(c)
{
}

//! Copy Assignment
const Histogram2D& Histogram2D::operator=(const Histogram2D &c)
{
    if (this != &c)
    {
        copy(c);
        curve_changed();
    }

    return *this;
}


/*!
  \brief Set data by copying x- and y-values from specified memory blocks
  Contrary to \b Plot::set_curveRawData, this function makes a 'deep copy' of
  the data.

  \param xData pointer to x values
  \param yData pointer to y values
  \param size size of xData and yData

  \sa QwData::set_data.
  @todo handle null pointers correctly
*/
void Histogram2D::set_data(const Binning &x_binning, const Binning &y_binning,
				   double *flat_bin_array)
{
    xBinning_=x_binning;
    yBinning_=y_binning;

    if (!xBinning_.is_valid() || !yBinning_.is_valid()) {
	hist_.clear();
	return;
    }

    unsigned int n_bins_total=(yBinning_.n_bins()+2)*(xBinning_.n_bins()+2);
    // is larger than zero by construction
    //  if (n_bins_total<=0) return;

    hist_.resize(n_bins_total);
    
    //    for (UInt_t bin_i=0; bin_i<n_bins_total; bin_i++) {
    //	x_[bin_i]=flat_bin_array[bin_i];
    //    }
    double *src_iter=flat_bin_array;
    double a_min=DBL_MAX;
    double a_max=-DBL_MAX;
    for(std::vector<double>::iterator dest_iter=hist_.begin(); dest_iter!=hist_.end(); dest_iter++, src_iter++) {
	*dest_iter=*src_iter;
	if (*dest_iter>a_max) a_max=*dest_iter;
	if (*dest_iter<a_min) a_min=*dest_iter;
    }
    if (scaleZrange_ || zMin()>=zMax()) {
	zMin_=a_min;
	zMax_=a_max;
	scaleZrange_=true;
    }
    curve_changed();
}

void Histogram2D::set_data(const Binning &x_binning, const Binning &y_binning,
			   float *flat_bin_array)
{

    xBinning_=x_binning;
    yBinning_=y_binning;

    if (!xBinning_.is_valid() || !yBinning_.is_valid()) {
	hist_.clear();
	return;
    }

    unsigned int n_bins_total=(yBinning_.n_bins()+2)*(xBinning_.n_bins()+2);
    // is larger than zero by construction
    //  if (n_bins_total<=0) return;

    hist_.resize(n_bins_total);
    
    //    for (UInt_t bin_i=0; bin_i<n_bins_total; bin_i++) {
    //	x_[bin_i]=flat_bin_array[bin_i];
    //    }
    float *src_iter=flat_bin_array;
    double a_min=DBL_MAX;
    double a_max=-DBL_MAX;
    for(std::vector<double>::iterator dest_iter=hist_.begin(); dest_iter!=hist_.end(); dest_iter++, src_iter++) {
	*dest_iter=*src_iter;
	if (*dest_iter>a_max) a_max=*dest_iter;
	if (*dest_iter<a_min) a_min=*dest_iter;
    }
    if (scaleZrange_ || zMin()>=zMax()) {
	zMin_=a_min;
	zMax_=a_max;
	scaleZrange_=true;
    }
    curve_changed();
}



/*!
  Returns the bounding rectangle of the curve data. If there is
  no bounding rect, like for empty data the rectangle is invalid:
  DoubleRect.is_valid() == FALSE
*/

DoubleRect Histogram2D::bounding_rect() const
{
    if ( xBinning_.x_min() >=  xBinning_.x_max() || yBinning_.x_min() >=  yBinning_.x_max()) 
        return DoubleRect(1.0, -1.0, 1.0, -1.0); // invalid

    return DoubleRect(xBinning_.x_min(), xBinning_.x_max(), yBinning_.x_min(), yBinning_.x_max() );
}

/*!
  \brief Draw an intervall of the curve
  \param painter Painter
  \param xMap maps x-values into pixel coordinates.
  \param yMap maps y-values into pixel coordinates.
  \param from index of the first point to be painted
  \param to index of the last point to be painted. If to < 0 the 
         curve will be painted to its last point.

  \sa Histogram2D::draw_curve_, Histogram2D::draw_dots_,
      Histogram2D::draw_lines_, Histogram2D::draw_symbols_,
      Histogram2D::draw_lsteps_, Histogram2D::draw_csteps_,
      Histogram2D::draw_rsteps_, Histogram2D::draw_sticks_
*/
void Histogram2D::draw(const Glib::RefPtr<Gdk::Drawable> &painter,
    const DoubleIntMap &xMap, const DoubleIntMap &yMap, int from, int to)
{
    if (!xBinning_.is_valid() || !yBinning_.is_valid()) return;

    if ( data_size() <= 0 )
        return;

    if (to < 0)
        to = data_size() - 1;

    if ( verify_range(from, to) > 0 ) {
        //draw_curve_(painter, cStyle_, xMap, yMap, from, to);
        draw_box_(painter, 0, xMap, yMap);
	
    }
}

/*!
  \brief Draw the line part (without symbols) of a curve interval. 
  \param painter Painter
  \param style curve style, see Histogram2DStyleID
  \param xMap x map
  \param yMap y map
  \param from index of the first point to be painted
  \param to index of the last point to be painted
  \sa Histogram2D::draw
*/


void Histogram2D::draw_box_(const Glib::RefPtr<Gdk::Drawable> &painter, 
			    int style,
			    const DoubleIntMap &xMap, 
			    const DoubleIntMap &yMap)
{
    unsigned x0_i=xBinning_.find(xMap.d1());
    unsigned x1_i=xBinning_.find(xMap.d2());
    if (x0_i <= 0) x0_i = 1;
    if (x1_i > xBinning_.n_bins()) x1_i = xBinning_.n_bins();

    unsigned y0_i=yBinning_.find(yMap.d1());
    unsigned y1_i=yBinning_.find(yMap.d2());

    if (y0_i <= 0) y0_i=1;
    if (y1_i > yBinning_.n_bins() ) y1_i = yBinning_.n_bins();

    unsigned int bins_per_y_slice = xBinning_.n_bins()+2;

    unsigned int bin_x0_i = y0_i*bins_per_y_slice + x0_i;

    double width_x=xBinning_.bin_width();
    double width_y=yBinning_.bin_width();
   
    std::vector<Gdk::Point> polyline;

    double y = yBinning_.x_min() + (y0_i-1) * width_y;

    if ( (xMap.i2()-xMap.i1())  <= 0 ||  ( yMap.i1()-yMap.i2() ) <= 0) return;

    unsigned int combine_bins_x=1;
    if (  static_cast<unsigned int>(xMap.i2()-xMap.i1()) < 10 * (x1_i - x0_i) ) {
	combine_bins_x= (10 * (x1_i - x0_i)) / (xMap.i2()-xMap.i1());
	if (   static_cast<unsigned int>(xMap.i2()-xMap.i1())  < 10 * (x1_i - x0_i) / combine_bins_x) {
	    combine_bins_x++;
	}
    }

    unsigned int combine_bins_y=1;
    if (   static_cast<unsigned int>( yMap.i1()-yMap.i2() ) < 10 * (y1_i - y0_i) ) {
	combine_bins_y=  (10 * (y1_i - y0_i)) / ( yMap.i1()-yMap.i2() );
	if (   static_cast<unsigned int>( yMap.i1()-yMap.i2() ) < 10 * (y1_i - y0_i) / combine_bins_y) {
	    combine_bins_y++;
	}
    }
//    std::cout << " comb: x =" << combine_bins_x << "  y " << combine_bins_y << std::endl;
    
    if (combine_bins_y>1 || combine_bins_x>1) {
//	if (combine_bins_x<1) combine_bins_x=1;
//	if (combine_bins_y<1) combine_bins_y=1;
	double x_start=xBinning_.x_min() + (x0_i-1) * width_x ;

//	std::cout << " width_x = " << width_x << " -> " ;
	width_x*=combine_bins_x;
//	std::cout << width_x << std::endl;

//	std::cout << " width_y = " << width_y << " -> " ;
	width_y*=combine_bins_y;
//	std::cout << width_y << std::endl;

	for (unsigned y_i=y0_i; y_i<=y1_i; y_i+=combine_bins_y) {
	    
	    unsigned bin_i=bin_x0_i;
	    double x = x_start;
	    
	    for (unsigned x_i=x0_i; x_i<=x1_i; x_i+=combine_bins_x) {
		    
//	    assert ( bin_i < hisst_.size() );
		    int a_x0 = xMap.transform(x);
		    int a_x1 = xMap.transform(x+width_x);
		    if (a_x0> a_x1) {
			int temp=a_x1;
			a_x1=a_x0;
			a_x0=temp;
		    }
		    
		    int a_y0 = yMap.transform(y);
		    int a_y1 = yMap.transform(y+width_y);
		    if (a_y0> a_y1) {
			int temp=a_y1;
			a_y1=a_y0;
			a_y0=temp;
		    }
		    
		    double bin_content=0;
		    unsigned int bin_xstart_ii=bin_i;
		    unsigned int last_bin_x=( x_i+combine_bins_x > xBinning_.n_bins()+1 ? xBinning_.n_bins()+1 : x_i+combine_bins_x) ;
		    unsigned int last_bin_y=( y_i+combine_bins_y > yBinning_.n_bins()+1 ? yBinning_.n_bins()+1 : y_i+combine_bins_y) ;
		    for (unsigned y_ii=y_i; y_ii<last_bin_y; y_ii++) {
			unsigned int bin_ii=bin_xstart_ii;
			for (unsigned x_ii=x_i; x_ii<last_bin_x; x_ii++) {
			    bin_content+=hist_[bin_ii] - zMin() ;
			    bin_ii++;
			}
			bin_xstart_ii += bins_per_y_slice;
		    }
		    
		if (bin_content > 0 )  {
		    double scale = 0.5 * (bin_content ) / (( zMax() - zMin() ) * (combine_bins_y * combine_bins_x));
		    if (scale>.49) scale=.49;
		    if (scale<0) scale=0.;
//	    assert ( scale *.99 < .5 );
		    int b_y0= static_cast<int>( 0.5 * (a_y0 + a_y1) - (a_y1-a_y0) * scale);
		    int b_y1= static_cast<int>( 0.5 * (a_y0 + a_y1) + (a_y1-a_y0) * scale);
		    
		    int b_x0= static_cast<int>( 0.5 * (a_x0 + a_x1) - (a_x1-a_x0) * scale);
		    int b_x1= static_cast<int>( 0.5 * (a_x0 + a_x1) + (a_x1-a_x0) * scale);

//		    std::cout <<  x << "," << y << " = " << bin_content  << " -> " << scale 
//			      << " x : "  << a_x0 << " , " << a_x1 
//			      << " -> " << b_x0 << "," << b_x1  << " -> " << (b_x1-b_x0 > 0 ? b_x1-b_x0 : 1 )
//			      << " y : " << a_y0 << ", "<< a_y1 << " -> " << b_y0 << "," << b_y1  << " -> " << (b_y1-b_y0 > 0 ? b_y1-b_y0 : 1)
//			      << std::endl;
		    
		    painter->draw_rectangle(paint()->pen(painter), false, b_x0, b_y0, (b_x1-b_x0 > 0 ? b_x1-b_x0 : 1 ), (b_y1-b_y0 > 0 ? b_y1-b_y0 : 1));
		}
		x+= width_x;
		
		bin_i+=combine_bins_x;
	    }
	    y += width_y;
	    bin_x0_i += bins_per_y_slice*combine_bins_y;
	}
    }
    else {
    for (unsigned y_i=y0_i; y_i<=y1_i; y_i++) {

	unsigned bin_i=bin_x0_i;
	double x = xBinning_.x_min() + (x0_i-1) * width_x ;

	for (unsigned x_i=x0_i; x_i<=x1_i; x_i++) {
	    if (hist_[bin_i] > zMin())  {

//	    assert ( bin_i < hisst_.size() );
	    int a_x0 = xMap.transform(x);
	    int a_x1 = xMap.transform(x+width_x);
	    if (a_x0> a_x1) {
		int temp=a_x1;
		a_x1=a_x0;
		a_x0=temp;
	    }

	    int a_y0 = yMap.transform(y);
	    int a_y1 = yMap.transform(y+width_y);
	    if (a_y0> a_y1) {
		int temp=a_y1;
		a_y1=a_y0;
		a_y0=temp;
	    }

	    double scale = 0.5 * (hist_[bin_i]-zMin()) / ( zMax() - zMin());
//	    assert ( scale *.99 < .5 );
//	    int b_y0= 0.5 * (a_y0 + a_y1) - static_cast<int>((a_y1-a_y0) * scale);
//	    int b_y1= 0.5 * (a_y0 + a_y1) + static_cast<int>((a_y1-a_y0) * scale);

//	    int b_x0= 0.5 * (a_x0 + a_x1) - static_cast<int>((a_x1-a_x0) * scale);
//	    int b_x1= 0.5 * (a_x0 + a_x1) + static_cast<int>((a_x1-a_x0) * scale);
	    int b_y0= static_cast<int>( 0.5 * (a_y0 + a_y1) - (a_y1-a_y0) * scale);
	    int b_y1= static_cast<int>( 0.5 * (a_y0 + a_y1) + (a_y1-a_y0) * scale);
	    
	    int b_x0= static_cast<int>( 0.5 * (a_x0 + a_x1) - (a_x1-a_x0) * scale);
	    int b_x1= static_cast<int>( 0.5 * (a_x0 + a_x1) + (a_x1-a_x0) * scale);
	    
	    painter->draw_rectangle(paint()->pen(painter), false, b_x0, b_y0, (b_x1-b_x0), (b_y1-b_y0));
	    }
	    x+= width_x;

	    bin_i++;
	}
	y += width_y;
	bin_x0_i += bins_per_y_slice;
    }
    }

}
#endif
