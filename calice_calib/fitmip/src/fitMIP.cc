#include    "Fitlikelihood.hh"
#include    "Langaus.hh"

#include    <fstream>
#include    <iostream>
#include    <cstdlib>
#include    <stdexcept>

#include    <vector>

#include    "RooFit.h"
#include    "RooPlot.h"
#include    "TGraph.h"
using namespace std;

/**@file fitMIP.cc
 * @brief Example of main function for finding the MIP calibration values using classes Langaus, Bezier and
 * Fitlikelihood. The mip calibration value 
 * is found by fitting the histogram by function defined like convolution of Landau and Gaus function. 
 * The mip calibration value is then the most probable value of function (MPV). For rough estimation of MPV is
 * used Bezier curve implemented in class Bezier. Be aware with tuning your own main program, especially with 
 * tuning of direct numerical calculation of convolution integral. Particulary with fine tuning of lower limit
 * of Gaus sigma. More in #Langaus.
 *
 Detailed description follows here.
 @author Boris Bulanek, DESY
 @date June 2010

 @code
 fitMipRun -infile <root file input> -histoname <name of histogram> [ -outtxt <txt output> -outroot <root file output>   -range <range(float)> -rebin <rebin> ]
 @endcode

 @param[in] inFile Relative entry to the input root file with histograms.
 @param[out] outpath Relative path for the output files.
 @param range Range of fit given as \f$\left<\mbox{left},\mbox{right}\cdot\mbox{peakInit}\right>\f$, where
 peakInit is roughly found peak.
 */


//! minimal number of entries for one cell.
const unsigned _minEntries = 10;
//! minimal value of mean of deposited energy for one cell.
const double _minMean    = 0;

//! upper limits for parameters of fit (Landau sigma, Landau mean, Gaus sigma).
const double _parMax[3]  = {400,2000,400};
//! lower limits for parameters of fit (Landau sigma, Landau mean, Gaus sigma).
const double _parMin[3]  = {0, 0, 0};


const char* _infilename;

const char* _histoname;

const char* _outroot;

const char* _outfileName;

ofstream  _outfile;

double _range=3;
double _leftRange,_rightRange;
bool _rangeExplicit=0;

int _mod,_chip,_chan;
const int SHIFT=8;
//! rebining of input histogram for fit.
int _rebin=8;

//! order of Bezier curve.
int _Bezierorder=20;

//! rebining of input histogram for Bezier curve.
int _Bezierrebin=18;

//! cache for discrete fourier transformation
int Langaus::FFT_CACHE=50000;
//! precision for numerical computation of convolution integral
float Langaus::EPS_ABS=0.0000001;
//! precision for numerical computation of convolution integral
float Langaus::EPS_REL=0.0000001;



bool _isRootfile, _isOutfile;
bool _useFFT=0;

void setParameters(int argc, char* argv[]);
void printParameters();
void printHelp();

//_____________________________________________________________________________________________________
int main( int argc, char *argv[] )
{
    if(argc==1|| strcmp(argv[1],"-h")==0){
        printHelp();
        return 1;
    }

    setParameters(argc,argv);
    printParameters();

    cout<<"-----------------------"<<endl;
    printf("Compiled file:\t %s\n"
           "When:\t %s, %s.\n",__FILE__,__DATE__,__TIME__); 
    cout<<"-----------------------"<<endl<<endl;;

    TFile* file=new TFile(_infilename);
    
    TH1* h=0;
    h=dynamic_cast<TH1*>(file->Get(_histoname));
    if (h==0){
        throw runtime_error( "No histogram!" );
        }

    TH1* initial;

    unsigned entries= (unsigned)h->GetEntries();
    printf ( "entries: %i \n",entries );
    double mean= h->GetMean();
    printf ( "mean: %.2f\n",mean );
    if(entries>_minEntries&&mean>_minMean){
        FitLikelihood* Mip=new FitLikelihood(h);

        // for direct numerical computation is needed to adjust properly the cut for lower limit of
        // sigma gauss
        double histoMax=Mip->getHistogram()->GetXaxis()->GetXmin();
        double histoMin=Mip->getHistogram()->GetXaxis()->GetXmin();
        double lowerCut=(histoMax-histoMin)/1000.;
        if(_useFFT==1){
        Mip->setParlimitslo(_parMin[0],_parMin[1],_parMin[2]);
        }else{
        Mip->setParlimitslo(_parMin[0],_parMin[1],lowerCut);
        }
        Mip->setParlimitshi(_parMax[0],_parMax[1],_parMax[2]);
        
       //needed to be carefully tuned 
        initial=Mip->cloneHisto(1,2,4); 


        Mip->findPeakInitBezier(_Bezierorder, _Bezierrebin);
        Mip->findRangeMin(initial);

       //Mip->setRange(0.5,3);                                
        double peakInit=Mip->getPeakInit();
        double RMS=Mip->getHistogram()->GetRMS();
        double rangeMin=Mip->getRangeMin(); 
        if(_rangeExplicit==0){
        Mip->setRange(peakInit -(peakInit-rangeMin)*0.9,peakInit+_range*RMS );
        }else{
            Mip->setRange(_leftRange,_rightRange);
        }
        h->Rebin(_rebin);
        
        printf ( "----------------Start of fit---------------------------\n" );
        
        double fitValues[3]={0,0,0};
        double effitValue[3]={0,0,0};
        double peak=0, epeak=0;

        Langaus* LXG= new Langaus(Mip->getRange(),Mip->getIniValues(),Mip->getParMin(),Mip->getParMax(),_useFFT);

        LXG->langaufit(h,fitValues,effitValue,peak,epeak);

        
        printf("Obtained parameters from fit:\n"
            "\tPeak:\t%.3f+-%.3f\n"
            "\tLandau mean:\t%.3f+-%.3f\n"
            "\tLandau sigma:\t%.3f+-%.3f\n"
            "\tGauss sigma:\t%.3f+-%.3f\n",
            peak,epeak,fitValues[1],effitValue[1],fitValues[0],effitValue[0],fitValues[2],effitValue[2]);

        
        
        Bezier* bezier;
        bezier=Mip->getBezier();
        TMultiGraph* bezierGraph;
        bezierGraph=bezier->plotBezier();
        TFile fileBezier("bezier.root","recreate");
          TCanvas can("can");
           can.cd();
           bezierGraph->Draw("ap");
           can.Write();
        fileBezier.Close();

        
        RooPlot* frame;
        /*  Get RooPlot, create legend via TPaveText */
        if(_isRootfile==1){ 

            frame=Mip->writeRooroot(h,LXG);
            TFile fitfile(_outroot,"update");                                       
            TPaveText* tbox=new TPaveText(0.55,0.5,0.85,0.85,"BRNDC");
            stringstream nameLab;
            char buffer1[40], buffer2[40];
            sprintf(buffer1,"%.3f",LXG->getMl()->getVal() );
            sprintf(buffer2,"%.3f",LXG->getMlerror());
            nameLab<<"#mu_{L}="<<buffer1<<"#pm"<<buffer2;
            tbox->AddText(nameLab.str().c_str());
            nameLab.str("");

            sprintf(buffer1,"%.3f",LXG->getSl()->getVal() );
            sprintf(buffer2,"%.3f",LXG->getSlerror());
            nameLab<<"#sigma_{L}="<<buffer1<<"#pm"<<buffer2;
            tbox->AddText(nameLab.str().c_str());
            nameLab.str("");

            sprintf(buffer1,"%.3f",abs(LXG->getSg()->getVal()) );
            sprintf(buffer2,"%.3f",LXG->getSgerror());
            nameLab<<"#sigma_{G}"<<"="<<buffer1<<"#pm"<<buffer2;
            tbox->AddText(nameLab.str().c_str());
            nameLab.str("");

            sprintf(buffer1,"%.3f",peak);
            sprintf(buffer2,"%.3f",epeak);
            nameLab<<"A_{MIP}"<<"="<<buffer1<<"#pm"<<buffer2;
            tbox->AddText(nameLab.str().c_str());

            frame->addObject(tbox);
            frame->Draw() ; 
            frame->Write();
            fitfile.Close();
        }


        
        if(_isOutfile == 1){
            int ID=(_mod<<SHIFT|_chip)<<SHIFT|_chan;
            _outfile<<ID<<'\t'<<peak<<'\t'<<epeak<<'\t'<<Mip->getHistogram()->GetEntries()<<'\t'<<LXG->getSl()->getVal()<<'\t'<<LXG->getSlerror()<<'\t'<<LXG->getMl()->getVal()<<'\t'<<LXG->getMlerror()<<'\t'<<abs(LXG->getSg()->getVal())<<'\t'<<LXG->getSgerror()<<endl;
            _outfile.close();
        }

        delete Mip;
        delete LXG;
        }
    
    h->Delete();
    return 0;
}

void printHelp(){
    cout<<"Obligatory parameters: "<< endl
        <<"\t -infile <root input file>"<<endl
        <<"\t -histoname <name of histogram>"<<endl;
    cout<<endl;
    cout<<"Other parameters: "<<endl
        <<"\t -outtxt <txt output>"<<endl
        <<"\t -outroot <root file output>"<<endl
        <<"\t -range <range of fit(explained in Fitlikelihood)>"<<endl
        <<"\t -rangeExplicit <rangeLeft rangeRight>"<<endl
        <<"\t -rebin <rebin of histogram>"<<endl
        <<"\t -bezierorder <order of bezier function>"<<endl
        <<"\t -bezierrebin <bezier rebin>"<<endl
        <<"\t -useFFT <bool(0)>"<<endl;
}

//_____________________________________________________________________________________________________
void setParameters(int argc, char* argv[]){
   _isOutfile=0;
   _isRootfile=0;
   
   
   for(int i=0; i<argc;++i){ 
    if(strcmp("-infile",argv[i])==0){
        _infilename=argv[++i];
    }
    if(strcmp("-outtxt", argv[i])==0){
        _outfile.open(argv[++i],fstream::app);
        _outfileName=argv[i];
        _isOutfile=1;
    }
    if(strcmp("-histoname", argv[i])==0){
        _histoname=argv[++i];
    }
    if(strcmp("-outroot", argv[i])==0){
        _outroot=argv[++i];
        _isRootfile=1;
    }
    if(strcmp("-range", argv[i])==0){
        _range=atof(argv[++i]);
    }
    if(strcmp("-rebin", argv[i])==0){
        _rebin=atoi(argv[++i]);
    }
    if(strcmp("-bezierorder", argv[i])==0){
        _Bezierorder=atoi(argv[++i]);
    }
    if(strcmp("-bezierrebin", argv[i])==0){
        _Bezierrebin=atoi(argv[++i]);
    }
    if(strcmp("-rangeExplicit", argv[i])==0){
        _rangeExplicit=1;
        _leftRange=atof(argv[++i]);
        _rightRange=atof(argv[++i]);
    }
    if(strcmp("-useFFT", argv[i])==0){
        _useFFT=(bool)atoi(argv[++i]);
    }
if(strcmp("-mod", argv[i])==0){
        _mod=(int)atoi(argv[++i]);
    }
if(strcmp("-chip", argv[i])==0){
        _chip=(int)atoi(argv[++i]);
    }
if(strcmp("-chan", argv[i])==0){
        _chan=(int)atoi(argv[++i]);
    }

}
}
void printParameters(){
    printf ( "Input rootfile: %s \n",_infilename );
    printf ( "Name of input histogram: %s \n",_histoname );
    if(_isRootfile==1) printf ( "Name of output root file: %s \n",_outroot );
    if(_isOutfile) printf ( "Name of output txt file: %s \n",_outfileName );
    printf ( "Rebining of histogram: %i \n",_rebin );
    printf ( "Right range: %.1f \n",_range);
    if(_rangeExplicit==1) printf ( "Range of fit: <%.1f,%.1f> \n",_leftRange,_rightRange);
    printf ( "Mod:%i, chip:%i, chan:%i \n",_mod,_chip,_chan );
}




