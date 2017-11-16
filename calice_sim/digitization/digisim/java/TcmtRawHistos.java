//
// File: TcmtRawHistos.java
//
// Purpose: 
//
// Usage:
//   This class can be run either standalone (java TcmtRawHistos data.slcio)
// or within a "framework", like (java AnalyzeLcioNew data.slcio), where
// AnalyzeLcioNew instantiates and uses class TcmtRawHistos.
//
// 20061106 - G.Lima - Created

import hep.aida.*;
import hep.lcio.event.*;
import hep.lcio.event.RawCalorimeterHit;
import hep.lcio.implementation.io.LCFactory;
import hep.lcio.io.*;
import java.io.IOException;

public class TcmtRawHistos implements LCEventListener {

  public void processEvent(LCEvent event) {

    //=== TCMT processing

    // sim hits
    LCCollection collection = null;
    try {
        collection = event.getCollection("TBcatcher06_catcherSD");
    }
    catch(Exception x) {
// 	System.out.println("Unavailable collection: TBcatcher06_catcherSD");
    }
    int ntcsim = 0;
    if(collection!=null) ntcsim = collection.getNumberOfElements();
    _hTCsimNhits.fill((float)ntcsim);
    for(int i = 0; i<ntcsim; ++i) {
      SimCalorimeterHit ihit = (SimCalorimeterHit)collection.getElementAt(i);

      // Fill histograms here
      float ene = ihit.getEnergy();
      float time = ihit.getTimeCont(0);
      _hTCsimEnergy.fill( ene );
      _hTCsimTime.fill( time );
      _hTCsimEnergyLog.fill( Math.log(ene)*lntolog10 );
      _hTCsimTimeLog.fill( Math.log(time)*lntolog10 );
    }

    // raw hits
    collection = null;
    try {
        collection = event.getCollection("TcmtRawCollection");
    }
    catch(Exception x) {
	System.out.println("Unavailable collection: TcmtRawCollection");
    }
    int ntcraw = 0;
    if(collection!=null) ntcraw = collection.getNumberOfElements();
    _hTCrawNhits.fill((float)ntcraw);
    for(int i = 0; i<ntcraw; ++i) {
      RawCalorimeterHit ihit = (RawCalorimeterHit) collection.getElementAt(i);

      // Fill histograms here
      float ene = ihit.getAmplitude();
      float time = ihit.getTimeStamp();
      _hTCrawEnergy.fill( ene );
      _hTCrawTime.fill( time );
      _hTCrawEnergyLog.fill( Math.log(ene)*lntolog10 );
      _hTCrawTimeLog.fill( Math.log(time)*lntolog10 );
    }

    // calib hits
    _esum = 0;
    _nhits = 0;
    int[] hitsLayer = new int[17];
    double[] energyLayer = new double[17];

    collection = null;
    try {
	collection = event.getCollection("TcmtCalibHits");
    }
    catch(Exception x) {
	System.out.println("Unavailable collection: TcmtCalibHits");
    }
    int ntccalib = 0;
    if(collection!=null) ntccalib = collection.getNumberOfElements();
    _hTCclbNhits.fill((float)ntccalib);
    for(int i = 0; i<ntccalib; ++i) {
      CalorimeterHit ihit = (CalorimeterHit) collection.getElementAt(i);
      int cellid = ihit.getCellID0();

      // fill histograms here
      float ene = ihit.getEnergy();
      float time = ihit.getTime();
      _hTCclbEnergy.fill( ene );
      _hTCclbTime.fill( time );
      _hTCclbEnergyLog.fill( Math.log(ene)*lntolog10 );
      _hTCclbTimeLog.fill( Math.log(time)*lntolog10 );

      // hit selection
      if(ene>0.5) {
	  ++ _nhits;
	  _esum += ene;

	  int layer = cellid & 0xff;
	  assert(layer<=16);
 	  hitsLayer[layer] += 1;
 	  energyLayer[layer] += ene;
      }
    }

     for(int i=0; i<=16; ++i) {
 	_hTChitsPerLayer.fill( i, hitsLayer[i] );
 	_hTCenergyPerLayer.fill( i, energyLayer[i] );
     }
    _nEvents++;
  }

  public void endOfJob() {
  }

  public void modifyEvent(LCEvent lCEvent) {
    // No thanks
  }

  // Constructor called by the main routine (standalone mode)
  // A new tree is created, to be stored in TcmtRawHistos.aida file
  public TcmtRawHistos() throws IOException {
    IAnalysisFactory af = IAnalysisFactory.create();
    _tree = af.createTreeFactory().create("TcmtRawHistos.aida","xml",false,true);
    IHistogramFactory hf = af.createHistogramFactory(_tree);
    bookHistos(hf);
  }

  // This constructor to be used from a "framework".  An .aida tree
  // should be provided, then histograms will be booked inside a folder
  // called "TcmtRawHistos".
  public TcmtRawHistos(IAnalysisFactory af, ITree tree) throws IOException {
    // save tree pointer for reuse in event loop
    _tree = tree;
    // create a new folder within the existing tree
    _ownFolder = true;
    _tree.mkdir("Tcmt");
    _tree.cd("Tcmt");

    // book histograms
    IHistogramFactory hf = af.createHistogramFactory(_tree);
    bookHistos(hf);

    _tree.cd("..");
  }

  // Histograms are to be created here
  public void bookHistos(IHistogramFactory hf) {
    // Create histograms here
    _hTCsimNhits = hf.createCloud1D("TCsim: Nhits");
    _hTCrawNhits = hf.createCloud1D("TCraw: Nhits");
    _hTCclbNhits = hf.createCloud1D("TCclb: Nhits");

    _hTCsimEnergy = hf.createHistogram1D("TCsim: Energy", 50, 0, 0.002);
    _hTCrawEnergy = hf.createHistogram1D("TCraw: Energy", 50, 0, 3000);
    _hTCclbEnergy = hf.createHistogram1D("TCclb: Energy", 60, 0, 3);

    _hTCsimEnergyLog = hf.createCloud1D("TCsim: Log Energy");
    _hTCrawEnergyLog = hf.createCloud1D("TCraw: Log Energy");
    _hTCclbEnergyLog = hf.createCloud1D("TCclb: Log Energy");

    _hTCsimTime = hf.createCloud1D("TCsim: Time stamps");
    _hTCrawTime = hf.createCloud1D("TCraw: Time stamps");
    _hTCclbTime = hf.createCloud1D("TCclb: Time stamps");

    _hTCsimTimeLog = hf.createCloud1D("TCsim: Log time stamps");
    _hTCrawTimeLog = hf.createCloud1D("TCraw: Log time stamps");
    _hTCclbTimeLog = hf.createCloud1D("TCclb: Log time stamps");

    _hTCenergyPerLayer = hf.createHistogram2D("TCclb: energy per layer",16,0.5,16.5,100,0,100);
    _hTChitsPerLayer = hf.createHistogram2D("TCclb: hits per layer",16,0.5,16.5,50,0,50);
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

    private ICloud1D _hTCsimNhits;
    private IHistogram1D _hTCsimEnergy;
    private ICloud1D _hTCsimTime;
    private ICloud1D _hTCsimEnergyLog;
    private ICloud1D _hTCsimTimeLog;

    private ICloud1D _hTCrawNhits;
    private IHistogram1D _hTCrawEnergy;
    private ICloud1D _hTCrawTime;
    private ICloud1D _hTCrawEnergyLog;
    private ICloud1D _hTCrawTimeLog;

    private ICloud1D _hTCclbNhits;
    private IHistogram1D _hTCclbEnergy;
    private ICloud1D _hTCclbTime;
    private ICloud1D _hTCclbEnergyLog;
    private ICloud1D _hTCclbTimeLog;

    private IHistogram2D _hTCenergyPerLayer;
    private IHistogram2D _hTChitsPerLayer;
//     private IProfile1D _hTCenergyPerLayer;
//     private IProfile1D _hTChitsPerLayer;

//     private ITuple _tuple;

    private boolean _ownFolder = false;
    private ITree _tree;
    private int _nEvents;
    private double lntolog10 = 1/Math.log(10);

  public static void main(String[] args) throws Exception {
    TcmtRawHistos analysis = new TcmtRawHistos();
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
