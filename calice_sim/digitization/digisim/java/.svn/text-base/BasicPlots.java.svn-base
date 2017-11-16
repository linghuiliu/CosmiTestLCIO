//
// File: BasicPlots.java
//
// Purpose: 
//
// Usage:
//   This class can be run either standalone (java BasicPlots data.slcio)
// or within a "framework", like (java AnalyzeLcioNew data.slcio), where
// AnalyzeLcioNew instantiates and uses class BasicPlots.
//
// 20040906 - G.Lima - Created

import hep.aida.*;
import hep.lcio.event.*;
import hep.lcio.event.RawCalorimeterHit;
import hep.lcio.implementation.io.LCFactory;
import hep.lcio.io.*;
import java.io.IOException;

public class BasicPlots implements LCEventListener {

  public void processEvent(LCEvent event) {

      // MCParticles
    LCCollection mcparts = null;
    try {
	mcparts = event.getCollection("MCParticle");
    }
    catch(Exception x) {
	System.out.println("Unavailable collection: MCParticle");
    }

    MCParticle mcp = null;
    if(mcparts.getNumberOfElements()==1) {
	mcp = (MCParticle)mcparts.getElementAt(0);
    }
    else {
	for(int i=0; i<mcparts.getNumberOfElements(); ++i) {
	    mcp = (MCParticle)mcparts.getElementAt(i);
	    if(mcp.getGeneratorStatus()==1) break; // pick FS particles
	}
	mcp = null;
    }
    assert mcp!=null : "No FS particle found.";

    double mcE = mcp.getEnergy();
    System.out.println("mcE="+mcE);


    // EM raw hits
    LCCollection collection = null;
    try {
        collection = event.getCollection("EcalBarrHits");
    }
    catch(Exception x) {
	System.out.println("Unavailable collection: EcalBarrHits");
    }

    int nhits = 0;
    double rawE = 0;
    if(collection!=null) nhits = collection.getNumberOfElements();

    for(int i = 0; i<nhits; ++i) {
      SimCalorimeterHit ihit = (SimCalorimeterHit) collection.getElementAt(i);

      // Fill histograms here
      rawE += ihit.getEnergy();
//       _hEMadcLog.fill( Math.log(rawE)*lntolog10 );
//       _hEMtimeLog.fill( Math.log(time)*lntolog10 );
    }

    int pdgid = mcp.getPDG();
    if(pdgid==22) {
	_hPhotonNhits.fill(mcE, (float)nhits);
	_hPhotonEraw.fill(mcE, rawE);
    }
    else {
	_hNeutronNhits.fill(mcE, (float)nhits);
	_hNeutronEraw.fill(mcE, rawE);
    }

    _nEvents++;
  }

  public void endOfJob() {
  }

  public void modifyEvent(LCEvent lCEvent) {
    // No thanks
  }

  // Constructor called by the main routine (standalone mode)
  // A new tree is created, to be stored in BasicPlots.aida file
  public BasicPlots() throws IOException {
    IAnalysisFactory af = IAnalysisFactory.create();
    _tree = af.createTreeFactory().create("BasicPlots.aida","xml",false,true);
    IHistogramFactory hf = af.createHistogramFactory(_tree);
    bookHistos(hf);
  }

  // This constructor to be used from a "framework".  An .aida tree
  // should be provided, then histograms will be booked inside a folder
  // called "BasicPlots".
  public BasicPlots(IAnalysisFactory af, ITree tree) throws IOException {
    // save tree pointer for reuse in event loop
    _tree = tree;
    // create a new folder within the existing tree
    _ownFolder = true;
    _tree.mkdir("BasicPlots");
    _tree.cd("BasicPlots");

    // book histograms
    IHistogramFactory hf = af.createHistogramFactory(_tree);
    bookHistos(hf);

    _tree.cd("..");
  }

  // Histograms are to be created here
  public void bookHistos(IHistogramFactory hf) {
    // Create histograms here
    _hPhotonNhits = hf.createCloud2D(".","Photon Nhits", 200);
    _hPhotonEraw = hf.createCloud2D(".","Photon Eraw", 200);
    _hNeutronNhits = hf.createCloud2D(".","Neutron Nhits", 200);
    _hNeutronEraw = hf.createCloud2D(".","Neutron Eraw", 200);

    // Create tuple here
//     ITupleFactory tupleFactory = af.createTupleFactory(tree);
//     _tuple = tupleFactory.create("myTuple","My Title", "int nmc, float etot");
  }

    // ***** Member data  *****

    private ICloud2D _hPhotonNhits;
    private ICloud2D _hPhotonEraw;
    private ICloud2D _hNeutronNhits;
    private ICloud2D _hNeutronEraw;

    private boolean _ownFolder = false;
    private ITree _tree;
    private int _nEvents;
    private double lntolog10 = 1/Math.log(10);

  public static void main(String[] args) throws Exception {
    BasicPlots analysis = new BasicPlots();
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
