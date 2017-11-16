#ifndef _HistogramDisplay_hh_
#define _HistogramDisplay_hh_
#ifdef HAVE_CALICEGUI

#include "Display.hh"
#include "GraphDisplay.hh"
#include "DisplayKit.hh"
//#include <plotmm/curve.h>
#include <gtkmm/box.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/combo.h>
#include <gtkmm/statusbar.h>
#include <glibmm/ustring.h>
#ifndef __APPLE__ 
#include <values.h>
#else
#include <limits.h>
#include <float.h>
#endif

#include <algorithm>

#include "KeyMap_t.hh"
#include "HistMgr.hh"

#include <map>
namespace histmgr {

class FloatHistogram1D;
class FloatHistogram2D;

/** The Drift chamber reconstruction display.
 */
class HistogramDisplay : public Gtk::VBox, public Display
{
public:
  HistogramDisplay();

  Gtk::Widget *getMainWidget() {return this;};

protected:

  /** show next event.
   */
  void onEventChanged();

protected:
  void status(Glib::ustring text);

  void updateHistogramDisplay();

  void onHistogramGroupChanged();
  void onHistogramCollectionChanged();
  void onFirstIndexChanged();
  void onRebinChanged();
  void onLogYChanged();

private:
  // -- GUI
  Gtk::Statusbar      _status;
  Gtk::HBox           _hbox;
  Gtk::VBox           _graphList;
  Gtk::ScrolledWindow _graphWindow;
  Gtk::Combo          _histogramGroupSelector;
  Gtk::Combo          _histogramCollectionSelector;
  Gtk::CheckButton    _showTotalHistogram;
  Gtk::Label          _firstIndexLabel;
  Gtk::SpinButton     _firstIndexSelector;
  Gtk::Label          _rebinLabel;
  Gtk::SpinButton     _rebinSelector;
  Gtk::CheckButton    _logY;

  struct HistPar_t {
    HistPar_t() : _firstIndex(0), _rebin(0), _logY(false), _maxY(-DBL_MAX),_minY(DBL_MAX) {};
    void setFirstIndex(UInt_t first_index) {_firstIndex=first_index;};
    void setRebin(UInt_t rebin) {_rebin=rebin;};
    void setLogY(bool log_scale) {_logY=log_scale;};

    UInt_t rebin() const {return _rebin;};
    UInt_t firstIndex() const {return _firstIndex;};
    bool logY() const {return _logY;};

    void setValue(Double_t a_value) {
      _maxY=std::max(a_value,_maxY);
      _minY=std::min(a_value,_minY);
    };

    void setRange(Double_t min_val, Double_t max_val) {
      _maxY=max_val;
      _minY=min_val;
    };

    void resetRange() {
      _maxY=-DBL_MAX;
      _minY=DBL_MAX;
    };

    const Double_t &min() const {return _minY;};
    const Double_t &max() const {return _maxY;};

    UInt_t _firstIndex;
    UInt_t _rebin;
    bool   _logY;
    Double_t _maxY;
    Double_t _minY;
  };

  //  std::map< Glib::ustring, Glib::ustring > _selectedCollectionPerGroup;
  //  std::map< Glib::ustring, HistPar_t > _parameterPerHistogramCollection;

  class GroupPar_t {
  public:
    GroupPar_t() {};

    HistPar_t &par(const Key_t &hist_key) {
      if (!_par.hasKey(hist_key)) {
	_par.assign(hist_key,HistPar_t());
	return _par[hist_key];
      }
      else {
	return _par[hist_key];
      }
    };

    const HistPar_t &par(const Key_t &hist_key) const {
#ifdef BOUNDARY_CHECK
      assert( _par.hasKey(hist_key) );
#endif
      return _par[hist_key];
    };

    void setCurrent(const Key_t &current) {_current=current;}
    const Key_t &current() const {return _current;}

  private:
    Key_t _current;
    KeyMap_t< HistPar_t > _par;
    
  };

  HistPar_t &getHistPar(const Key_t &group_key, HistMgr::ECollectionType type, const Key_t &hist_key) {
    try {
    if (!_groupPar[type].hasKey(group_key)) {
      _groupPar[type].assign(group_key,GroupPar_t());
      return _groupPar[type][group_key].par(hist_key);
    }
    else {
      return _groupPar[type][group_key].par(hist_key);
    }
    } 
    catch (std::logic_error &err) {
      std::cerr << "getHistPar::" << err.what() << std::endl;
      throw err;
    }
  }

  const HistPar_t &getHistPar(const Key_t &group_key, HistMgr::ECollectionType type, const Key_t &hist_key) const {
#ifdef BOUNDARY_CHECK
    assert ( type < HistMgr::kNTypes );
    assert (_groupPar[type].hasKey(group_key));
#endif
    return _groupPar[type][group_key].par(hist_key);
  }

  const Key_t &currentHistogramCollection(const Key_t &group_key, HistMgr::ECollectionType type) const {
    return _groupPar[type][group_key].current();
  }

  void setCurrentHistogramCollection(const Key_t &group_key, HistMgr::ECollectionType type, const Key_t &new_key) {
    if (!_groupPar[type].hasKey(group_key)) {
      _groupPar[type].assign(group_key,GroupPar_t());
    }
    _groupPar[type][group_key].setCurrent(new_key);
  }

  bool hasCurrent(const Key_t &group_key, HistMgr::ECollectionType type) const {
    if (! _groupPar[type].hasKey(group_key)) return false;
    return (_groupPar[type][group_key].current().isSet() || !_groupPar[type][group_key].current().name().empty());
  }

  KeyMap_t< GroupPar_t > _groupPar[HistMgr::kNTypes];

  //  std::vector< std::pair< Glib::ustring, HistPar_t > > _parameterPerHistogramCollection;

  void setGroupNames();
  void setHistogramCollectionNames();

  void createHistogramGraphs(const Glib::ustring &title, const Glib::ustring &x_axis_title, 
			     UInt_t n_hist, const Float_t &x_min, const Float_t &x_max, 
			     const HistPar_t &hist_par);

  void setGraph(std::vector< Gtk::Widget * > &graphs, UInt_t hist_i, 
		const Glib::ustring &title, const FloatHistogram1D * const a_hist,
		HistPar_t &a_par);

  void setGraph(std::vector< Gtk::Widget * > &graphs, UInt_t hist_i, 
		const Glib::ustring &title, const FloatHistogram2D * const a_hist, 
		HistPar_t &hist_par);

  void showHistogramCollection(const histmgr::Key_t &group_key, HistMgr::ECollectionType type, const histmgr::Key_t &hist_key);
  void showHistogram2DCollection(const histmgr::Key_t &group_key, HistMgr::ECollectionType type, const histmgr::Key_t &hist_key);

  void replotGraphs(const HistPar_t &hist_par);

  std::vector< Glib::RefPtr<PlotMM::Curve> > _curveList;
  std::vector< Glib::RefPtr<PlotMM::Curve> > _underflowCurveList;
  std::vector< Glib::RefPtr<PlotMM::Curve> > _overflowCurveList;

  std::string _groupListMD5; /**< checksum of the group list. */
  std::string _histListMD5; /**< checksum of the histogram list. */
  HistMgr::ECollectionType _lastCollectionType;
};

/** Helper class to create the display object in the GUI thread.
 */
class HistogramDisplayKit : public DisplayKit
{
public:
  Display *createDisplay() const {return new HistogramDisplay;};
};

}
#endif //calice gui
#endif
