//
// File: RawHistos.java
//
// Purpose: 
//
// Usage:
//   This class can be run either standalone (java RawHistos data.slcio)
// or within a "framework", like (java AnalyzeLcioNew data.slcio), where
// AnalyzeLcioNew instantiates and uses class RawHistos.
//
// 20040906 - G.Lima - Created

import hep.aida.*;
import hep.lcio.event.*;
import hep.lcio.event.RawCalorimeterHit;
import hep.lcio.implementation.io.LCFactory;
import hep.lcio.io.*;
import java.io.IOException;

public class RawHistos implements LCEventListener {

  public void processEvent(LCEvent event) {

    // EM raw hits
    LCCollection collection = null;
    try {
        collection = event.getCollection("EcalBarrRawHits");
    }
    catch(Exception x) {
	System.out.println("Unavailable collection: EcalBarrRawHits");
    }
    int nemraw = 0;
    if(collection!=null) nemraw = collection.getNumberOfElements();
    _hEMNhits.fill((float)nemraw);
    for(int i = 0; i<nemraw; ++i) {
      RawCalorimeterHit ihit = (RawCalorimeterHit) collection.getElementAt(i);

      // Fill histograms here
      float adc = ihit.getAmplitude();
      float time = ihit.getTimeStamp();
      _hEMadc.fill( (float)adc );
      _hEMtime.fill( (float)time );
      _hEMadcLog.fill( Math.log(adc)*lntolog10 );
      _hEMtimeLog.fill( Math.log(time)*lntolog10 );
    }

    // HAD raw hits
    collection = null;
    try {
	collection = event.getCollection("HcalBarrRawHits");
    }
    catch(Exception x) {
	System.out.println("Unavailable collection: HcalBarrRawHits");
    }
    int nhadraw = 0;
    if(collection!=null) nhadraw = collection.getNumberOfElements();
    _hHADNhits.fill((float)nhadraw);
    for(int i = 0; i<nhadraw; ++i) {
      RawCalorimeterHit ihit = (RawCalorimeterHit) collection.getElementAt(i);

      // Fill histograms here
      float adc = ihit.getAmplitude();
      float time = ihit.getTimeStamp();
      _hHADadc.fill( (float)adc );
      _hHADtime.fill( (float)time );
      _hHADadcLog.fill( Math.log(adc)*lntolog10 );
      _hHADtimeLog.fill( Math.log(time)*lntolog10 );
    }

    _nEvents++;
  }

  public void endOfJob() {
  }

  public void modifyEvent(LCEvent lCEvent) {
    // No thanks
  }

  // Constructor called by the main routine (standalone mode)
  // A new tree is created, to be stored in RawHistos.aida file
  public RawHistos() throws IOException {
    IAnalysisFactory af = IAnalysisFactory.create();
    _tree = af.createTreeFactory().create("RawHistos.aida","xml",false,true);
    IHistogramFactory hf = af.createHistogramFactory(_tree);
    bookHistos(hf);
  }

  // This constructor to be used from a "framework".  An .aida tree
  // should be provided, then histograms will be booked inside a folder
  // called "RawHistos".
  public RawHistos(IAnalysisFactory af, ITree tree) throws IOException {
    // save tree pointer for reuse in event loop
    _tree = tree;
    // create a new folder within the existing tree
    _ownFolder = true;
    _tree.mkdir("RawHistos");
    _tree.cd("RawHistos");

    // book histograms
    IHistogramFactory hf = af.createHistogramFactory(_tree);
    bookHistos(hf);

    _tree.cd("..");
  }

  // Histograms are to be created here
  public void bookHistos(IHistogramFactory hf) {
    // Create histograms here
    _hEMNhits = hf.createHistogram1D("EM: Number of raw hits", 50, 0, 1000);
    _hEMadc = hf.createHistogram1D("EM: ADC counts", 50, 0, 1000);
    _hEMtime = hf.createHistogram1D("EM: Time stamps", 50, 0, 50);
    _hEMadcLog = hf.createHistogram1D("EM: Log ADC counts", 50, 0, 10);
    _hEMtimeLog = hf.createHistogram1D("EM: Log time stamps", 100, 0, 5);

    _hHADNhits = hf.createHistogram1D("HAD: Number of raw hits", 50, 0, 1000);
    _hHADadc = hf.createHistogram1D("HAD: ADC counts", 50, 0, 5000);
    _hHADtime = hf.createHistogram1D("HAD: Time stamps", 50, 0, 50);
    _hHADadcLog = hf.createHistogram1D("HAD: Log ADC counts", 50, 0, 50);
    _hHADtimeLog = hf.createHistogram1D("HAD: Log time stamps", 100, 0, 5);

    // Create tuple here
//     ITupleFactory tupleFactory = af.createTupleFactory(tree);
//     _tuple = tupleFactory.create("myTuple","My Title", "int nmc, float etot");
  }

    // ***** Member data  *****
    private IHistogram1D _hEMNhits;
    private IHistogram1D _hEMadc;
    private IHistogram1D _hEMtime;
    private IHistogram1D _hEMadcLog;
    private IHistogram1D _hEMtimeLog;

    private IHistogram1D _hHADNhits;
    private IHistogram1D _hHADadc;
    private IHistogram1D _hHADtime;
    private IHistogram1D _hHADadcLog;
    private IHistogram1D _hHADtimeLog;

//     private ITuple _tuple;

    private boolean _ownFolder = false;
    private ITree _tree;
    private int _nEvents;
    private double lntolog10 = 1/Math.log(10);

  public static void main(String[] args) throws Exception {
    RawHistos analysis = new RawHistos();
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
