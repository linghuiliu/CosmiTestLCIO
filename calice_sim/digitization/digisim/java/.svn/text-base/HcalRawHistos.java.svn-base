//
// File: HcalRawHistos.java
//
// Purpose: 
//
// Usage:
//   This class can be run either standalone (java HcalRawHistos data.slcio)
// or within a "framework", like (java AnalyzeLcioNew data.slcio), where
// AnalyzeLcioNew instantiates and uses class HcalRawHistos.
//
// 20061106 - G.Lima - Created

import hep.aida.*;
import hep.lcio.event.*;
import hep.lcio.event.RawCalorimeterHit;
import hep.lcio.implementation.io.LCFactory;
import hep.lcio.io.*;
import java.io.IOException;

public class HcalRawHistos implements LCEventListener {

  public void processEvent(LCEvent event) {

    //=== HCAL processing

    // sim hits
    LCCollection collection = null;
    try {
//         collection = event.getCollection("TBhcal06_hcalSD");
        collection = event.getCollection("HcalBarrHits");
    }
    catch(Exception x) {
	System.out.println("Unavailable collection: TBhcal06_hcalSD");
    }
    int nhdsim = 0;
    if(collection!=null) nhdsim = collection.getNumberOfElements();
    _hHDsimNhits.fill((float)nhdsim);
    for(int i = 0; i<nhdsim; ++i) {
      SimCalorimeterHit ihit = (SimCalorimeterHit)collection.getElementAt(i);

      // Fill histograms here
      float ene = ihit.getEnergy();
      float time = ihit.getTimeCont(0);
      _hHDsimEnergy.fill( ene );
      _hHDsimTime.fill( time );
      _hHDsimEnergyLog.fill( Math.log(ene)*lntolog10 );
      _hHDsimTimeLog.fill( Math.log(time)*lntolog10 );
    }

    // raw hits
    collection = null;
    try {
        collection = event.getCollection("HcalRawCollection");
    }
    catch(Exception x) {
	System.out.println("Unavailable collection: HcalRawCollection");
    }
    int nhdraw = 0;
    if(collection!=null) nhdraw = collection.getNumberOfElements();
    _hHDrawNhits.fill((float)nhdraw);
    for(int i = 0; i<nhdraw; ++i) {
      RawCalorimeterHit ihit = (RawCalorimeterHit) collection.getElementAt(i);

      // Fill histograms here
      float ene = ihit.getAmplitude();
      float time = ihit.getTimeStamp();
      _hHDrawEnergy.fill( ene );
      _hHDrawTime.fill( time );
      if(ene>0) _hHDrawEnergyLog.fill( Math.log(ene)*lntolog10 );
      _hHDrawTimeLog.fill( Math.log(time)*lntolog10 );
    }

    // calib hits
    _nhits = 0;
    _esum = 0;
    collection = null;
    try {
	collection = event.getCollection("HcalCalibHits");
    }
    catch(Exception x) {
	System.out.println("Unavailable collection: HcalCalibHits");
    }
    int nhdcalib = 0;
    if(collection!=null) nhdcalib = collection.getNumberOfElements();
    _hHDclbNhits.fill((float)nhdcalib);
    for(int i = 0; i<nhdcalib; ++i) {
      CalorimeterHit ihit = (CalorimeterHit) collection.getElementAt(i);

      // Fill histograms here
      float ene = ihit.getEnergy();
      float time = ihit.getTime();
      _hHDclbEnergy.fill( ene );
      _hHDclbTime.fill( time );
      _hHDclbEnergyLog.fill( Math.log(ene)*lntolog10 );
      _hHDclbTimeLog.fill( Math.log(time)*lntolog10 );

      if(ene>0.5) {
	  ++ _nhits;
	  _esum += ene;
      }
    }

    _nEvents++;
  }

  public void endOfJob() {
  }

  public void modifyEvent(LCEvent lCEvent) {
    // No thanks
  }

  // Constructor called by the main routine (standalone mode)
  // A new tree is created, to be stored in HcalRawHistos.aida file
  public HcalRawHistos() throws IOException {
    IAnalysisFactory af = IAnalysisFactory.create();
    _tree = af.createTreeFactory().create("HcalRawHistos.aida","xml",false,true);
    IHistogramFactory hf = af.createHistogramFactory(_tree);
    bookHistos(hf);
  }

  // This constructor to be used from a "framework".  An .aida tree
  // should be provided, then histograms will be booked inside a folder
  // called "HcalRawHistos".
  public HcalRawHistos(IAnalysisFactory af, ITree tree) throws IOException {
    // save tree pointer for reuse in event loop
    _tree = tree;
    // create a new folder within the existing tree
    _ownFolder = true;
    _tree.mkdir("Hcal");
    _tree.cd("Hcal");

    // book histograms
    IHistogramFactory hf = af.createHistogramFactory(_tree);
    bookHistos(hf);

    _tree.cd("..");
  }

  // Histograms are to be created here
  public void bookHistos(IHistogramFactory hf) {
    // Create histograms here
    _hHDsimNhits = hf.createCloud1D("HDsim: Nhits");
    _hHDrawNhits = hf.createCloud1D("HDraw: Nhits");
    _hHDclbNhits = hf.createCloud1D("HDclb: Nhits");

    _hHDsimEnergy = hf.createHistogram1D("HDsim: Energy", 500, 0, 0.3);
    _hHDrawEnergy = hf.createHistogram1D("HDraw: Energy", 50, 0, 3000);
    _hHDclbEnergy = hf.createHistogram1D("HDclb: Energy", 60, 0, 3);

    _hHDsimEnergyLog = hf.createCloud1D("HDsim: Log Energy");
    _hHDrawEnergyLog = hf.createCloud1D("HDraw: Log Energy");
    _hHDclbEnergyLog = hf.createCloud1D("HDclb: Log Energy");

    _hHDsimTime = hf.createCloud1D("HDsim: Time stamps");
    _hHDrawTime = hf.createCloud1D("HDraw: Time stamps");
    _hHDclbTime = hf.createCloud1D("HDclb: Time stamps");

    _hHDsimTimeLog = hf.createCloud1D("HDsim: Log time stamps");
    _hHDrawTimeLog = hf.createCloud1D("HDraw: Log time stamps");
    _hHDclbTimeLog = hf.createCloud1D("HDclb: Log time stamps");

  }

    public int getNhits() {
	return _nhits;
    }

    public double getEnergySum() {
	return _esum;
    }


    // ***** Member data  *****
    private int _nhits;
    private double _esum;

    private ICloud1D _hHDsimNhits;
    private IHistogram1D _hHDsimEnergy;
    private ICloud1D _hHDsimTime;
    private ICloud1D _hHDsimEnergyLog;
    private ICloud1D _hHDsimTimeLog;

    private ICloud1D _hHDrawNhits;
    private IHistogram1D _hHDrawEnergy;
    private ICloud1D _hHDrawTime;
    private ICloud1D _hHDrawEnergyLog;
    private ICloud1D _hHDrawTimeLog;

    private ICloud1D _hHDclbNhits;
    private IHistogram1D _hHDclbEnergy;
    private ICloud1D _hHDclbTime;
    private ICloud1D _hHDclbEnergyLog;
    private ICloud1D _hHDclbTimeLog;

//     private ITuple _tuple;

    private boolean _ownFolder = false;
    private ITree _tree;
    private int _nEvents;
    private double lntolog10 = 1/Math.log(10);

  public static void main(String[] args) throws Exception {
    HcalRawHistos analysis = new HcalRawHistos();
    ILCFactory factory = LCFactory.getInstance();
    LCReader reader = factory.createLCReader();
    reader.open(args[0]);
    reader.registerLCEventListener(analysis);
    reader.readStream();
    reader.close();
    System.out.println("Analyzed "+analysis._nEvents+" events");
    analysis._tree.commit();
  }
}
