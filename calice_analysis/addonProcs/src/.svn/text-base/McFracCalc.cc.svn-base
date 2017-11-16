#include <McFracCalc.hh>

#include <cmath>
#include <iostream>

#include "marlin/Exceptions.h"
#include "EVENT/CalorimeterHit.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/SimCalorimeterHitImpl.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "UTIL/CellIDEncoder.h"
#include "UTIL/CellIDDecoder.h"

#include "FastCaliceHit.hh"

using namespace std;
using namespace lcio;
using namespace marlin;
using namespace CALICE;

//by Alexander Kaplan (kaplan@kip.uni-heidelberg.de)

McFracCalc aMcFracCalc;

/*----------------------------------------------------------------------------*/

McFracCalc::McFracCalc()
    : Processor("McFracCalc"),
      _tot_eSum(0.0),
      _tot_nEvents(0),
      _compFracEncStr("")
{
    _description = "Creates fully digitized CalorimeterHit collections"
                   " containing only the fraction of specified sub components";

    registerProcessorParameter( "inputColName",
                                "Name of the input collection "
                                " containing digitized CalorimeterHits",
                                _inColName,
                                std::string("AhcCalorimeter_Hits") );

    registerProcessorParameter( "noiseColName",
                                "Name of the noise collection "
                                " containing calibrated CalorimeterHits",
                                _noiseColName,
                                std::string("AhcNoise1") );

    registerProcessorParameter( "afterDigiColName",
                                "Name of the collection containing Ahcal "
                                "CalorimeterHits after Digitization",
                                _afterDigiColName,
                                std::string("hcalAfterDigitization") );

    registerProcessorParameter( "compColBase",
                                "Base of the input collection names"
                                " containing single shower components.",
                                _inColBase,
                                std::string("hcalAfterGanging") );

    registerProcessorParameter( "outputColBase",
                                "Base of the output collection names.",
                                _outColBase,
                                std::string("AhcCalorimeter_Hits") );

    registerProcessorParameter( "mipCut",
                                "mipCut (only applied to input collection)",
                                _mipCut,
                                0.5f );

    vector<string> CompExample;
    CompExample.push_back("mesons");

    registerOptionalParameter( "Component",
                               "Define shower sub-component to split",
                               _components,
                               CompExample,
                               CompExample.size() );
}

/*----------------------------------------------------------------------------*/

McFracCalc::~McFracCalc()
{
}

/*----------------------------------------------------------------------------*/

void McFracCalc::init()
{
    vector<int> allPDGs;

    cout<<"input Collection:"<<_inColName<<endl
        <<"output Collection base:"<<_outColBase<<endl;

    if( parameterSet( "Component" ) ) {

        unsigned index = 0 ;
        while( index < _components.size() ){

            string name( _components[ index++ ] ) ;
            
            cout<<"adding component \""<< name << "\", reading from input "
                <<"collection "<<_inColBase+"_"+name<<endl;

            _eSums.push_back( 0.0 );
        }
    }
    cout<<"adding component \""<< "rest" << "\", reading from input "
        <<"collection "<<_inColBase+"_rest"<<endl;

    _components.push_back(string("rest"));
    _eSums.push_back(0.0);

    _eSumNoise = 0.0;
}

/*----------------------------------------------------------------------------*/

void McFracCalc::processEvent(LCEvent* evt)
{
    //cout<<"McFracCalc::processEvent(): ev # "<<evt->getEventNumber()<<endl;

    //delete data from last event
    _noiseFractions.clear();
    for( unsigned i=0; i<_compFractions.size(); i++ ) {
        _compFractions[i].clear();
    }
    _compFractions.clear();

    calcNoiseFractions(evt);
    calcCompFractions(evt);

    generatePseudoHits(evt);

}

/*----------------------------------------------------------------------------*/

void McFracCalc::calcNoiseFractions( lcio::LCEvent* evt )
{
    //Save Noise Energies 
    LCCollection* noiseCol = evt->getCollection( _noiseColName );
    if( noiseCol ) {
        int nNoiseHits = noiseCol->getNumberOfElements();

        for(int i=0; i!=nNoiseHits; i++) {
            FastCaliceHit * noiseHit =
                dynamic_cast<FastCaliceHit*>( noiseCol->getElementAt(i) );
            if(!noiseHit) {
                cout<<"errror: could not get noise Hit "
                    <<i<<" from collection"<<endl;
                return;
            }
            
            //the FastCaliceHit::getCellID() returns a HcalTileIndex-compatible
            //index
            unsigned key = noiseHit->getCellID();
            _noiseFractions[key] = abs(noiseHit->getEnergyValue());
        } 
    }

    //Get AhcalHits after digi (ad) (superset of noise hits)
    map<unsigned, float> adE;
    LCCollection* digiCol = evt->getCollection( _afterDigiColName );
    if( digiCol ) {
        int nDigiHits = digiCol->getNumberOfElements();

        for(int i=0; i!=nDigiHits; i++) {
            FastCaliceHit* digiHit =
                dynamic_cast<FastCaliceHit*>( digiCol->getElementAt(i) );
            if( !digiHit ) {
                cout<<"errror: could not get digi Hit "
                    <<i<<" from collection"<<endl;
                return;
            }

            //the FastCaliceHit::getCellID() returns a HcalTileIndex-compatible
            //index
            unsigned key = digiHit->getCellID();
            float energy = digiHit->getEnergyValue();

            adE[key] = energy;

            if( energy != 0 ) {
               _noiseFractions[ key ] /= energy;
            } else {
                if( _noiseFractions[key] != 0 ) {
                    cout<<"we have a noiseHit, key==0x"<<hex<<key<<dec
                        <<", energy=="<< _noiseFractions[key]
                        <<" but digiHit.energy=="<<energy
                        <<" sth. wrong here!"<<endl;
                }
            }
        } 
    }
    //paranoid extra check:
    map<unsigned,float>::iterator it;
    for(it=_noiseFractions.begin(); it!=_noiseFractions.end(); it++) {
        if( adE[it->first] == 0 ) {
            if( _noiseFractions[it->first] != 0 ) {
                cout<<"xtra check: we have a noiseHit, key="
                    <<hex<<it->first<<dec
                    <<" but hit energy == "<<adE[it->first]
                    <<" sth. wrong here!"<<endl;
            }
        }
    }
}

/*----------------------------------------------------------------------------*/

void McFracCalc::calcCompFractions( lcio::LCEvent* evt )
{
    //get collection containing all components
    //cout<<"getting collection "<< _inColBase<<endl;
    LCCollection* allCompCol = evt->getCollection( _inColBase );
    int nAcc = allCompCol->getNumberOfElements();
    //        cout<<"containing "<<nAcc<<"hits with normalization" <<endl;

    _compFracEncStr =
        allCompCol->getParameters().getStringVal("CellIDEncoding");
    BitField64 compFracEnc( _compFracEncStr );
    //cout<<"BitField64 compFracEnc( \""<<_compFracEncStr<<"\" );"<<endl;
        
    vector< map< lcio::long64, float > * > fractions;
    //for each component calculate fraction
    for( unsigned i=0; i<_components.size(); i++ ) {
        LCCollection* compCol =
            evt->getCollection( _inColBase + "_" + _components[i] );
        int nCc = compCol->getNumberOfElements();
        
        CellIDDecoder<CalorimeterHit> compDec(compCol);

        if( nAcc != nCc ) {
            cout<<"nAcc=="<<nAcc<<" != nCc=="<<nCc<<" here is sth. wrong!"
                <<endl;
            break;
        }

        map<long64, float> hitFractions;
        //loop over all hits
        for( int j=0; j<nCc; j++ ) {
            CalorimeterHit* allCompHit =
                dynamic_cast<CalorimeterHit*>(allCompCol->getElementAt(j));
            if(! allCompHit ) break; 

            CalorimeterHit* compHit =
                dynamic_cast<CalorimeterHit*>(compCol->getElementAt(j));
            if(! compHit ) break;

            if(    allCompHit->getCellID0() != compHit->getCellID0()
                || allCompHit->getCellID1() != compHit->getCellID1() ) {
                cout<<"different cell ids -> sth. wrong here!"<<endl;
            }

            float hitFrac
                = compHit->getEnergy() / allCompHit->getEnergy();
            if( hitFrac > 1 ) {
                cout<<"hitFrac = "<<hitFrac<<" > 1 sth. wrong here!"<<endl;
            }
            if( hitFrac < 0 ) {
                cout<<"hitFrac = "<<hitFrac<<" < 0 sth. wrong here!"<<endl;
            }

            compFracEnc["I"] = compDec(compHit)["I"].value();
            compFracEnc["J"] = compDec(compHit)["J"].value();
            compFracEnc["K"] = compDec(compHit)["K"].value();
            long64 key=compFracEnc.getValue();
            
            //int I=compDec(compHit)["I"].value();
            //int J=compDec(compHit)["J"].value();
            //int K=compDec(compHit)["K"].value();
            //cout<<"key for ("<<I<<","<<J<<","<<K<<")==0x"<<hex<<key<<dec<<endl;
            //cout<<"hitFractions["<<i<<"][0x"<<hex<<key<<dec<<"] = "
            //    <<hitFrac<<endl;

            hitFractions[ key ] = hitFrac;    

            //cout<<"calculating "<<_components[i]<<"-fraction for hit 0x"
            //    <<hex<<key<<" to be "<< compHit->getEnergy() <<" / "
            //    <<allCompHit->getEnergy()<<" = "<<hitFrac<<endl;
        }
        _compFractions.push_back( hitFractions );
    }
}

/*----------------------------------------------------------------------------*/

void McFracCalc::generatePseudoHits( lcio::LCEvent* evt )
{
    try {
        LCCollection *col  = evt->getCollection( _inColName );
        CellIDDecoder<CalorimeterHit> deco(col);

        //prepare output collections for each component  
        vector<LCCollectionVec*> compCols;
        for( unsigned i=0; i<_components.size(); i++ ) {
            //cout<<"preparing collection for component \""
            //    <<_components[i]<<"\""<<endl;
            compCols.push_back( 
                new IMPL::LCCollectionVec( LCIO::CALORIMETERHIT )
            );

            //copy stuff from input collection
            compCols[i]->parameters().setValue(
                "CellIDEncoding",
                col->parameters().getStringVal("CellIDEncoding")
                );
            compCols[i]->setFlag( col->getFlag() );
            compCols[i]->setSubset(false);
            //cout<<"isSubset()=="<<compCols[i]->isSubset()<<endl;
        }

        //prepare noise output collection
        LCCollectionVec* outNoiseCol =
            new LCCollectionVec( LCIO::CALORIMETERHIT );
        outNoiseCol->parameters().setValue(
            "CellIDEncoding",
            col->parameters().getStringVal("CellIDEncoding")
        );
        outNoiseCol->setFlag( col->getFlag() );
        outNoiseCol->setSubset(false);

        //prepare all-afterMipCut output collection
        LCCollectionVec* outAllCol =
            new LCCollectionVec( LCIO::CALORIMETERHIT );
        outAllCol->parameters().setValue(
            "CellIDEncoding",
            col->parameters().getStringVal("CellIDEncoding")
        );
        outAllCol->setFlag( col->getFlag() );
        outAllCol->setSubset(true);  //save some space here

        BitField64 compFracEnc = BitField64( _compFracEncStr );
       
        //loop over all input CalorimeterHits 
        for( int it=0; it!=col->getNumberOfElements(); it++ ) {
            CalorimeterHit *hit =
                dynamic_cast<CalorimeterHit*>( col->getElementAt(it) );
            if(!hit) {
                cout<<"McFracCalc::proccessEvent():"<<endl
                    <<"  error, could not cast to CalorimeterHit"<<endl;
                return;
            }

            if( hit->getEnergy() < _mipCut ) {
                continue;
            }

            _tot_eSum += hit->getEnergy();

            compFracEnc["I"] = deco(hit)["I"].value();
            compFracEnc["J"] = deco(hit)["J"].value();
            compFracEnc["K"] = deco(hit)["K-1"].value()+1;
            long64 compKey   = compFracEnc.getValue();

            //int I=deco(hit)["I"].value();
            //int J=deco(hit)["J"].value();
            //int K=deco(hit)["K-1"].value()+1;
            //cout<<"apply key for ("<<I<<","<<J<<","<<K<<")==0x"
            //    <<hex<<compKey<<dec<<endl;

            unsigned noiseKey = hit->getCellID1();

            //from MC fractions of input SimCalorimeterHit
            //build pseudo-CalorimeterHit for each component
            for( unsigned j=0; j<_components.size(); j++ ) {
                CalorimeterHitImpl *newHit = new CalorimeterHitImpl;
              
                float hitCompFrac;

                hitCompFrac  = _compFractions[j][compKey];
                //cout<<"hitCompFrac = _compFractions["<<j<<"]["<<compKey
                //   <<"]="<<hitCompFrac<<endl;
                
                //cout<<"hitCompFrac *= ( 1 - _noiseFractions["<<noiseKey
                //    <<"]==( 1 - "<<_noiseFractions[noiseKey]<<")"
                //    <<endl;

                hitCompFrac *= ( 1 - _noiseFractions[noiseKey] );

                //cout<<"hitCompFrac="<<hitCompFrac<<endl;

                //copy stuff
                newHit->setCellID0( hit->getCellID0() );
                newHit->setCellID1( hit->getCellID1() );
                newHit->setTime( hit->getTime() );
                float pos[3];
                pos[0] = hit->getPosition()[0]; 
                pos[1] = hit->getPosition()[1]; 
                pos[2] = hit->getPosition()[2]; 
                newHit->setPosition(pos );
                newHit->setType( hit->getType() );
                newHit->setRawHit( hit->getRawHit() );
               
                //calculate & set energy fraction
                float energy = hit->getEnergy();
                newHit->setEnergy( energy * hitCompFrac );
                
                //this is definitly no correct error handling... ->todo!
                //newHit->setEnergyError( hit->getEnergyError() * hitFrac );
                newHit->setEnergyError( 0 );

                //cout<<_components[j]<<"-fraction of hit 0x"<<hex<<key<<": "
                //    <<" has energy = " << hitFrac 
                //    << " * " << hit->getEnergy()
                //    << " = " << newHit->getEnergy()<<endl;

                _eSums[j] += newHit->getEnergy();
                
                //add new hit to collection coreseponding to component
                compCols[j]->addElement( newHit ); 
            }

            //-- begin: build pseudo-CalorimeterHit for noise component also
            CalorimeterHitImpl *newHit = new CalorimeterHitImpl;

            float hitNoiseFrac = _noiseFractions[noiseKey];

            //copy stuff
            newHit->setCellID0( hit->getCellID0() );
            newHit->setCellID1( hit->getCellID1() );
            newHit->setTime( hit->getTime() );
            float pos[3];
            pos[0] = hit->getPosition()[0]; 
            pos[1] = hit->getPosition()[1]; 
            pos[2] = hit->getPosition()[2]; 
            newHit->setPosition(pos );
            newHit->setType( hit->getType() );
            newHit->setRawHit( hit->getRawHit() );

            //calculate & set energy fraction
            float energy = hit->getEnergy();
            newHit->setEnergy( energy * hitNoiseFrac );

            //this is definitly no correct error handling... ->todo!
            //newHit->setEnergyError( hit->getEnergyError() * hitFrac );
            newHit->setEnergyError( 0 );

            //cout<<"noise-fraction of hit 0x"<<hex<<key<<": "
            //    <<" has energy = " << hitFrac 
            //    << " * " << hit->getEnergy()
            //    << " = " << newHit->getEnergy()<<endl;
            _eSumNoise += newHit->getEnergy();

            //add new hit to collection coreseponding to component
            outNoiseCol->addElement( newHit ); 
            //-- end: build pseudo-CalorimeterHit for noise component also


            //-- begin: build pseudo-CalorimeterHit for all after mipCut
            outAllCol->addElement( hit ); 
            //-- end: build pseudo-CalorimeterHit for all after mipCut
  
        }

        //for each component add output collection with pseudo-
        //CalorimeterHits to event
        // and clean up!
        for( unsigned i=0; i<_components.size(); i++ ) {
            //cout<<"  adding output collection \""
            //  << _inColName + "_" + _components[i]
            //  <<"\" for component \""
            //  <<_components[i]<<"\" containing "
            //  <<compCols[i]->getNumberOfElements()<<" elements"
            //  <<endl;
        
            evt->addCollection( compCols[i],
                                _outColBase + "_" + _components[i] );
        }

        //also add the noise component:
        //cout<<"  adding output collection \""
        //    << _inColName + "_noise\" for component noise"
        //    <<" containing "<<outNoiseCol->getNumberOfElements()<<" elements"
        //    <<endl;
        evt->addCollection( outNoiseCol, _inColName + "_noise" );

        //and add the "all"-afterMipCut component:
        evt->addCollection( outAllCol, _inColName + "_all" );

    } catch ( DataNotAvailableException err ) {
        cout<<"McFracCalc::proccessEvent():"
            <<"DataNotAvailableException::what() == "<<err.what()<<endl;

        cout<<"sorry this seems to be a noise only event (for the hcal at least)"
            <<"cannot handle such events -> skipping..."<<endl;

        throw marlin::SkipEventException(this);
    }

    _tot_nEvents++;
}

/*----------------------------------------------------------------------------*/

void McFracCalc::end()
{
    cout<<"----------------------------------------------------------------"
        <<endl<<" total number of events: \t\t\t"<<_tot_nEvents<<endl
        <<" avg. energy deposited in total: \t\t"
        << _tot_eSum / _tot_nEvents<<endl;

    for(unsigned i=0; i<_components.size(); i++) {
        cout<<" avg. energy deposited in "<<_components[i]<<": \t\t"
            << _eSums[i] / _tot_nEvents << "\t"
            << "("  << _eSums[i] / _tot_eSum * 100 << "%)"<<endl;
    }
    cout<<" avg. energy deposited in noise: \t\t"
        << _eSumNoise / _tot_nEvents << "\t"
        << "("  << _eSumNoise / _tot_eSum * 100 << "%)"<<endl;
    cout<<"----------------------------------------------------------------"
        <<endl;
}


