
#include "HistogramOutput.hh"
#include "HistMgr.hh"


#ifdef HAVE_PLOTMM
#  include <GuiThread.hh>
#  include "HistogramDisplay.hh"
#endif

namespace histmgr {
  // make Processor known to marlin
#ifdef HAVE_ROOT
  HistogramOutput __histogramOutput;
#endif

  HistogramOutput::HistogramOutput()
   :  Processor("HistogramOutput")
#ifdef HAVE_PLOTMM
      ,_display(0)
#endif

  {
    StringVec fileFolderAssignment ;
    fileFolderAssignment.push_back("groupName");
    fileFolderAssignment.push_back("folderName");
    fileFolderAssignment.push_back("fileName.root");
    
    registerOptionalParameter( "HistogramFileName" , 
			       "assign file and folder names to histogram groups" ,
			       _fileFolderAssignment ,
			       fileFolderAssignment ,
			       fileFolderAssignment.size() ) ;

#ifdef HAVE_PLOTMM
    _visualise=0;
    registerProcessorParameter( "Visualisation" , 
				"Visualise the histograms." ,
				_visualise ,
				_visualise);
#endif

  }

  HistogramOutput::~HistogramOutput()
  {
#ifdef HAVE_PLOTMM
    if (_display) {
      GLVIEW::GuiThread *gui=GLVIEW::GuiThread::getInstance();
      Display *display_ptr=static_cast<Display *>(_display);
      gui->removeDisplay(&display_ptr);
    }
#endif
  }


  void HistogramOutput::init()
  {
    printParameters();

    HistMgr *histograms=HistMgr::getInstance();
    UInt_t index=0;

    if ( _fileFolderAssignment.size()%3 != 0) {
      throw std::runtime_error("HistogramOutput::init> The parameter HistogramFileName must contain for each histogram group exactly 3 values: \
the histogram group name, the assigned output file name, the output folder name or \"\".");
    }


    while( index < _fileFolderAssignment.size() ){

      // histogram group name
      // file name
      // folder name inside the root file;
      if ( _fileFolderAssignment[index].empty() || _fileFolderAssignment[index+1].empty() ) {
	throw std::runtime_error("HistogramOutput::init> The histogram group name and the assigned file name specified by the parameter HistogramFileName must not be empty.");      }

      std::string folder_name(_fileFolderAssignment[index+2]);
      if (folder_name.size()>0 && folder_name[0]=='\"' && folder_name[folder_name.size()-1]) {
	folder_name.erase(0,1);
	folder_name.erase(folder_name.size()-1);
      }
	  
      std::cout << "HistogramOutput::init> assign group " << _fileFolderAssignment[index] << " to file " << _fileFolderAssignment[index+1] 
	   << " and write into the folder" << folder_name << std::endl;

      histograms->assignFileName(_fileFolderAssignment[index],_fileFolderAssignment[index+1],folder_name);
      index+=3;
    }

    histograms->warnOnUnassigned();
    histograms->listRegisteredGroups();

#ifdef HAVE_PLOTMM
    // these two objects must stay alive until the GUI thread has created the display
    Display *display_ptr=0;
    HistogramDisplayKit display_kit;
    GLVIEW::GuiThread *gui=0;
    if (_visualise>0) {
      gui=GLVIEW::GuiThread::getInstance();
      gui->registerDisplay(&display_ptr,&display_kit,"Histograms");
      // until the display is created we can do some other stuff.

      gui->waitForDisplay(&display_ptr);
      _display=dynamic_cast<HistogramDisplay *>(display_ptr);
      if (!_display) {
	throw std::runtime_error("HistogramOutput::Init> ERROR: Histogram display creation failed!");
      }
    }
    else {
      _display=0;
    }
#endif

  }

  void HistogramOutput::end()
  {

#ifdef HAVE_CALICEGUI
    if (_display) {
      // wait for the gui to be stopped
      GLVIEW::GuiThread *gui=GLVIEW::GuiThread::getInstance();
      Display *ptr=static_cast<Display *>(_display);
      gui->removeDisplay(&ptr);
    }
#endif

    HistMgr *histograms=HistMgr::getInstance();
    std::cout << " --- " << name() << " Report :" << std::endl;
    histograms->listRegisteredGroups();
#ifdef HAVE_ROOT
    histograms->writeHistograms();
#endif
    HistMgr::deleteInstance();
    std::cout << std::endl;
  }
}
