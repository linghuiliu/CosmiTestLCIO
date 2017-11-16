#include    "Bezier.hh"
#define PRECISION 10
using namespace std;

//______________________________________________________________________________________
Bezier::Bezier(vector<float>& x,vector<float>& y, int order,bool zero){
    _zero=zero;
    for ( unsigned i=0;i<x.size() ;++i ) {
        if(!(_zero==0&&y[i]==0)){
            _xV.push_back(x[i]);
            _yV.push_back(y[i]);
        }
    }
    _order     = order;
    _peak      = 0;
    _wherePeak = 0;
    _precision = (_xV[_xV.size()-1]-_xV[0])/((float)_xV.size()*PRECISION);
}
//______________________________________________________________________________________
Bezier::Bezier(TH1* h, int rebin, int order, bool zero){
    _zero = zero;
    h->Rebin(rebin);
    int binmin = h->FindBin(h->GetXaxis()->GetXmin());
    int binmax = h->FindBin(h->GetXaxis()->GetXmax());

    for ( int i=binmin;i<binmax;++i ) {
        if(!(_zero==0&&h->GetBinContent(i)==0)){
            _xV.push_back(h->GetBinCenter(i));
            _yV.push_back(h->GetBinContent(i) );
        }
    }
    _order     = order;
    _peak      = 0;
    _wherePeak = 0;
    _precision = (_xV[_xV.size()-1]-_xV[0])/((float)_xV.size()*PRECISION);
}
//______________________________________________________________________________________
void    Bezier::setPrecision(float precision){
    _precision=precision;
}
//______________________________________________________________________________________
float* Bezier::getPoint(float t,unsigned from){
    if(!(t>=0&&t<=1)){
        for(int i=0;i<2;++i) _point[i]=0;
        return _point;
    }

    for(int i=0;i<2;++i) _point[i]=0;
    if(_xV.size()-from>_order){
        for (unsigned i=0;i<=_order ;++i ) {
            _point[0] += binomic((int)_order,i) *pow(1-t,(int)(_order-i))*pow(t,(int)i)*_xV[from+i];
            _point[1] += binomic((int)_order,i) *pow(1-t,(int)(_order-i))*pow(t,(int)i)*_yV[from+i];
        }
    }
    return _point;
}
//______________________________________________________________________________________
float Bezier::evaluate(float x){
    unsigned where = 0;
    if(!(x>=_xV[0]&&x<=_xV[_xV.size()-1])) return 0;
    for ( unsigned i=0;i<_xV.size() ;++i ){
        where+=_order;
        if(where<_xV.size())    if(x<_xV[where]){
            where-=_order;
            break;
        }
    }


    if(where+_order>_xV.size()-1) return 0;
    int sgn=1;  
    float difference=x-getPoint(0,where)[0];
    float t=0;
    int k=2;
    while(abs(difference)>_precision){
        t+=1./k; 
        difference=x-getPoint(t,where)[0];
        if(signum(difference)!=sgn){
            sgn=-sgn;
            k=-k;
            k*=2;
        }
    }
    return getPoint(t, where)[1];
}
//______________________________________________________________________________________
float Bezier::evaluate(float x, unsigned where){
   
    unsigned lowest=where%_order;
    if(!(x>=_xV[0]&&x<=_xV[_xV.size()-1])) return 0;
    if(x-_xV[lowest]<0) return 0; 
    where=lowest;
    unsigned Where=lowest;
    do{
        Where=where;
        where+=_order;
    }while(where<_xV.size()-1 && x-_xV[where]>0);
    if(Where+_order>_xV.size()-1) return 0;
    int sgn=1;  
    float difference=x-getPoint(0,Where)[0];
    float t=0;
    int k=2;
    while(abs(difference)>_precision){
        t+=1./k; 
        difference=x-getPoint(t,Where)[0];
        if(signum(difference)!=sgn){
            sgn=-sgn;
            k=-k;
            k*=2;
        }
    }
    return getPoint(t, Where)[1];
}
//______________________________________________________________________________________
float Bezier::findPeak2(){
    float from  = _xV[0];
    float till  = _xV[_xV.size()-1];
    float where = 0;
    float value = 0;
    float tempvalue;
    for ( float x=from;x<till;x+=PRECISION) {
        tempvalue=this->evaluate(x);
        if(tempvalue>value) value=tempvalue, where=x; 
    }
    _peak=where;
    return where;
}
//______________________________________________________________________________________
float Bezier::findPeak(){
    if(_peak==0) _peak=this->findPeak2();
    float from  = _xV[0];
    float till  = _xV[_xV.size()-1];
    float where = 0;
    float value = 0;
    float tempvalue;
    printf ( "initial peak: %.2f \n",_peak ); 
   
    for ( int i=0;i<3 ;++i ){
    value=0;
   for ( float x=from;x<till;x+=_precision) {
        tempvalue=this->evaluate2(x);
        if(tempvalue>value) value=tempvalue, where=x; 
    }
    _peak=where;
    printf ( "%i-itteration of Bezier peak finding with peak %.2f \n",i+1,_peak );
   }
   
   
   
   
     

    return where;
}
//______________________________________________________________________________________
float Bezier::evaluate2(float x){
    if(_peak==0) _peak= this->findPeak2();
 int wherepeak = 0;

 for ( unsigned i=0;i<_xV.size() ;++i ) {
        if(signum(_peak-_xV[i])==-1){
            wherepeak=i;
            break;
            }
    }
int where=wherepeak-(int)(_order/2);
if(where<0) where=0;
_wherePeak=where;

float returnvalue=this->evaluate(x,_wherePeak);
return returnvalue;
}
//______________________________________________________________________________________
TMultiGraph* Bezier::plotBezier(){
TGraph* dataGraph=new TGraph(_xV.size(),&_xV[0],&_yV[0]);

TGraph* curveGraph=new TGraph();
double left=_xV[0];
double right=_xV[_xV.size()-1];
double step=(right-left)/(5*_xV.size());

for ( unsigned i=0;i<_xV.size()*5 ;++i ) {
curveGraph->SetPoint(i,_xV[0]+i*step,this->evaluate2(_xV[0]+i*step));
}
double max=0;
double where=0;
for ( unsigned i=0;i<_xV.size()*5 ;++i ) {
    if((double)this->evaluate2(_xV[0]+i*step)>max){
        max=this->evaluate2(_xV[0]+i*step),where=_xV[0]+i*step;
}
}
printf ( "Plot of Bezier function with peak in %.3f \n",where);
curveGraph->SetMarkerColor(2);
curveGraph->SetMarkerStyle(6);
TMultiGraph* curveDataGraph=new TMultiGraph();
curveDataGraph->Add(dataGraph);
curveDataGraph->Add(curveGraph);

return curveDataGraph;
}
//______________________________________________________________________________________
int Bezier::binomic(int n, int k){
    double temp = n;

    if(n<k||k<0||n<1){
        return 0;
    }else if(k==0||(k==n)){
        return 1;
    }else{
        for(int i=1;i<k;++i) temp*=(double)(n-i)/(i+1);

        return (int)temp;
    }
}
//______________________________________________________________________________________
template<typename T>
T  Bezier::signum(T n)
{
    if (n < 0) return -1;
    if (n > 0) return 1;
    return 0;
}






