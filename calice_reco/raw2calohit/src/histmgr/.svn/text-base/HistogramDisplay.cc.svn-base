#ifdef HAVE_CALICEGUI
#include "HistogramDisplay.hh"
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <cassert>

#include <plotmm/curve.h>
#include <plotmm/paint.h>
#include <glibmm/listhandle.h>
#include <GuiThread.hh>

#include "HistMgr.hh"
#include "FloatHistogram1D.hh"
#include "plotmmext/histogram2d.h"

#include <md5.hh>

namespace histmgr {


HistogramDisplay::HistogramDisplay() 
  : Gtk::VBox(false, 3),
    _status(),
    _showTotalHistogram("Show total histogram"),
    _firstIndexLabel("Major/Layer Index"),
    _rebinLabel("Combine bins"),
    _logY("Log Y")
{   
 
  pack_start(_hbox,Gtk::PACK_SHRINK,5); 
  pack_start(_graphWindow,Gtk::PACK_EXPAND_WIDGET,5); 
  pack_start(_status,Gtk::PACK_SHRINK,5); 

  _graphWindow.add(_graphList);
  _graphWindow.set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC);

  _histogramGroupSelector.get_entry()->set_width_chars(40);
  _histogramCollectionSelector.get_entry()->set_width_chars(40);

  _hbox.pack_start(_histogramGroupSelector,Gtk::PACK_SHRINK,5);
  _hbox.pack_start(_histogramCollectionSelector,Gtk::PACK_SHRINK,5);
  _hbox.pack_start(_firstIndexLabel,Gtk::PACK_SHRINK,5);
  _hbox.pack_start(_firstIndexSelector,Gtk::PACK_SHRINK,5);
  _hbox.pack_start(_rebinLabel,Gtk::PACK_SHRINK,5);
  _hbox.pack_start(_rebinSelector,Gtk::PACK_SHRINK,5);
  _hbox.pack_start(_logY,Gtk::PACK_SHRINK,5);

  _firstIndexSelector.set_numeric(true);
  _firstIndexSelector.set_range(0,0);
  _firstIndexSelector.set_increments(1,5);
  _firstIndexSelector.set_sensitive(false);

  _rebinSelector.set_numeric(true);
  _rebinSelector.set_range(0,100);
  _rebinSelector.set_increments(1,5);
  _rebinSelector.set_sensitive(true);

  setGroupNames();


#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION >= 4)
  //    _moduleSelector.signal_value_changed().connect(sigc::mem_fun(*this, &HistogramDisplay::onModuleChanged));
  //    _oneModuleButton.signal_toggled().connect(sigc::mem_fun(*this, &HistogramDisplay::onDisplayModeToggled));

    _histogramGroupSelector.get_entry()->signal_changed()
      .connect( sigc::mem_fun(*this, &HistogramDisplay::onHistogramGroupChanged) );

    _histogramCollectionSelector.get_entry()->signal_changed()
      .connect( sigc::mem_fun(*this, &HistogramDisplay::onHistogramCollectionChanged) );

    _firstIndexSelector.signal_value_changed().connect( sigc::mem_fun(*this, &HistogramDisplay::onFirstIndexChanged) );

    _rebinSelector.signal_value_changed().connect( sigc::mem_fun(*this, &HistogramDisplay::onRebinChanged) );

    _logY.signal_toggled().connect( sigc::mem_fun(*this, &HistogramDisplay::onLogYChanged) );
#else
    _histogramGroupSelector.get_entry()->signal_changed()
      .connect( SigC::slot(*this, &HistogramDisplay::onHistogramGroupChanged) );

    _histogramCollectionSelector.get_entry()->signal_changed()
      .connect( SigC::slot(*this, &HistogramDisplay::onHistogramCollectionChanged) );

    _firstIndexSelector.signal_value_changed().connect( SigC::slot(*this, &HistogramDisplay::onFirstIndexChanged) );

    _rebinSelector.signal_value_changed().connect( SigC::slot(*this, &HistogramDisplay::onRebinChanged) );

    _logY.signal_toggled().connect( SigC::slot(*this, &HistogramDisplay::onLogYChanged) );
#endif

  show_all();
  onHistogramGroupChanged();
}

void HistogramDisplay::setGroupNames()
{
  HistMgrPtr histograms(HistMgr::getInstance());
  if (!histograms) return;

  if (histograms->getNGroups() != _histogramGroupSelector.get_popdown_strings().size()) {
    std::vector<std::string> group_names;
    histograms->fillGroupList(group_names);

    Glib::ustring current_selection=_histogramGroupSelector.get_entry()->get_text();

    // find out whether the group list has changed
    MD5 md5;
    bool selection_is_valid=false;
    for (std::vector<std::string>::const_iterator group_iter=group_names.begin();
	 group_iter!=group_names.end(); 
	 group_iter++) {
      md5.update(reinterpret_cast<const unsigned char*>(group_iter->c_str()), group_iter->size() );
      if (!selection_is_valid && current_selection == Glib::ustring(*group_iter)) {
	selection_is_valid=true;
      }
    }
    md5.finalize();
    std::string hex_digest=md5.hex_digest();
    if (hex_digest!=_groupListMD5) {


      std::list<Glib::ustring> listStrings;
      for (std::vector<std::string>::const_iterator group_iter=group_names.begin();
	   group_iter!=group_names.end(); 
	   group_iter++) {
	listStrings.push_back(*group_iter);
      }
      
      _histogramGroupSelector.set_popdown_strings(listStrings);
      _histogramGroupSelector.set_value_in_list();

      _groupListMD5=hex_digest;
    }

    if (!selection_is_valid) {
      if (_histogramGroupSelector.get_popdown_strings().empty()) {
	_histogramGroupSelector.get_entry()->set_text("");
      }
      else {
	_histogramGroupSelector.get_entry()->set_text(*(_histogramGroupSelector.get_popdown_strings().begin()));
	// maybe the histogram group has changed.
	// setHistogramCollectionNames();
      }
    }
    else {
      setHistogramCollectionNames();
    }
  }
}

void HistogramDisplay::setHistogramCollectionNames()
{
  Glib::ustring current_group_selection=_histogramGroupSelector.get_entry()->get_text();

  if (!current_group_selection.empty()) {

    std::pair<std::string,HistMgr::ECollectionType> group_name_and_type=HistMgr::getGroupNameAndCollectionType(current_group_selection);
    if (group_name_and_type.second >= HistMgr::kNTypes) return;

    HistMgrPtr histograms(HistMgr::getInstance());
    if (!histograms) return;

    std::vector<std::string> histogram_collection_names;
    histograms->fillHistogramCollectionList(Key_t(group_name_and_type.first), group_name_and_type.second, histogram_collection_names);
    
    Glib::ustring current_hist_collection_selection=_histogramCollectionSelector.get_entry()->get_text();


    if (histogram_collection_names.empty()) {
      if (_histogramCollectionSelector.is_sensitive()) {
	_histogramCollectionSelector.set_sensitive(false);
      }
    }
    else {
      if (!_histogramCollectionSelector.is_sensitive()) {
	_histogramCollectionSelector.set_sensitive(true);
      }

      MD5 md5;
      bool selection_is_valid=false;
      for (std::vector<std::string>::const_iterator hist_iter=histogram_collection_names.begin();
	   hist_iter!=histogram_collection_names.end(); 
	   hist_iter++) {
	md5.update(reinterpret_cast<const unsigned char*>(hist_iter->c_str()), hist_iter->size() );
	if (!selection_is_valid && current_hist_collection_selection == Glib::ustring(*hist_iter)) {
	  selection_is_valid=true;
	}
      }
      md5.finalize();
      std::string hex_digest=md5.hex_digest();
      if (hex_digest!=_histListMD5) {

	std::list<Glib::ustring> listStrings;
	for (std::vector<std::string>::const_iterator hist_iter=histogram_collection_names.begin();
	     hist_iter!=histogram_collection_names.end(); 
	     hist_iter++) {
	  std::string::size_type pos=hist_iter->find("::");
	  if (pos!=std::string::npos) {
	    listStrings.push_back( std::string(*hist_iter,pos+2,hist_iter->size()-pos-2+1));
	  }
	  else {
	    listStrings.push_back( *hist_iter);
	  }
	}
      
	_histogramCollectionSelector.set_popdown_strings(listStrings);
	_histogramCollectionSelector.set_value_in_list();
	
	_histListMD5=hex_digest;
      }
      
      if (!selection_is_valid) {
	Key_t group_key(group_name_and_type.first);
	if (hasCurrent(group_key,group_name_and_type.second)) {
	  Key_t last_selection=currentHistogramCollection(group_key, group_name_and_type.second);
	  _histogramCollectionSelector.get_entry()->set_text(last_selection.name());
	}
	else if (!_histogramCollectionSelector.get_popdown_strings().empty()) {
	  setCurrentHistogramCollection(group_key, group_name_and_type.second, Key_t(*(_histogramCollectionSelector.get_popdown_strings().begin())));
	  _histogramCollectionSelector.get_entry()->set_text(*(_histogramCollectionSelector.get_popdown_strings().begin()));
	}
	else {
	  _histogramCollectionSelector.get_entry()->set_text("");

	}

      }
    }
  }
}

void HistogramDisplay::onHistogramGroupChanged()
{
  setHistogramCollectionNames();
}

void HistogramDisplay::onHistogramCollectionChanged()
{

  Glib::ustring current_group_selection=_histogramGroupSelector.get_entry()->get_text();
  if (current_group_selection.empty()) return;

  Glib::ustring current_hist_collection_selection=_histogramCollectionSelector.get_entry()->get_text();
  if (current_hist_collection_selection.empty()) return;

  std::pair<std::string,HistMgr::ECollectionType> group_name_and_type=HistMgr::getGroupNameAndCollectionType(current_group_selection);
  if (group_name_and_type.second >= HistMgr::kNTypes) return;

  if (!current_hist_collection_selection.empty()) {
    HistMgrPtr histograms(HistMgr::getInstance());
    if (!histograms) return;
    

    try {
      histmgr::Key_t group_key(group_name_and_type.first);
      histmgr::Key_t hist_key(_histogramCollectionSelector.get_entry()->get_text());

      setCurrentHistogramCollection(group_key,group_name_and_type.second,hist_key);

      bool is_2d=false;
      unsigned n_major=0;
      if (group_name_and_type.second != _lastCollectionType) {
	// when the histogramm ing mode is changed all the plots are deleted and rdone.
	_graphList.hide_all();
	while (!_graphList.get_children().empty()) {
	  _graphList.remove(**(_graphList.get_children().begin()));
	}
      }
      _lastCollectionType=group_name_and_type.second;
      switch (group_name_and_type.second) {
      case HistMgr::kH1: {
	const HistogramCollection_t &hist_col=histograms->getHistogramCollection(Key_t(group_key.name()), Key_t(hist_key.name()));
	if ((is_2d=hist_col.is2D() )) {
	  n_major=hist_col.nMajor();
	}

	break;
      }
      case HistMgr::kH2: {
	const Histogram2DCollection_t &hist_col=histograms->getHistogram2DCollection(Key_t(group_key.name()), Key_t(hist_key.name()));
	if ((is_2d=hist_col.is2D())) {
	    n_major=hist_col.nMajor();
	  }

	break;
      }
      case HistMgr::kGraph: {
	const GraphCollection_t &hist_col=histograms->getGraphCollection(Key_t(group_key.name()), Key_t(hist_key.name()));
	if (( is_2d=hist_col.is2D() )) {
	    n_major=hist_col.nMajor();
	}

	break;
      }
      case HistMgr::kProfile: {
	const ProfileCollection_t &hist_col=histograms->getProfileCollection(Key_t(group_key.name()), Key_t(hist_key.name()));
	if (( is_2d=hist_col.is2D() )) {
	  n_major=hist_col.nMajor();
	}
	break;
      }
      default:
	break;
      }

      if (is_2d) {
	_firstIndexSelector.set_sensitive(true);
	_firstIndexSelector.get_adjustment()->set_upper(n_major);

	HistPar_t &hist_par=getHistPar(group_key,group_name_and_type.second,hist_key);

	_rebinSelector.set_value(hist_par.rebin());
	_logY.set_active(hist_par.logY());
	if (hist_par.firstIndex() >= n_major && n_major>0) {
	  hist_par.setFirstIndex(n_major-1);
	}
	if ( hist_par.firstIndex() != static_cast<UInt_t>(_firstIndexSelector.get_value_as_int())) {
	  _firstIndexSelector.set_value(hist_par.firstIndex());
	}
	else {
	  updateHistogramDisplay();
	}
      }
      else {
	_firstIndexSelector.set_sensitive(false);
	updateHistogramDisplay();
      }
    }
    catch (std::logic_error &err) {
      // this should never happen but if ii happens it is not a desaster for the GUI 
      // so it is just ignored.
    }
  }
}

void HistogramDisplay::onFirstIndexChanged()
{
  updateHistogramDisplay();
}

void HistogramDisplay::onRebinChanged()
{
  Glib::ustring current_hist_collection_selection=_histogramCollectionSelector.get_entry()->get_text();

  if (!current_hist_collection_selection.empty()) {

    std::pair<std::string,HistMgr::ECollectionType> group_name_and_type=HistMgr::getGroupNameAndCollectionType(_histogramGroupSelector.get_entry()->get_text());
    if (group_name_and_type.second >= HistMgr::kNTypes) return;

    histmgr::Key_t group_key(group_name_and_type.first );
    histmgr::Key_t hist_key(current_hist_collection_selection);

    HistPar_t &hist_par=getHistPar(group_key,group_name_and_type.second, hist_key);

    hist_par.resetRange();
    updateHistogramDisplay();
  }
}

void HistogramDisplay::onLogYChanged()
{
  Glib::ustring current_hist_collection_selection=_histogramCollectionSelector.get_entry()->get_text();
  if (!current_hist_collection_selection.empty()) {
    HistMgrPtr histograms(HistMgr::getInstance());
    if (!histograms) return;
    
    try {
      std::pair<std::string,HistMgr::ECollectionType> group_name_and_type=HistMgr::getGroupNameAndCollectionType(_histogramGroupSelector.get_entry()->get_text());
      if (group_name_and_type.second >= HistMgr::kNTypes) return;
      
      histmgr::Key_t group_key(group_name_and_type.first );
      histmgr::Key_t hist_key(current_hist_collection_selection);
      
      HistPar_t &hist_par=getHistPar(group_key,group_name_and_type.second, hist_key);

      hist_par.setLogY(_logY.get_active());
      std::vector< Gtk::Widget * > graphs( _graphList.get_children());

      Double_t miny=(_logY.get_active() ? std::max(0.9,hist_par.min()) : hist_par.min());
      Double_t maxy=hist_par.max();

      for (std::vector<Gtk::Widget *>::iterator graph_iter=graphs.begin();
	   graph_iter!=graphs.end();
	   graph_iter++) {
	
	GraphDisplay *a_graph=dynamic_cast<GraphDisplay *>(*graph_iter);
	assert ( a_graph );

	if (miny<maxy) {
	  a_graph->scale(PlotMM::AXIS_LEFT)->set_range(miny,maxy,_logY.get_active());
	  a_graph->scale(PlotMM::AXIS_RIGHT)->set_range(miny,maxy,_logY.get_active());
	  a_graph->scale(PlotMM::AXIS_LEFT)->set_autoscale(false);
	  a_graph->scale(PlotMM::AXIS_RIGHT)->set_autoscale(false);
	}
	a_graph->replot();
      }
    }
    catch (std::logic_error &err) {
      // this should never happen but if ii happens it is not a desaster for the GUI 
      // so it is just ignored.
    }
  }
}

void HistogramDisplay::updateHistogramDisplay()
{
  Glib::ustring current_hist_collection_selection=_histogramCollectionSelector.get_entry()->get_text();

  Glib::ustring current_group_selection=_histogramGroupSelector.get_entry()->get_text();

  if (!current_hist_collection_selection.empty()) {

    std::pair<std::string,HistMgr::ECollectionType> group_name_and_type=HistMgr::getGroupNameAndCollectionType(_histogramGroupSelector.get_entry()->get_text());
    if (group_name_and_type.second >= HistMgr::kNTypes) return;
    
    histmgr::Key_t group_key(group_name_and_type.first );
    histmgr::Key_t hist_key(current_hist_collection_selection);
    setCurrentHistogramCollection(group_key,group_name_and_type.second,hist_key);
    switch (group_name_and_type.second) {
      //  HistPar_t &hist_par=getHistPar(group_key,group_name_and_type.second, hist_key);
    case HistMgr::kH1: {
      showHistogramCollection(group_key, group_name_and_type.second, hist_key);
      break;
    }
    case HistMgr::kH2: {
      showHistogram2DCollection(group_key, group_name_and_type.second, hist_key);
      break;
    }
    case HistMgr::kGraph: {
      break;
      }
    case HistMgr::kProfile: {
      break;
      }
    default:
      break;
    }

    
  }
}

void HistogramDisplay::showHistogramCollection(const histmgr::Key_t &group_key, HistMgr::ECollectionType type, const histmgr::Key_t &hist_key)
{
  if (type != HistMgr::kH1) return;
    HistMgrPtr histograms(HistMgr::getInstance());
    if (!histograms) return;

    try {
      // there are two keys: one to access the parameters for of the histogram display, and one to access the histogram
      const HistogramCollection_t &hist_col=histograms->getHistogramCollection(Key_t(group_key.name()),Key_t(hist_key.name()));

      HistPar_t &hist_par=getHistPar(group_key,type,hist_key);

      bool need_to_create_graphs=false;
      for (std::vector< Glib::RefPtr<PlotMM::Curve> >::const_iterator curve_iter=_curveList.begin();
	   curve_iter!=_curveList.end();
	   curve_iter++) {
	if (!dynamic_cast<PlotMM::Curve *>(((*curve_iter).operator->())) || dynamic_cast<PlotMM::Histogram2D *>(((*curve_iter).operator->()))) {
	  need_to_create_graphs=true;
	}
      }

      UInt_t a_rebin=static_cast<UInt_t>(_rebinSelector.get_value_as_int());
      hist_par.setRebin(a_rebin);

      if (hist_col.is2D()) {
	UInt_t current_index=static_cast<UInt_t>(_firstIndexSelector.get_value_as_int());
	if (current_index<hist_col.nMajor()) {
	  //	  Glib::ustring histogram_name=_histogramCollectionSelector.get_entry()->get_text();
	  if (!hist_key.name().empty()) {
	    hist_par.setFirstIndex(current_index);
	  }

	  UInt_t n_histograms=hist_col.nMinor(current_index);
	  if (need_to_create_graphs || n_histograms!=_graphList.get_children().size()) {
	    Float_t xmin=0;
	    Float_t xmax=0;
	    if (hist_col.n()>0) {
	      const FloatHistogram1D *a_hist=hist_col.histogram(0);
	      xmin=a_hist->xMin();
	      xmax=a_hist->xMax();
	    }
	    createHistogramGraphs(hist_key.name(),Glib::ustring(""),n_histograms,xmin,xmax,hist_par);
	  }

	  std::vector< Gtk::Widget * > graphs( _graphList.get_children());
	  assert ( graphs.size() == n_histograms );
	  
	  for (UInt_t hist_i=0; hist_i<n_histograms; hist_i++) {
	    std::stringstream full_title;
	    full_title << hist_key.name() << hist_col.getName(current_index) << " (index " << hist_i << ")";  
	    GraphDisplay *a_graph=dynamic_cast<GraphDisplay *>(graphs[hist_i]);
	    assert ( a_graph );
	    a_graph->title()->set_text(full_title.str());

	    const FloatHistogram1D *a_hist=hist_col.histogram(current_index, hist_i);
	    setGraph(graphs, hist_i, hist_key.name() , a_hist,hist_par);
	  }
	}
	else if (hist_col.nMajor()>0) {
	  _firstIndexSelector.set_value(hist_col.nMajor()-1);
	}
      }
      else {
	UInt_t n_histograms=hist_col.n();
	if (need_to_create_graphs || n_histograms!=_graphList.get_children().size()) {
	  Float_t xmin=0;
	  Float_t xmax=0;
	  if (n_histograms>0) {
	    const FloatHistogram1D *a_hist=hist_col.histogram(0);
	    xmin=a_hist->xMin();
	    xmax=a_hist->xMax();
	  }
	  createHistogramGraphs(hist_key.name(),"",n_histograms,xmin,xmax, hist_par);
	}
	std::vector< Gtk::Widget * > graphs( _graphList.get_children());
	assert ( graphs.size() == n_histograms );
	
	for (UInt_t hist_i=0; hist_i<n_histograms; hist_i++) {
	  std::stringstream full_title;
	  full_title << hist_key.name() << " " << hist_col.getName(hist_i) << " (index " << hist_i << ")";  
	  GraphDisplay *a_graph=dynamic_cast<GraphDisplay *>(graphs[hist_i]);
	  assert ( a_graph );
	  a_graph->title()->set_text(full_title.str());

	  const FloatHistogram1D *a_hist=hist_col.histogram(hist_i);
	  setGraph(graphs, hist_i, hist_key.name() , a_hist,hist_par);
	}

      }
      replotGraphs(hist_par);
    }
    catch (std::logic_error &err) {
      // this should never happen but if ii happens it is not a desaster for the GUI 
      // so it is just ignored.
    }
}

void HistogramDisplay::showHistogram2DCollection(const Key_t &group_key, HistMgr::ECollectionType type, const Key_t &hist_key)
{
  if (type != HistMgr::kH2) return;

    HistMgrPtr histograms(HistMgr::getInstance());
    if (!histograms) return;

    try {
      // there are two keys: one to access the parameters for of the histogram display, and one to access the histogram
      const Histogram2DCollection_t &hist_col=histograms->getHistogram2DCollection(Key_t(group_key.name()),Key_t(hist_key.name()));
      HistPar_t &hist_par=getHistPar(group_key,type,hist_key);
      UInt_t a_rebin=static_cast<UInt_t>(_rebinSelector.get_value_as_int());
      hist_par.setRebin(a_rebin);

      bool need_to_create_graphs=false;
      for (std::vector< Glib::RefPtr<PlotMM::Curve> >::const_iterator curve_iter=_curveList.begin();
	   curve_iter!=_curveList.end();
	   curve_iter++) {
	if (!dynamic_cast<PlotMM::Curve *>(((*curve_iter).operator->())) || !dynamic_cast<PlotMM::Histogram2D *>(((*curve_iter).operator->()))) {
	  need_to_create_graphs=true;
	}
      }

      if (hist_col.is2D()) {
	UInt_t current_index=static_cast<UInt_t>(_firstIndexSelector.get_value_as_int());
	if (current_index<hist_col.nMajor()) {
	  Glib::ustring histogram_name=_histogramCollectionSelector.get_entry()->get_text();
	  if (!histogram_name.empty()) {
	    hist_par.setFirstIndex(current_index);
	  }

	  UInt_t n_histograms=hist_col.nMinor(current_index);
	  if ( need_to_create_graphs || n_histograms!=_graphList.get_children().size()) {
	    Float_t xmin=0;
	    Float_t xmax=0;
	    if (hist_col.n()>0) {
	      const FloatHistogram2D *a_hist=hist_col.histogram(0);
	      xmin=a_hist->xMin();
	      xmax=a_hist->xMax();
	    }
	    createHistogramGraphs(hist_key.name(),Glib::ustring(""),n_histograms,xmin,xmax,hist_par);
	  }

	  std::vector< Gtk::Widget * > graphs( _graphList.get_children());
	  assert ( graphs.size() == n_histograms );
	  
	  for (UInt_t hist_i=0; hist_i<n_histograms; hist_i++) {
	    std::stringstream full_title;
	    full_title << hist_key.name() << hist_col.getName(current_index) << " (index " << hist_i << ")";  
	    GraphDisplay *a_graph=dynamic_cast<GraphDisplay *>(graphs[hist_i]);
	    assert ( a_graph );
	    a_graph->title()->set_text(full_title.str());

	    const FloatHistogram2D *a_hist=hist_col.histogram(current_index, hist_i);
	    setGraph(graphs, hist_i, hist_key.name() , a_hist,hist_par);
	  }
	}
	else if (hist_col.nMajor()>0) {
	  _firstIndexSelector.set_value(hist_col.nMajor()-1);
	}
      }
      else {
	UInt_t n_histograms=hist_col.n();
	if ( need_to_create_graphs || n_histograms!=_graphList.get_children().size()) {
	  Float_t xmin=0;
	  Float_t xmax=0;
	  if (n_histograms>0) {
	    const FloatHistogram2D *a_hist=hist_col.histogram(0);
	    xmin=a_hist->xMin();
	    xmax=a_hist->xMax();
	  }
	  createHistogramGraphs(hist_key.name(),"",n_histograms,xmin,xmax, hist_par);
	}
	std::vector< Gtk::Widget * > graphs( _graphList.get_children());
	assert ( graphs.size() == n_histograms );
	
	for (UInt_t hist_i=0; hist_i<n_histograms; hist_i++) {
	  std::stringstream full_title;
	  full_title << hist_key.name() << " " << hist_col.getName(hist_i) << " (index " << hist_i << ")";  
	  GraphDisplay *a_graph=dynamic_cast<GraphDisplay *>(graphs[hist_i]);
	  assert ( a_graph );
	  a_graph->title()->set_text(full_title.str());

	  const FloatHistogram2D *a_hist=hist_col.histogram(hist_i);
	  setGraph(graphs, hist_i, hist_key.name() , a_hist,hist_par);
	}

      }
      replotGraphs(hist_par);
    }
    catch (std::logic_error &err) {
      // this should never happen but if ii happens it is not a desaster for the GUI 
      // so it is just ignored.
    }
}


void HistogramDisplay::createHistogramGraphs(const Glib::ustring &title, const Glib::ustring &x_axis_title, 
					     UInt_t n_hist, const Float_t &x_min, const Float_t &x_max, 
					     const HistPar_t &hist_par)
{
  _graphList.hide_all();
  while (!_graphList.get_children().empty()) {
    _graphList.remove(**(_graphList.get_children().begin()));
  }
  _curveList.clear();
  _underflowCurveList.clear();
  _overflowCurveList.clear();

  Double_t miny=(hist_par.logY() ? std::max(0.9,hist_par.min()) : hist_par.min());
  Double_t maxy=hist_par.max();

  for (UInt_t hist_i=0; hist_i < n_hist; hist_i++) {
    std::stringstream full_title;
    full_title << title << " (index " << hist_i << ")";  
    
    GraphDisplay *a_graph=Gtk::manage(new GraphDisplay(Glib::ustring(full_title.str()),
						       x_axis_title,
						       Glib::ustring("entries"),
						       x_min,x_max,0.,0.));
    a_graph->set_size_request(-1,150);

    if (miny<maxy) {
      a_graph->scale(PlotMM::AXIS_LEFT)->set_range(miny,maxy,hist_par.logY());
      a_graph->scale(PlotMM::AXIS_RIGHT)->set_range(miny,maxy,hist_par.logY());
      a_graph->scale(PlotMM::AXIS_LEFT)->set_autoscale(false);
      a_graph->scale(PlotMM::AXIS_RIGHT)->set_autoscale(false);
    }
    
    _graphList.add(*a_graph);
  }

  _graphList.show_all();
}

void HistogramDisplay::setGraph(std::vector< Gtk::Widget * > &graphs, UInt_t hist_i, 
				const Glib::ustring &title, const FloatHistogram1D * const a_hist, 
				HistPar_t &hist_par)
{
#ifdef BOUNDARY_CHECK
  assert( a_hist);
  assert( graphs.size() > hist_i);
#endif

  GraphDisplay *a_graph=dynamic_cast<GraphDisplay *>(graphs[hist_i]);
  assert ( a_graph );

  if (hist_i >= _curveList.size()) {
    std::stringstream full_title;
    full_title << title;
    _curveList.resize(hist_i);
    _curveList.push_back(Glib::RefPtr<PlotMM::Curve>(new PlotMM::Curve(full_title.str())));
    _curveList.back()->set_curve_style(PlotMM::CURVE_LINES);
    _curveList.back()->paint()->set_pen_color(Gdk::Color("black"));
    _curveList.back()->symbol()->set_style(PlotMM::SYMBOL_NONE);
    a_graph->add_curve(_curveList.back());
  }
  //  else if (!dynamic_cast<PlotMM::Curve *>(((_curveList[hist_i]).operator->())) || dynamic_cast<PlotMM::Histogram2D *>(((_curveList[hist_i]).operator->()))) {
    //    _curveList[hist_i]=Glib::RefPtr<PlotMM::Curve>(new PlotMM::Curve(title));
    //  }

  if (hist_i >= _underflowCurveList.size()) {
    std::stringstream full_title;
    full_title << "underflow_" << title;
    _underflowCurveList.resize(hist_i);
    _underflowCurveList.push_back(Glib::RefPtr<PlotMM::Curve>(new PlotMM::Curve(full_title.str())));
    _underflowCurveList.back()->set_curve_style(PlotMM::CURVE_LINES);
    Gdk::Color red("red");
    _underflowCurveList.back()->paint()->set_pen_color(red);
    _underflowCurveList.back()->symbol()->set_style(PlotMM::SYMBOL_ELLIPSE);
    _underflowCurveList.back()->symbol()->set_size(10);
    _underflowCurveList.back()->symbol()->paint()->set_pen_color(red);
    _underflowCurveList.back()->symbol()->paint()->set_brush_color(red);
    a_graph->add_curve(_underflowCurveList.back());
  }

  if (hist_i >= _overflowCurveList.size()) {
    std::stringstream full_title;
    full_title << "overflow_" << title;
    _overflowCurveList.resize(hist_i);
    _overflowCurveList.push_back(Glib::RefPtr<PlotMM::Curve>(new PlotMM::Curve(full_title.str())));
    _overflowCurveList.back()->set_curve_style(PlotMM::CURVE_LINES);
    Gdk::Color red("red");
    _overflowCurveList.back()->paint()->set_pen_color(red);
    _overflowCurveList.back()->symbol()->set_style(PlotMM::SYMBOL_ELLIPSE);
    _overflowCurveList.back()->symbol()->set_size(10);
    _overflowCurveList.back()->symbol()->paint()->set_pen_color(red);
    _overflowCurveList.back()->symbol()->paint()->set_brush_color(red);
    a_graph->add_curve(_overflowCurveList.back());
  }

  std::vector<Double_t> x_arr;
  std::vector<Double_t> y_arr;
  
  UInt_t combine_n_bins=hist_par.rebin();
  if (combine_n_bins>1) {
    UInt_t n_bins=(a_hist->nBins()+combine_n_bins-1)/combine_n_bins;
    if (n_bins>200) {
      for (UInt_t main_bin_i=a_hist->firstBinIndex(); main_bin_i<=a_hist->lastBinIndex(); main_bin_i+=combine_n_bins) {
	Double_t sum_y=0;
	Double_t sum_x=0;
	for (UInt_t bin_i=main_bin_i; bin_i < main_bin_i+combine_n_bins; bin_i++) {
	sum_x+=a_hist->binCenter(bin_i);
	sum_y+=a_hist->binContent(bin_i);
	}
	x_arr.push_back(sum_x/combine_n_bins);
	y_arr.push_back(sum_y);
	hist_par.setValue(sum_y);
      }
    }
    else {
      for (UInt_t main_bin_i=a_hist->firstBinIndex(); main_bin_i<=a_hist->lastBinIndex(); main_bin_i+=combine_n_bins) {
	Double_t sum_y=0;
	Double_t sum_bin_width=0;
	for (UInt_t bin_i=main_bin_i; bin_i < main_bin_i+combine_n_bins; bin_i++) {
	  sum_y+=a_hist->binContent(bin_i);
	  sum_bin_width+=a_hist->binWidth(bin_i);
	}
	x_arr.push_back(a_hist->binLowEdge(main_bin_i));
	x_arr.push_back(x_arr.back()+sum_bin_width);
	y_arr.push_back(sum_y);
	y_arr.push_back(sum_y);
	hist_par.setValue(sum_y);
      }
    }
  }
  else {
    if (a_hist->nBins()>200) {
      for (UInt_t bin_i=a_hist->firstBinIndex(); bin_i<=a_hist->lastBinIndex(); bin_i++) {
	x_arr.push_back(a_hist->binCenter(bin_i));
	y_arr.push_back(a_hist->binContent(bin_i));
	hist_par.setValue(y_arr.back());
      }
    }
    else {
      for (UInt_t bin_i=a_hist->firstBinIndex(); bin_i<=a_hist->lastBinIndex(); bin_i++) {
	x_arr.push_back(a_hist->binLowEdge(bin_i));
	x_arr.push_back(x_arr.back()+a_hist->binWidth(bin_i));
	y_arr.push_back(a_hist->binContent(bin_i));
	y_arr.push_back(y_arr.back());
	hist_par.setValue(y_arr.back());
      }
    }
  }
  _curveList[hist_i]->set_data(x_arr,y_arr);
  
  Double_t x_val[2];
  Double_t y_val[2];
  x_val[0]=a_hist->binCenter(a_hist->firstBinIndex());
  x_val[1]=a_hist->binCenter(a_hist->firstBinIndex());
  y_val[0]=0.;
  y_val[1]=a_hist->underflow();
  _underflowCurveList[hist_i]->set_data(x_val,y_val,2);
  hist_par.setValue(y_val[1]);

  x_val[0]=a_hist->binCenter(a_hist->lastBinIndex());
  x_val[1]=a_hist->binCenter(a_hist->lastBinIndex());
  y_val[0]=0.;
  y_val[1]=a_hist->overflow();
  _overflowCurveList[hist_i]->set_data(x_val,y_val,2);
  hist_par.setValue(y_val[1]);
}

void HistogramDisplay::setGraph(std::vector< Gtk::Widget * > &graphs, UInt_t hist_i, 
				const Glib::ustring &title, const FloatHistogram2D * const a_hist, 
				HistPar_t &hist_par)
{
#ifdef BOUNDARY_CHECK
  assert( a_hist);
  assert( graphs.size() > hist_i);
#endif

  GraphDisplay *a_graph=dynamic_cast<GraphDisplay *>(graphs[hist_i]);
  assert ( a_graph );
	
  if (hist_i >= _curveList.size()) {
    _curveList.resize(hist_i);
    _curveList.push_back(Glib::RefPtr<PlotMM::Curve>(new PlotMM::Histogram2D(title)));
    _curveList.back()->set_curve_style(PlotMM::CURVE_LINES);
    _curveList.back()->paint()->set_pen_color(Gdk::Color("black"));
    _curveList.back()->symbol()->set_style(PlotMM::SYMBOL_NONE);
    a_graph->add_curve(_curveList.back());
  }
  //else {
  //  _curveList[hist_i]=Glib::RefPtr<PlotMM::Curve>(new PlotMM::Histogram2D(title));
  //}

  std::vector<Double_t> x_arr;
  std::vector<Double_t> y_arr;
  
  UInt_t combine_n_bins=hist_par.rebin();
  if (combine_n_bins>1) {
    std::vector<Float_t> temp_arr;
    UInt_t y_bin_offset=(a_hist->xNBins()/combine_n_bins);
    if (y_bin_offset<=1) {
      y_bin_offset++;
    }
    y_bin_offset+=2; //overflow

    unsigned int n_bins_y=a_hist->yNBins()/combine_n_bins;
    if (n_bins_y<=1) {
      n_bins_y++;
    }
    n_bins_y+=2; //overflow

    temp_arr.resize( y_bin_offset * n_bins_y );

    unsigned int dest_bin_y_i = y_bin_offset;
    for (UInt_t y_i=a_hist->yFirstBinIndex(); y_i<=a_hist->yLastBinIndex();y_i+=combine_n_bins) {
      unsigned int dest_bin_i=dest_bin_y_i + 1; // overflow
      for (UInt_t x_i=a_hist->xFirstBinIndex(); x_i<=a_hist->xLastBinIndex();x_i+=combine_n_bins) {
	
	unsigned int last_bin_y_i = ( y_i + combine_n_bins < a_hist->yLastBinIndex() ? y_i + combine_n_bins : a_hist->yLastBinIndex());
	unsigned int last_bin_x_i = ( x_i + combine_n_bins < a_hist->xLastBinIndex() ? x_i + combine_n_bins : a_hist->xLastBinIndex());
 
	double bin_content=0;
	for (UInt_t y_ii=y_i; y_ii<last_bin_y_i; y_ii++) {
	  for (UInt_t x_ii=y_i; x_ii<last_bin_x_i; x_ii++) {
	    bin_content += a_hist->binContent(a_hist->binIndex(x_ii,y_ii));
	  }
	}
#ifdef BOUNDARY_CHECK
	assert(dest_bin_i<temp_arr.size());;
#endif
	temp_arr[dest_bin_i]=bin_content;
	dest_bin_i++;
      }
      dest_bin_y_i += y_bin_offset;
    }
#ifdef BOUNDARY_CHECK
    assert ( dynamic_cast<PlotMM::Histogram2D *>(_curveList[hist_i].operator->()) != NULL );
#endif
    dynamic_cast<PlotMM::Histogram2D *>(_curveList[hist_i].operator->())->set_data(PlotMM::Binning(y_bin_offset-2,a_hist->xMin(),a_hist->xMax()),
				 PlotMM::Binning(n_bins_y-2,a_hist->yMin(),a_hist->yMax()), &temp_arr[0] );
  }
  else {
    std::vector<Float_t> temp_arr;
    UInt_t y_bin_offset=a_hist->xNBins()+2;

    temp_arr.resize( y_bin_offset * (a_hist->yNBins() + 2) );

    unsigned int dest_bin_y_i=y_bin_offset;
    for (UInt_t y_i=a_hist->yFirstBinIndex(); y_i<=a_hist->yLastBinIndex();y_i++) {
      unsigned int dest_bin_i=dest_bin_y_i + 1; // overflow
      for (UInt_t x_i=a_hist->xFirstBinIndex(); x_i<=a_hist->xLastBinIndex();x_i++) {
#ifdef BOUNDARY_CHECK
	assert(dest_bin_i<temp_arr.size());;
#endif
	temp_arr[dest_bin_i]=a_hist->binContent(a_hist->binIndex(x_i,y_i));
	dest_bin_i++;
      }
      dest_bin_y_i += y_bin_offset;
    }
#ifdef BOUNDARY_CHECK
    assert ( dynamic_cast<PlotMM::Histogram2D *>(_curveList[hist_i].operator->()) != NULL );
#endif
    dynamic_cast<PlotMM::Histogram2D *>(_curveList[hist_i].operator->())->set_data(PlotMM::Binning(a_hist->xNBins(),a_hist->xMin(),a_hist->xMax()),
				 PlotMM::Binning(a_hist->yNBins(),a_hist->yMin(),a_hist->yMax()), &temp_arr[0] );
  }
}

void HistogramDisplay::replotGraphs(const HistPar_t &hist_par)
{
  std::vector< Gtk::Widget * > graphs( _graphList.get_children());
  
  Double_t miny=(_logY.get_active() ? std::max(0.9,hist_par.min()) : hist_par.min());
  Double_t maxy=hist_par.max();
  
  for (std::vector<Gtk::Widget *>::iterator graph_iter=graphs.begin();
       graph_iter!=graphs.end();
       graph_iter++) {
    
    GraphDisplay *a_graph=dynamic_cast<GraphDisplay *>(*graph_iter);
    assert ( a_graph );
    
    if (miny<maxy) {
      a_graph->scale(PlotMM::AXIS_LEFT)->set_range(miny,maxy,_logY.get_active());
      a_graph->scale(PlotMM::AXIS_RIGHT)->set_range(miny,maxy,_logY.get_active());
      a_graph->scale(PlotMM::AXIS_LEFT)->set_autoscale(false);
      a_graph->scale(PlotMM::AXIS_RIGHT)->set_autoscale(false);
    }
    a_graph->replot();
  }
}

void HistogramDisplay::onEventChanged()
{
  setGroupNames();
  updateHistogramDisplay();
}


}

#endif
