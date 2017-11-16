#include <McSplitter.hh>

#include <iostream>

#include "marlin/Exceptions.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/SimCalorimeterHitImpl.h"

using namespace marlin;
using namespace lcio;
using namespace std;

//by Alexander Kaplan (kaplan@kip.uni-heidelberg.de)

McSplitter aMcSplitter;

/*----------------------------------------------------------------------------*/

McSplitter::McSplitter()
    : Processor("McSplitter"),
      _tot_eSum(0.0),
      _tot_nEvents(0)
{
    _description = "Splits sub components from SimCalorimeterHits";

    registerInputCollection( "SimCalorimeterHit",
                             "InputCollection",
                             "Name of the input SimCalorimeterHit collection",
                             _inColName,
                             std::string("hcalSD") );


    vector<string> initCompExample;
    initCompExample.push_back("mesons");
    initCompExample.push_back("13");   //muon +-
    initCompExample.push_back("-13");
    initCompExample.push_back("211");  //pion +-
    initCompExample.push_back("-211");
    initCompExample.push_back("321");  //K +-
    initCompExample.push_back("-321");

    registerOptionalParameter( "Component",
                               "Define shower sub-component to split",
                               _initComp,
                               initCompExample,
                               initCompExample.size() );
}

/*----------------------------------------------------------------------------*/

McSplitter::~McSplitter()
{
}

/*----------------------------------------------------------------------------*/

void McSplitter::init()
{
    vector<int> allPDGs;

    if( parameterSet( "Component" ) ) {

        unsigned index = 0 ;
        while( index < _initComp.size() ){

            string name( _initComp[ index++ ] ) ;
            
            cout<<"adding component \""<< name << "\" accepting PDG codes:";

            McSplitterComp *comp = new McSplitterComp( name );
            while( index <_initComp.size() && isInt( _initComp[index] ) ) {
                int pdg = toInt( _initComp[index] );
                cout<<" " << pdg;
                comp->addPDG( pdg );
                allPDGs.push_back( pdg );
                index++;
            }
            _components.push_back( comp );
            _eSums.push_back( 0.0 );
            cout<<endl;
        }
    }
    
    cout<<"adding component \"rest\" accepting all PDG codes but:";
    McInvertedComp *rest = new McInvertedComp( "rest" );
    for(unsigned i=0; i!=allPDGs.size(); i++ ) {
        cout<<" " << allPDGs[i];
        rest->addPDG( allPDGs[i] );
    }
    _components.push_back( rest );
    _eSums.push_back(0.0);
    cout<<endl;
}

/*----------------------------------------------------------------------------*/

void McSplitter::processEvent(LCEvent* evt)
{
    try {
        LCCollection *col  = evt->getCollection( _inColName );

        _tot_nEvents++;

        //prepare output collections for each component  
        vector<LCCollection*> compCols;
        for( unsigned i=0; i<_components.size(); i++ ) {
            //cout<<"preparing collection for component \""
            //    <<_components[i]->name()<<"\""<<endl;
            compCols.push_back( 
                new IMPL::LCCollectionVec( LCIO::SIMCALORIMETERHIT )
            );

            compCols[i]->parameters().setValue(
                "CellIDEncoding",
                col->parameters().getStringVal("CellIDEncoding")
                );
            compCols[i]->setFlag( col->getFlag() );
        }
       
        //loop over all input SimCalorimeterHits 
        for( int it=0; it!=col->getNumberOfElements(); it++ ) {
            SimCalorimeterHit *hit =
                dynamic_cast<SimCalorimeterHit*>( col->getElementAt(it) );
            if(!hit) {
                cout<<"McSplitter::proccessEvent():"<<endl
                    <<"  error, could not cast to SimCalorimeterHit"<<endl;
                break;
            }

            _tot_eSum += hit->getEnergy();

            //from MC contributions of input SimCalorimeterHit
            //build pseudo-SimCalorimeterHit for each component
            for( unsigned j=0; j<_components.size(); j++ ) {
                McSplitterComp* comp = _components[j];

                //copy cellID and pos from original SimCalorimeterHit
                comp->copyStuffFrom( hit );
                comp->resetEnergy();
           
                //loop over all MC contributions 
                int NMC = hit->getNMCContributions();
                //cout<<"hit "<<it<<" has "<<NMC<<" MC contributions"<<endl;
                for( int i=0; i<NMC; i++ ) {
                    //does the pdg match to the component?
                    if( comp->matches( hit->getPDGCont(i) ) ) {
                        //if so, sum up energy
                        comp->addEnergy( hit->getEnergyCont(i) );
                        _eSums[j] += hit->getEnergyCont(i);
                    }
                }
       
                //add pseudo-SimCalorimeterHit to component's collection 
                //if( comp->energy() > 0 ) {
                    compCols[j]->addElement(
                        comp->createSimCalorimeterHitImpl()
                    ); 
                //}
            }
        }

        //for each component add output collection with pseudo-
        //SimCalorimeterHits to event
        for( unsigned i=0; i<_components.size(); i++ ) {
            //cout<<"adding output collection \""
            //  << _inColName + "_" + _components[i]->name()
            //  <<"\" for component \""
            //  <<_components[i]->name()<<"\""<<endl;
            evt->addCollection( compCols[i],
                                _inColName + "_" + _components[i]->name() );
        }

    } catch ( DataNotAvailableException err ) {
        cout<<"McSplitter::proccessEvent():"
            <<" WARNING: Collection \""
            <<_inColName
            <<"\" not available in event "<<evt->getEventNumber()<<endl
            <<"DataNotAvailableException::what() == "<<err.what()<<endl;

        cout<<"sorry this seems to be a noise only event (for the hcal at least)"
            <<"cannot handle such events -> skipping..."<<endl;

        throw SkipEventException(this);
    }
}

/*----------------------------------------------------------------------------*/

void McSplitter::end()
{
    cout<<"----------------------------------------------------------------"
        <<endl<<" total number of events: \t\t\t"<<_tot_nEvents<<endl
        <<" avg. energy deposited in total: \t\t"
        << _tot_eSum / _tot_nEvents<<endl;

    for(unsigned i=0; i<_components.size(); i++) {
        cout<<" avg. energy deposoted in "<<_components[i]->name()<<": \t\t"
            << _eSums[i] / _tot_nEvents << "\t"
            << "("  << _eSums[i] / _tot_eSum * 100 << "%)"<<endl;
    }
    cout<<"----------------------------------------------------------------"
        <<endl;
}

/*----------------------------------------------------------------------------*/
int McSplitter::toInt( const std::string s )
{
    stringstream ss;
    int i;

    ss<<s; ss>>i;
    return i;
}

/*----------------------------------------------------------------------------*/

bool McSplitter::isInt( string s )
{
    for(unsigned i=0; i<s.length(); i++) {
        switch( s[i] ) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '+':
            case '-':
                break;
            default:
                return false;
        }
    }
    return true;
}
