/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Plot Widget Library
 * Copyright (C) 2004   Andy Thaller
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the LGPL
 *****************************************************************************/
/* ported from qwt */

#ifndef PLOTMM_HISTOGRAM_TWOD_H
#define PLOTMM_HISTOGRAM_TWOD_H
#ifdef HAVE_CALICEGUI

#include <vector>

#include <plotmm/compat.h>
#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION > 4)
#include <glibmm/objectbase.h>
#else
#include <sigc++/object.h>
#endif

#include <glibmm/arrayhandle.h>
#include <glibmm/ustring.h>

#include <plotmm/curve.h>


namespace Gtk {
    class Drawable;
}

namespace PlotMM {

    class Binning 
    {
    public:
	Binning() : n_bins_(0), x_min_(0), x_max_(0) {}
	Binning(unsigned int n_bins, double x_min, double x_max) : n_bins_(n_bins), x_min_(x_min), x_max_(x_max) {}

	double x_min() const {return x_min_;}
	double x_max() const {return x_max_;}
	unsigned int n_bins() const {return n_bins_;}

	double bin_width() { return (n_bins()>0 ? (x_max()-x_min())/n_bins() : 0);}

	unsigned int find(double x) { if ( x >= x_max() ) return n_bins()+1; else if ( x < x_min() ) return 0; else return static_cast<unsigned int>((x-x_min())*n_bins()/(x_max()-x_min())); }

	bool is_valid()  const {return n_bins()>0 && x_min()< x_max();}


    private:
	unsigned n_bins_;
	double x_min_;
	double x_max_;
    };

    /*! 
      Histogram2D options. 
      \sa Histogram2D::set_options
    */
    enum Histogram2DOptions
    {
	HISTOGRAM2D_BOX = 0,
	HISTOGRAM2D_COLZ = 1
    };


    /*! @brief A class which draws a 2d histogram
     *
     *  This class can be used to display data as a curve in the  x-y plane.
     *  It supports different display styles and symbols.
     *
     *  \par Usage 
     *  <dl><dt>A. Assign curve properties</dt> 
     *      <dd>When a curve is created, it is configured to draw black solid
     *          lines with CURVE_LINES and no symbols. You can change this by
     *          calling Curve::set_pen(), Curve::set_style() and
     *          Curve::set_symbol().</dd> 
     *      <dt>B. Assign or change data.</dt> 
     *      <dd>use one of the provided Curve::set_data() functions. The 
     *          curve's x and y data are assigned by copying from different 
     *          data structures.</dd>
     *      <dt>C. Draw</dt>
     *      <dd>Curve::draw() maps the data into pixel coordinates and paints 
     *          them.  </dd>
     *  </dl>
     *     
     *  \par Example:
     *  see examples/curvdemo
     *
     *  \sa Symbol, DoubleIntMap
     */
    class Histogram2D : public Curve
    {
    public:
	Histogram2D(const Glib::ustring &title = "");
	Histogram2D(const Histogram2D &h2);
	~Histogram2D();

	const Histogram2D& operator= (const Histogram2D &h2);

	virtual void set_data(const Binning &x_binning, const Binning &y_par,
			      double *flat_bin_array);
	virtual void set_data(const Binning &x_binning, const Binning &y_par,
			      float *flat_bin_array);

	int data_size() const {
	    return hist_.size();
	}

	DoubleRect bounding_rect() const;

//	virtual void set_x_axis(Binning &x_binning);
//	virtual void set_y_axis(Binning &y_binning);

//	virtual void set_bin_content(unsigned int bin_i, double value);
//	unsigned int get_bin(unsigned int bin_x_i, unsigned int bin_y_i) const;

//	virtual void set_bin_content(unsigned int bin_x_i, unsigned int bin_y_i, double value);
//	virtual double get_bin_content(unsigned int bin_x_i, unsigned int bin_y_i);


	void set_baseline(double ref) {};
	double baseline() const {return 0.;};

	void draw(const Glib::RefPtr<Gdk::Drawable> &p,
			  const DoubleIntMap &xMap, const DoubleIntMap &yMap,
			  int from = 0, int to = -1);

	virtual void set_draw_options(const Histogram2DOptions &draw_options) {
	    draw_options_=draw_options;
	    curve_changed();
	}

	virtual Histogram2DOptions draw_options() const {
	    return draw_options_;
	}

	double zMin() const {return zMin_;}
	double zMax() const {return zMax_;}
	

    protected:
	void init(const Glib::ustring &title);
	void copy(const Histogram2D &h2);

	virtual void draw_box_(const Glib::RefPtr<Gdk::Drawable> &p, 
			       int style,
			       const DoubleIntMap &xMap, 
			       const DoubleIntMap &yMap);

	virtual void draw_colz_(const Glib::RefPtr<Gdk::Drawable> &p, 
				int style,
				const DoubleIntMap &xMap, 
				const DoubleIntMap &yMap) {}

	virtual void draw_curve_(const Glib::RefPtr<Gdk::Drawable> &p, 
				 int style,
				 const DoubleIntMap &xMap, 
				 const DoubleIntMap &yMap,
				 int from, int to) {}

	void draw_symbols_(const Glib::RefPtr<Gdk::Drawable> &p,
				   const Glib::RefPtr<Symbol> &,
				   const DoubleIntMap &xMap,
				   const DoubleIntMap &yMap,
			   int from, int to) {}

	void draw_lines_(const Glib::RefPtr<Gdk::Drawable> &p,
			 const DoubleIntMap &xMap,
			 const DoubleIntMap &yMap,
			 int from, int to) {}
	void draw_sticks_(const Glib::RefPtr<Gdk::Drawable> &p,
			  const DoubleIntMap &xMap, 
			  const DoubleIntMap &yMap,
			  int from, int to) {}

	void draw_dots_(const Glib::RefPtr<Gdk::Drawable> &p,
				const DoubleIntMap &xMap,
				const DoubleIntMap &yMap,
				int from, int to) {};
	void draw_lsteps_(const Glib::RefPtr<Gdk::Drawable> &p,
				  const DoubleIntMap &xMap,
				  const DoubleIntMap &yMap,
				  int from, int to) {};
	void draw_csteps_(const Glib::RefPtr<Gdk::Drawable> &p,
				  const DoubleIntMap &xMap,
				  const DoubleIntMap &yMap,
				  int from, int to) {};
	void draw_rsteps_(const Glib::RefPtr<Gdk::Drawable> &p,
				  const DoubleIntMap &xMap,
				  const DoubleIntMap &yMap,
				  int from, int to) {};
	void close_polyline_(const DoubleIntMap &, 
				     const DoubleIntMap &,
				     std::vector<Gdk::Point> &) const {};

//	virtual int verify_range(int &i1, int &i2);

    private:

	unsigned int first_y_bin() const {return 1;};
	unsigned int last_y_bin()  const {return yBinning_.n_bins();};

	unsigned int bin_offset(unsigned int bin_y_i)         const { return (xBinning_.n_bins()+2)*bin_y_i;};
    public:
	unsigned int get_bin(unsigned int bin_x_i, unsigned int bin_y_i)  const {return bin_offset(bin_y_i)+bin_x_i;};
    private:
	unsigned int last_x_bin_offset(unsigned int bin_y_i)  const {return bin_offset(bin_y_i)+xBinning_.n_bins();};
	unsigned int first_x_bin_offset(unsigned int bin_y_i) const {return bin_offset(bin_y_i)+1;};
	unsigned int overflow_x_bin(unsigned int bin_y_i)     const {return bin_offset(bin_y_i)+xBinning_.n_bins()+1;};
	unsigned int underflow_x_bin(unsigned int bin_y_i)    const {return bin_offset(bin_y_i);};
	unsigned int overflow_y_bin(unsigned int bin_x_i)     const {return get_bin(bin_x_i,yBinning_.n_bins()+1);};
	unsigned int underflow_y_bin(unsigned int bin_x_i)    const {return get_bin(bin_x_i,0);};

	Histogram2DOptions draw_options_;

	std::vector<double> hist_;

	Binning xBinning_;
	Binning yBinning_;
	double zMin_;
	double zMax_;
	bool   scaleZrange_;
    };
}

#endif //calice gui
#endif
