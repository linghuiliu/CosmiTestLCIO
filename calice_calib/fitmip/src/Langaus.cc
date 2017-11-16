
#include    "../include/Langaus.hh"
using namespace RooFit;
using namespace std;

//___________________________________________________________________________________________
Langaus::Langaus(){
}
//___________________________________________________________________________________________
Langaus::Langaus(const double* fitrange,const double* startValues,const double* parlimitslo, const double* parlimitshi, bool useFFT){

    RooRealVar    *t      = new RooRealVar("t","t",fitrange[0],fitrange[1]);
    RooRealVar    *mg     = new RooRealVar("mg","mg",0);
    RooRealVar    *sg     = new RooRealVar("sg","sg",startValues[2],parlimitslo[2],parlimitshi[2]);
    RooRealVar    *ml     = new RooRealVar("ml","mean landau",startValues[1],parlimitslo[1],parlimitshi[1]);
    RooRealVar    *sl     = new RooRealVar("sl","sigma landau",startValues[0],parlimitslo[0],parlimitshi[0]);
    //RooRealVar * mp=new RooRealVar("mp","mean poisson",startValues[1],parlimitslo[1],parlimitshi[1]);

    RooLandau     *landau = new RooLandau("landau","landau",*t,*ml,*sl);
    RooGaussian   *gauss  = new RooGaussian("gaus","gaus",*t,*mg,*sg);
    //RooPoisson * poisson=new RooPoisson("poisson","poisson",*t,*mp);

    //RooNumConvPdf * gxp=new RooNumConvPdf("gxp","poisson (x) gauss", *t,*poisson, *gauss);

    RooFFTConvPdf *lxg    = new RooFFTConvPdf("lxg","lxg",*t,*landau,*gauss,2);
    RooNumConvPdf *lxgNum = new RooNumConvPdf("lxgNum","lxgNum",*t,*landau,*gauss);

    //RooFFTConvPdf* lxg=new RooFFTConvPdf("lxg","landau (X) gauss (X) poisson", *t, *landau, *(dynamic_cast<RooAbsPdf*>(gxp)),2);

    lxg->setCacheObservables(*t);

    _t=t, _mg=mg, _sg=sg, _ml=ml, _sl=sl, _landau=landau, _gauss=gauss, _lxg=lxg, _lxgNum=lxgNum;
    _fitrange=fitrange, _startValues=startValues, _parlimitslo=parlimitslo, _parlimitshi=parlimitshi;
    _var=_lxg->getVariables(); 
    _data=0,_r=0;
    _peak=0, _ePeak=0;
    _useFFT=useFFT;

    _lxgNum->convIntConfig().setEpsRel(EPS_REL);
    _lxgNum->convIntConfig().setEpsAbs(EPS_ABS);
    
};
//___________________________________________________________________________________________
Langaus::~Langaus(){
    std::cout<<"Destructor of object Langaus"<<'\n';
    delete _t;
    delete _mg;
    delete _sg;
    delete _ml;
    delete _sl;
    delete _landau;
    delete _gauss;
    delete _lxg;
    delete _lxgNum;
    if(_data!=0) delete _data;
}
//___________________________________________________________________________________________
Langaus& Langaus::setMl(double ml){
    _ml->setVal(ml);
    return *this;
}
//___________________________________________________________________________________________
Langaus& Langaus::setSl(double sl){
    _sl->setVal(sl);
    return *this;
}
//___________________________________________________________________________________________
Langaus& Langaus::setSg(double sg){
    _sg->setVal(sg);
    return *this;
}
//___________________________________________________________________________________________
Langaus& Langaus::setData(TH1* h){
    if (_data!=0){ 
        delete _data;
    }
    _data = new RooDataHist("data","data for t",*_t,h);
    return *this;
}
//___________________________________________________________________________________________
RooDataHist* Langaus::getData(){
    if (_data==0){
        throw runtime_error("No data.");
    }
    return _data;
}
//___________________________________________________________________________________________
double Langaus::getChiSquare(){
    RooPlot* frame = _t->frame(Name("frame"),Title("frame"));
    _data->plotOn(frame) ;
    _lxg->plotOn(frame) ;
    return frame->chiSquare(0);
}
//___________________________________________________________________________________________
double Langaus::findPeak(){

    double  value  = 0;
    double  xvalue = 0;

    RooRealVar* t=new RooRealVar("t","t",_t->getMin(),_t->getMax());
    if(_useFFT==1){

        int i=0;
        t->setBins(FFT_CACHE,"cache");

        RooFFTConvPdf* lxg=new RooFFTConvPdf("lxg","landau (X) gauss",*t,*_landau,*_gauss) ;
        RooDataHist* lxgcache=lxg->getCacheHist(RooArgSet(*t));

        t= (RooRealVar*)lxgcache->get()->find("t");
        lxgcache=lxg->getCacheHist(RooArgSet(*t));

        i=0;
        lxgcache->get(i);
        value=lxgcache->weight();
        xvalue=t->getVal();

        while(i<lxgcache->numEntries()){

            lxgcache->get(i);
            if(value<lxgcache->weight()) value=lxgcache->weight() ,xvalue=t->getVal();
            ++i;
        }
        delete lxg;
    }else{
        RooNumConvPdf* lxg=new RooNumConvPdf("lxg","landau (X) gauss",*t,*_landau,*_gauss) ;

        double tempvalue=0;
        double xScale[2]={_t->getMin(),_t->getMax()};

        /*  find peak maximum */
        for ( int i=0;i<NUM_SAMPLING;++i ) {
            t->setVal(xScale[0]+(float(i)/(float)NUM_SAMPLING)*(xScale[1]-xScale[0]));
            tempvalue=lxg->evaluate();
            if(value<tempvalue) value=tempvalue, xvalue=t->getVal();
        }

        double tempRange=0;
        (xvalue-xScale[0])<(xScale[1]-xvalue)? tempRange=xvalue-xScale[0] : tempRange=xScale[1]-xvalue;

        /*  repeat until precision given by NUM_RATIO is reached */
        while(tempRange/float(NUM_SAMPLING)/abs(xScale[0]-xScale[1])>NUM_RATIO ){
            for ( int i=0;i<NUM_SAMPLING;++i ) {
                t->setVal(xvalue-tempRange+(float(i)/(float)NUM_SAMPLING)*2*tempRange);
                tempvalue=lxg->evaluate();
                if(value<tempvalue) value=tempvalue, xvalue=t->getVal();
            }
            tempRange/=2.;
        }
        delete lxg;
    }

    printf("Peak:%.2f\n",xvalue);
    _peak=xvalue;
    return xvalue ;
}
//_____________________________________________________________________________
double Langaus::findPeak(const double* par){

    RooArgSet* var=0;
    if(_useFFT==0){    
        var=_lxg->getVariables();
    }else{
        var=_lxgNum->getVariables();
    }
    RooRealVar* t  = (RooRealVar*)var->find("t");
    RooRealVar* ml = (RooRealVar*)var->find("ml");
    RooRealVar* sl = (RooRealVar*)var->find("sl");
    RooRealVar* sg = (RooRealVar*)var->find("sg");

    ml->setVal(par[0]);
    sl->setVal(par[1]);
    sg->setVal(par[2]);
    //var->Print("v");

    double  value  = 0;
    double  xvalue = 0;


    if(_useFFT==1){
        int     i      = 0;

        RooDataHist* lxgcache=_lxg->getCacheHist(RooArgSet(*_t));

        t        = (RooRealVar*)lxgcache->get()->find("t");
        lxgcache = _lxg->getCacheHist(RooArgSet(*t));
        i        = 0;
        lxgcache->get(i);
        value    = lxgcache->weight();
        xvalue   = t->getVal();

        while(i<lxgcache->numEntries()){
            lxgcache->get(i);
            if(value<lxgcache->weight()) value=lxgcache->weight() ,xvalue=t->getVal();
            ++i;
        }
    }else{
        RooRealVar* t=new RooRealVar("t","t",_t->getMin(),_t->getMax());
        RooNumConvPdf* lxg=new RooNumConvPdf("lxg","landau (X) gauss",*t,*_landau,*_gauss) ;


        double tempvalue=0;
        double xScale[2]={_t->getMin(),_t->getMax()};

        /*  find peak maximum */
        for ( int i=0;i<NUM_SAMPLING;++i ) {
            t->setVal(xScale[0]+(float(i)/(float)NUM_SAMPLING)*(xScale[1]-xScale[0]));
            tempvalue=lxg->evaluate();
            if(value<tempvalue) value=tempvalue, xvalue=t->getVal();
        }

        double tempRange=0;
        (xvalue-xScale[0])<(xScale[1]-xvalue)? tempRange=xvalue-xScale[0] : tempRange=xScale[1]-xvalue;

        /*  repeat until precision given by NUM_RATIO is reached */
        while(tempRange/float(NUM_SAMPLING)/abs(xScale[0]-xScale[1])>NUM_RATIO ){
            for ( int i=0;i<NUM_SAMPLING;++i ) {
                t->setVal(xvalue-tempRange+(float(i)/(float)NUM_SAMPLING)*2*tempRange);
                tempvalue=lxg->evaluate();
                if(value<tempvalue) value=tempvalue, xvalue=t->getVal();
            }
            tempRange/=2.;
        }



        delete lxg;
        delete t;
    }

    printf("Peak: %.2f\n",xvalue);
    return xvalue;
}
//_____________________________________________________________________________
double Langaus::findePeak(){


    double par[3],epar[3];
    par[0]  = _ml->getVal();
    par[1]  = _sl->getVal();
    par[2]  = _sg->getVal();
    epar[0] = _ml->getError();
    epar[1] = _sl->getError();
    epar[2] = _sg->getError();

    if(_peak==0) _peak=findPeak();
    double derivates[3];
    for(int i=0;i<3;++i) derivates[i]=0;
    double step;

    int count;
    for(int i=0;i<3;i++){
        step=0.01*par[i];
        if(epar[i]/par[i]>0.01) step=epar[i]/4.0;
        count=0;
        do{ 
            ++count;

            par[i]+=step;
            derivates[i]=(findPeak(par)-_peak)/step;
            par[i]-=step; 

            if(par[i]-step>0&&i!=2){
                par[i]-=step;
                derivates[i]=(derivates[i]+(_peak-findPeak(par))/step)/2;
                par[i]+=step;
            } 
            else{               
                //derivates[i]=0;
            }
            if(i==2){
                par[i]-=step;
                derivates[i]=(derivates[i]+(_peak-findPeak(par))/step)/2;
                par[i]+=step;
            }
            if(derivates[i]==0) ++step;
        }while(derivates[i]==0&&count<20);
    }
    findPeak(par);

    double error=0;
    double cor[3][3];
    cor[1][2] = _r->correlation("sl","sg");
    cor[0][1] = _r->correlation("ml","sl");
    cor[0][2] = _r->correlation("ml","sg");
    cor[2][1] = cor[1][2],cor[1][0] = cor[0][1],cor[2][0] = cor[0][2];
    for( int i=0;i<3;i++) cor[i][i]=1;

    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            error+=derivates[i]*cor[i][j]*derivates[j]*epar[i]*epar[j];    
        }
    }
    if(error<0){
        throw "Not positive definite correlation matrix";
    }
    error=sqrt(error);

    printf("peak:\t %.2f +- %.2f\n",_peak,error);

    _ePeak=error;
    return error;
}
//___________________________________________________________________________________________
RooFitResult* Langaus::Fit(void){
    if(_useFFT==1){
        _r= _lxg->fitTo(*_data, Range(_fitrange[0],_fitrange[1]),Strategy(1),Save()) ;
        printf("Minimum of -log(L):\t%3.f\n",_r->minNll());
        _logL=_r->minNll();
    }else{
        _lxgNum->setConvolutionWindow(*(RooAbsReal*)_mg,*(RooAbsReal*)_sg ,100.);
        _r= _lxgNum->fitTo(*_data, Range(_fitrange[0],_fitrange[1]),Strategy(1),Save()) ;
    }

    _mlerror = _ml->getError();
    _slerror = _sl->getError();
    _sgerror = _sg->getError();
    return _r;
}
//___________________________________________________________________________________________
void Langaus::langaufit( const TH1* h, double* fitparams, double* fiterrors,double& peak,double& epeak){
    
        ((RooRealVar*)this->getT())->setBins(FFT_CACHE,"cache");
   
    this->setData((TH1*)h);
    this->Fit(); 
    // Construct landau (x) gauss    (fast fourier transformation)

    //m.setStrategy(2);
    //m.fit("m");
    //m.migrad();
    //m.hesse();
    //m.migrad();
    //m.minos();

    //retrieve values of observables
    fitparams[0] = this->getSl()->getVal();
    fitparams[1] = this->getMl()->getVal();
    fitparams[2] = this->getSg()->getVal();
    //retrieve values for errors of observables
    fiterrors[0] = this->getSlerror();
    fiterrors[1] = this->getMlerror();
    fiterrors[2] = this->getSgerror();
    
    peak=findPeak();
    printf("----estimation of peak error--------\n");
    epeak=findePeak();
}
//___________________________________________________________________________________________
double Langaus::getCorrelation(const char* first,const char* second){
    if(_r->correlation(first,second)==0){
        printf("Arguments for Langaus::getCorrelation(const char*, const char*):\n"
                "\tml(Landau mean), sl(Landau sigma), sg(Gaus sigma)\n");
        return 0;
    }else{
        return _r->correlation(first,second);
    }
}
//___________________________________________________________________________________________

int Langaus::FFT_CACHE=50000;
float Langaus::NUM_RATIO=0.01;
int Langaus::NUM_SAMPLING=1000;
float Langaus::EPS_ABS=0.00001;
float Langaus::EPS_REL=0.01;

















