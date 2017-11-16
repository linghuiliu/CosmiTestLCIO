//
// File: EcalRawHistos.java
//
// Purpose: 
//
// Usage:
//   This class can be run either standalone (java EcalRawHistos data.slcio)
// or within a "framework", like (java AnalyzeLcioNew data.slcio), where
// AnalyzeLcioNew instantiates and uses class EcalRawHistos.
//
// 20061106 - G.Lima - Created

import hep.aida.*;
import hep.lcio.event.*;
import hep.lcio.event.RawCalorimeterHit;
import hep.lcio.implementation.io.LCFactory;
import hep.lcio.io.*;
import java.io.IOException;

public class EcalRawHistos implements LCEventListener {

  public void processEvent(LCEvent event) {

    //=== ECAL processing

    // sim hits
    LCCollection collection = null;
    try {
//         collection = event.getCollection("ProtoDesy0506_ProtoSD03");
        collection = event.getCollection("EcalBarrHits");
    }
    catch(Exception x) {
 	System.out.println("Unavailable collection: ProtoDesy0506_ProtoSD03");
    }
    int ntcsim = 0;
    if(collection!=null) ntcsim = collection.getNumberOfElements();
    _hEMsimNhits.fill((float)ntcsim);
    for(int i = 0; i<ntcsim; ++i) {
      SimCalorimeterHit ihit = (SimCalorimeterHit)collection.getElementAt(i);

      // Fill histograms here
      float ene = ihit.getEnergy();
      float time = ihit.getTimeCont(0);
      _hEMsimEnergy.fill( ene );
      _hEMsimTime.fill( time );
      if(ene>0) _hEMsimEnergyLog.fill( Math.log(ene)*lntolog10 );
      _hEMsimTimeLog.fill( Math.log(time)*lntolog10 );
    }

    // raw hits
    collection = null;
    try {
        collection = event.getCollection("EcalRawCollection");
    }
    catch(Exception x) {
	System.out.println("Unavailable collection: EcalRawCollection");
    }
    int ntcraw = 0;
    if(collection!=null) ntcraw = collection.getNumberOfElements();
    _hEMrawNhits.fill((float)ntcraw);
    for(int i = 0; i<ntcraw; ++i) {
      RawCalorimeterHit ihit = (RawCalorimeterHit) collection.getElementAt(i);

      // Fill histograms here
      float ene = ihit.getAmplitude();
      float time = ihit.getTimeStamp();
      _hEMrawEnergy.fill( ene );
      _hEMrawTime.fill( time );
      _hEMrawEnergyLog.fill( Math.log(ene)*lntolog10 );
      _hEMrawTimeLog.fill( Math.log(time)*lntolog10 );
    }

    // calib hits
    collection = null;
    try {
	collection = event.getCollection("EcalCalibHits");
    }
    catch(Exception x) {
	System.out.println("Unavailable collection: EcalCalibHits");
    }

    _nhits = 0;
    _esum = 0;
    int ntccalib = 0;
    if(collection!=null) ntccalib = collection.getNumberOfElements();
    _hEMclbNhits.fill((float)ntccalib);
    for(int i = 0; i<ntccalib; ++i) {
      CalorimeterHit ihit = (CalorimeterHit) collection.getElementAt(i);

      // Fill histograms here
      float ene = ihit.getEnergy();
      float time = ihit.getTime();
      _hEMclbEnergy.fill( ene );
      _hEMclbTime.fill( time );
      _hEMclbEnergyLog.fill( Math.log(ene)*lntolog10 );
      _hEMclbTimeLog.fill( Math.log(time)*lntolog10 );

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
  // A new tree is created, to be stored in EcalRawHistos.aida file
  public EcalRawHistos() throws IOException {
    IAnalysisFactory af = IAnalysisFactory.create();
    _tree = af.createTreeFactory().create("EcalRawHistos.aida","xml",false,true);
    IHistogramFactory hf = af.createHistogramFactory(_tree);
    bookHistos(hf);
  }

  // This constructor to be used from a "framework".  An .aida tree
  // should be provided, then histograms will be booked inside a folder
  // called "EcalRawHistos".
  public EcalRawHistos(IAnalysisFactory af, ITree tree) throws IOException {
    // save tree pointer for reuse in event loop
    _tree = tree;
    // create a new folder within the existing tree
    _ownFolder = true;
    _tree.mkdir("Ecal");
    _tree.cd("Ecal");

    // book histograms
    IHistogramFactory hf = af.createHistogramFactory(_tree);
    bookHistos(hf);

    _tree.cd("..");
  }

  // Histograms are to be created here
  public void bookHistos(IHistogramFactory hf) {
    // Create histograms here
    _hEMsimNhits = hf.createCloud1D("EMsim: Nhits");
    _hEMrawNhits = hf.createCloud1D("EMraw: Nhits");
    _hEMclbNhits = hf.createCloud1D("EMclb: Nhits");

    _hEMsimEnergy = hf.createHistogram1D("EMsim: Energy", 500, 0, 0.3);
    _hEMrawEnergy = hf.createHistogram1D("EMraw: Energy", 50, 0, 300);
    _hEMclbEnergy = hf.createHistogram1D("EMclb: Energy", 60, 0, 3);

    _hEMsimEnergyLog = hf.createCloud1D("EMsim: Log Energy");
    _hEMrawEnergyLog = hf.createCloud1D("EMraw: Log Energy");
    _hEMclbEnergyLog = hf.createCloud1D("EMclb: Log Energy");

    _hEMsimTime = hf.createCloud1D("EMsim: Time stamps");
    _hEMrawTime = hf.createCloud1D("EMraw: Time stamps");
    _hEMclbTime = hf.createCloud1D("EMclb: Time stamps");

    _hEMsimTimeLog = hf.createCloud1D("EMsim: Log time stamps");
    _hEMrawTimeLog = hf.createCloud1D("EMraw: Log time stamps");
    _hEMclbTimeLog = hf.createCloud1D("EMclb: Log time stamps");

    // Create tuple here
//     ITupleFactory tupleFactory = af.createTupleFactory(tree);
//     _tuple = tupleFactory.create("myTuple","My Title", "int nmc, float etot");
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

    private ICloud1D _hEMsimNhits;
    private IHistogram1D _hEMsimEnergy;
    private ICloud1D _hEMsimTime;
    private ICloud1D _hEMsimEnergyLog;
    private ICloud1D _hEMsimTimeLog;

    private ICloud1D _hEMrawNhits;
    private IHistogram1D _hEMrawEnergy;
    private ICloud1D _hEMrawTime;
    private ICloud1D _hEMrawEnergyLog;
    private ICloud1D _hEMrawTimeLog;

    private ICloud1D _hEMclbNhits;
    private IHistogram1D _hEMclbEnergy;
    private ICloud1D _hEMclbTime;
    private ICloud1D _hEMclbEnergyLog;
    private ICloud1D _hEMclbTimeLog;

//     private ITuple _tuple;

    private boolean _ownFolder = false;
    private ITree _tree;
    private int _nEvents;
    private double lntolog10 = 1/Math.log(10);

  public static void main(String[] args) throws Exception {
    EcalRawHistos analysis = new EcalRawHistos();
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
