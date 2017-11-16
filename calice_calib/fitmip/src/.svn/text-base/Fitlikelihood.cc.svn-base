#include    "../include/Fitlikelihood.hh"
using namespace std;
using namespace RooFit;

//_____________________________________________________________________________________
FitLikelihood::FitLikelihood(TH1* h){
    _histogram    = h;
    _iniValues[1] = (double)_histogram->GetMean();
    _iniValues[0] = (double)_histogram->GetRMS()/2;
    _iniValues[2] = _iniValues[0];
    _rangeMin     = 0;
    _peakInit     = h->GetMean();
    for(int i=0;i<2;++i) _rangeInit[i]=0;
}
//______________________________________________________________________________________
double FitLikelihood::findRangeMin(const TH1* clone){
    int rmaxbin     = clone->GetXaxis()->FindBin(_peakInit);
    int zerobin     = clone->GetXaxis()->FindBin((double)0.);
    float content   = 0;
    float min       = (float)clone->GetBinContent(zerobin);
    int minX        = clone->GetXaxis()->FindBin((double)0);
    double histoMin = 0;

    for ( int i=zerobin;i<rmaxbin ;i++ ){
        content=(float)clone->GetBinContent(i); 
        if((min>=content)) min=content, minX=i;
    }

    histoMin=clone->GetXaxis()->GetBinCenter(minX);
    if(histoMin<_peakInit/2.&&histoMin>0){
        _rangeMin=clone->GetXaxis()->GetBinCenter(minX);
    }else{
        _rangeMin=0;
    }
    printf("_rangeMin:\t%.2f\n",_rangeMin);
    return _rangeMin;
}
//______________________________________________________________________________________
double FitLikelihood::findPeakInitBezier(int order,int rebin){
    TH1* h=(TH1*)_histogram->Clone();
    
    int rmin_inibin= h->FindBin(_rangeMin);

    for(int i=0;i<rmin_inibin;++i){
        h->SetBinContent(i,0);
    }
    
    
    float peak;
    vector<float> x,y;
    int binmin=h->FindBin(_parlimitslo[1]);
    int binmax=h->FindBin(_parlimitshi[1]);

    for ( int i=binmin;i<binmax;++i ) {
        x.push_back(h->GetBinCenter(i));
        y.push_back(h->GetBinContent(i) );
    }
    _bezier=new Bezier(h,rebin, order,1 );
    peak=_bezier->findPeak();
    //if(peak>_parlimitslo[1]&&peak<_parlimitshi[1]) _peakInit=(double)peak;  
    _peakInit=(double)peak;
    _iniValues[1]=(double)peak;
    printf("Peak extracted from Bezier function:\t%.3f\n",peak); 
    return (double)peak;
}
//______________________________________________________________________________________
FitLikelihood& FitLikelihood::setRange(double left, double right){
    _rangeInit[0] = left;
    _rangeInit[1] = right;

    if(_rangeInit[0]<_histogram->GetXaxis()->GetXmin() ){
        _rangeInit[0]=_histogram->GetXaxis()->GetXmin();
    }
    if(_rangeInit[1]>_histogram->GetXaxis()->GetXmax() ){
        _rangeInit[1]=_histogram->GetXaxis()->GetXmax();
    }
    return *this;
}
//______________________________________________________________________________________
FitLikelihood& FitLikelihood::setIniValues(const double* iniValues){
    for(int i=0; i<3;++i) _iniValues[i]=iniValues[i];
    return *this;
}
//______________________________________________________________________________________
FitLikelihood& FitLikelihood::setParlimitslo(double widthlo,double peaklo,double sigmalo){
    _parlimitslo[0] = widthlo;
    _parlimitslo[1] = peaklo;
    _parlimitslo[2] = sigmalo;
    return *this;
};
//______________________________________________________________________________________
FitLikelihood& FitLikelihood::setParlimitshi(double widthhi,double peakhi,double sigmahi){
    _parlimitshi[0] = widthhi;
    _parlimitshi[1] = peakhi;
    _parlimitshi[2] = sigmahi;
    return *this;
};
//______________________________________________________________________________________
FitLikelihood& FitLikelihood::setPeakInit(const double energy){
    _peakInit = energy;
    return *this;
}
//______________________________________________________________________________________
RooPlot* FitLikelihood::writeRooroot( const TH1* h,const Langaus* lxg){

    RooRealVar* t = (RooRealVar*)lxg->getLangausBase()->getVariables()->find("t");
    RooDataHist data("data","data for t",*t,h);                                           

    RooPlot* Frame = t->frame(Name(h->GetName()),Title(h->GetName()));
    data.plotOn(Frame) ;
    lxg->getLangausBase()->plotOn(Frame) ;
    
    return Frame;
}
//______________________________________________________________________________________
TH1* FitLikelihood::cloneHisto(int rbn,int rng,int lps){
    assert(_histogram);

    TH1 *clone= (TH1*)_histogram->Clone();  
    clone->SetName("clone");
    clone->Rebin(rbn);
    for(int l=0;l<lps;++l){
        Int_t N=_histogram->GetNbinsX();
        Int_t p;
        Double_t Content;
        for(p=rng+1;p<=N-rng-1;++p){
            Int_t t=rng;  
            do{
                Content=clone->GetBinContent(p)+
                    (+_histogram->GetBinContent(p-t)+_histogram->GetBinContent(p+t));
                t--;
                clone->SetBinContent(p,Content);
            }while(t>0);
            clone->SetBinContent(p,Content/(2*rng+1));
        }
    }
    return clone;
};
//_____________________________________________________________________________________________

