/*
 * HCalTrackFilter.cpp
 *
 *  Created on: 19.09.2011
 *      Author: weuste
 */

#include "HCalTrackFilter.hh"

#include <float.h>
#include <cmath>

#include <IMPL/ClusterImpl.h>
#include <IMPL/LCCollectionVec.h>

#include <vector>
#include <iostream>
#include <iomanip>
#include <set>
#include <stdlib.h>



#ifdef ROOT_OUT
#include <TH2D.h>
#include <TPolyLine.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TFile.h>
#endif

using namespace std;

HCalTrackFilter aHCalTrackFilter;

HCalTrackFilter::HCalTrackFilter() : Processor("HCalTrackFilter")
{

//	registerProcessorParameter(
//			"cellNeighboursProcessor",
//			"the name of the CellNeighboursProcessor for geometry information",
//			_cellNeighbourProcessorName,
//			std::string("AhcCellNeighboursProcessor"));

	registerProcessorParameter(
			"cellDescriptionProcessor",
			"the name of the CellDescriptionProcessor for geometry information",
			_cellDescriptionProcessorName,
			std::string("AhcCellDescriptionProcessor"));

	registerProcessorParameter(
			"maxNrRejectedHits",
			"The maximum number of hits we are allowed to filter out before the entire track is rejected",
			_maxNrRejectedHits,
			2
			);

	registerProcessorParameter(
			"minNrOfHitsPerTrack",
			"The minimum number of hits a track has to have after the filtering, otherwise the entire track is rejected",
			_minNumberOfHits,
			3
			);

	registerInputCollection(
			LCIO::CLUSTER,
			"TrackCollection",
			"Name of the LCIO::CLUSTER Collection with the identified tracks in the Calorimeter",
			_lccTrackClusterName,
			std::string("AhcTracksNN"));

	registerOutputCollection(
			LCIO::CLUSTER,
			"FilteredTrackCollection",
			"Name of the LCIO::CLUSTER Collection with the identified tracks in the Calorimeter after the filtering",
			_lccFilteredTrackClusterName,
			std::string("AhcTracksNN_filtered"));
}

HCalTrackFilter::~HCalTrackFilter()
{

}

void HCalTrackFilter::init()
{
	streamlog_out(MESSAGE4) << "Init Processor HCalTrackFilter" << endl;

	printParameters();

	// connect to CALICE geometry procs

    _cellDescriptions = CALICE::CellDescriptionProcessor::getCellDescriptions(_cellDescriptionProcessorName);
	if (!_cellDescriptions)
		throw Exception("We have no CALICE::CellDescriptionProcessor");

//    _cellNeighbours = CALICE::CellNeighboursProcessor::getNeighbours(_cellNeighbourProcessorName);
//	if (!_cellNeighbours)
//		throw Exception("We have no CALICE::CellNeighboursProcessor");

	_statsNrHitsRejected = 0;
	_statsNrTracksRejected = 0;
	_statsNrTracks = 0;
	_statsNrEvents = 0;

	#ifdef ROOT_OUT
	_rootFile = new TFile("debug.root", "RECREATE");
	#endif

	streamlog_out(MESSAGE4) << "Successfully loaded HCalTrackFilter" << endl;
}


void HCalTrackFilter::processEvent(EVENT::LCEvent* evt)
{
	try
	{
		LCCollection* lccTracks = evt->getCollection(_lccTrackClusterName);
		_cellDescriptions->getDecoder()->setCellIDEncoding( lccTracks->getParameters().getStringVal("CellIDEncoding") );
		LCCollectionVec* lccFilteredTracks = new LCCollectionVec(LCIO::CLUSTER);

		streamlog_out(DEBUG) << "===========================================================" << endl
							 << "New event: " << _statsNrEvents << endl;

		for (int i = 0; i < lccTracks->getNumberOfElements(); ++i)
		{
			EVENT::Cluster* filteredTrack = this->filterTrackHough(dynamic_cast<EVENT::Cluster*>(lccTracks->getElementAt(i)));

			_statsNrTracks++;

			if (filteredTrack)
				lccFilteredTracks->addElement(filteredTrack);

			#ifdef ROOT_OUT
			saveRootTrackDiffCanvas(dynamic_cast<EVENT::Cluster*>(lccTracks->getElementAt(i)), filteredTrack);
			#endif
		}

		_statsNrEvents++;

		evt->addCollection(lccFilteredTracks, _lccFilteredTrackClusterName);

	}
	catch (DataNotAvailableException &ignored)	{ }
}

#ifdef ROOT_OUT
void HCalTrackFilter::saveRootTrackDiffCanvas(EVENT::Cluster *preFilterTrack, EVENT::Cluster *postFilterTrack)
{
	TGraphErrors gxPre, gxPost;
	TGraphErrors gyPre, gyPost;
	gxPre.SetTitle(TString::Format("unfiltered Track %d @ Event %d;z[mm];x[mm]", _statsNrTracks, _statsNrEvents));
	gyPre.SetTitle(TString::Format("unfiltered Track %d @ Event %d;z[mm];y[mm]", _statsNrTracks, _statsNrEvents));
	gxPost.SetTitle(TString::Format("filtered Track %d @ Event %d;z[mm];x[mm]", _statsNrTracks, _statsNrEvents));
	gyPost.SetTitle(TString::Format("filtered Track %d @ Event %d;z[mm];y[mm]", _statsNrTracks, _statsNrEvents));


	// fill the vectors
	CalorimeterHitVec hitsPre = preFilterTrack->getCalorimeterHits();	// copy constructor --> no 'const'
	for (unsigned n=0; n<hitsPre.size(); n++)
	{

		EVENT::CalorimeterHit* hit = hitsPre.at(n);

		CALICE::CellDescription* d = _cellDescriptions->getByCellID(hit->getCellID0());

		gxPre.SetPoint(n, 		d->getZ(), 	d->getX());
		gxPre.SetPointError(n, 2.5, 		d->getSizeX()/2);
		gyPre.SetPoint(n, 		d->getZ(), 	d->getY());
		gyPre.SetPointError(n, 2.5, 		d->getSizeY()/2);

	}

	// fill the vectors
	if (postFilterTrack != NULL)
	{
		CalorimeterHitVec hitsPost = postFilterTrack->getCalorimeterHits();	// copy constructor --> no 'const'
		for (unsigned n=0; n<hitsPost.size(); n++)
		{

			EVENT::CalorimeterHit* hit = hitsPost.at(n);

			CALICE::CellDescription* d = _cellDescriptions->getByCellID(hit->getCellID0());

			gxPost.SetPoint(n, 		d->getZ(), 	d->getX());
			gxPost.SetPointError(n, 2.5, 		d->getSizeX()/2);
			gyPost.SetPoint(n, 		d->getZ(), 	d->getY());
			gyPost.SetPointError(n, 2.5, 		d->getSizeY()/2);

		}
	}

	TCanvas* c = new TCanvas(TString::Format("cTracks_%d_%d", _statsNrEvents,_statsNrTracks),
							 TString::Format("Event %d Track %d", _statsNrEvents,_statsNrTracks));
	c->Divide(2,2);
	c->cd(1);
	gxPre.Draw("ap");
	c->cd(2);
	gyPre.Draw("ap");
	if (postFilterTrack)
	{
		c->cd(3);
		gxPost.Draw("ap");
		c->cd(4);
		gyPost.Draw("ap");
	}

	c->Draw();
	_rootFile->WriteTObject(c);
}
#endif

void HCalTrackFilter::end()
{
	#ifdef ROOT_OUT
	_rootFile->Close();
	#endif
	streamlog_out(MESSAGE) << "Statistics:" << endl;
	streamlog_out(MESSAGE) << "  Nr events: " << _statsNrEvents << endl;
	streamlog_out(MESSAGE) << "  Nr tracks (rejected): " << _statsNrTracks << " (" << _statsNrTracksRejected << ")" <<  endl;
	streamlog_out(MESSAGE) << "  Nr hits rejected (per track): " << _statsNrHitsRejected << " (" << 1.0* _statsNrHitsRejected / _statsNrTracks << ")" <<  endl;
}


void HCalTrackFilter::divideAndCheckHoughSpace(int mMin, int tMin, int mMax, int tMax)
{


//	if (_recursiveCounter > 25)
//	{
//		streamlog_out(MESSAGE) << "Maximum depth arrived - returning" << endl;
//		_recursiveCounter--;
//
//		#ifdef ROOT_OUT
//		_rootFile->WriteTObject(_cHough);
//		_rootFile->Write();
//		_rootFile->Close();
//		exit(1);
//		#endif
//	}


	_recursiveCounter++;

	// calculate the middle of this quadrant
	int mMidIdx;
	int tMidIdx;
	double mMidVal;
	double tMidVal;

	bool maxAccuracy = false;
	if (mMax - mMin == 1 || tMax - tMin == 1)
	{
		mMidIdx = -1;
		tMidIdx = -1;

		mMidVal = (_houghSpaceBinningM[mMax] - _houghSpaceBinningM[mMin]) / 2 + _houghSpaceBinningM[mMin];
		tMidVal = (_houghSpaceBinningT[tMax] - _houghSpaceBinningT[tMin]) / 2 + _houghSpaceBinningT[tMin];

		maxAccuracy = true;
	}
	else
	{
		mMidIdx = (mMax - mMin) / 2 + mMin;
		tMidIdx = (tMax - tMin) / 2 + tMin;

		mMidVal = _houghSpaceBinningM[mMidIdx];
		tMidVal = _houghSpaceBinningT[tMidIdx];

		maxAccuracy = false;
	}

	// helper vars for the loop
	int mIdx[3] = {mMin, mMidIdx, mMax};
	int tIdx[3] = {tMin, tMidIdx, tMax};
	double mVal[3] = {_houghSpaceBinningM[mMin], mMidVal, _houghSpaceBinningM[mMax]};
	double tVal[3] = {_houghSpaceBinningT[tMin], tMidVal, _houghSpaceBinningT[tMax]};

	streamlog_out(DEBUG2) << "New divide of Hough Space (indices): "
			<< setw(12) << mMin << " /"
			<< setw(12) << mMax << "  "
			<< setw(12) << tMin << " /"
			<< setw(12) << tMax
			<< "  ==>  "
			<< setw(12) << mMin << " /"
			<< setw(12) << mMidIdx << " /"
			<< setw(12) << mMax << "  "
			<< setw(12) << tMin << " /"
			<< setw(12) << tMidIdx << " /"
			<< setw(12) << tMax
			<< endl;

	// loop over all 4 quadrants
	for (int i=0; i<2; i++)
	for (int j=0; j<2; j++)
	{
		double mLo = mVal[i];
		double mHi = mVal[i+1];
		double tLo = tVal[i];
		double tHi = tVal[i+1];

		#ifdef ROOT_OUT
		TPolyLine* rect = new TPolyLine(4);
		rect->SetPoint(0, mLo, r2t(tLo));
		rect->SetPoint(1, mHi, r2t(tLo));
		rect->SetPoint(2, mHi, r2t(tHi));
		rect->SetPoint(3, mLo, r2t(tHi));
		rect->SetLineWidth(3);
		rect->SetLineStyle(kDashed);
		rect->Draw("same");
		#endif

		streamlog_out(DEBUG1) << "Checking rect"
				<< setw(12) << mLo << "/"
				<< setw(12) << tLo << "/"
				<< setw(12) << mHi << "/"
				<< setw(12) << tHi << endl;

		vector < HoughBand* > houghBandsPartiallyInQuadrant;
		vector < HoughBand* > houghBandsCompleteInQuadrant;

		// check all houghbands
		for (unsigned n=0; n<_allHoughBands.size(); n++)
		{
			HoughBand* b = _allHoughBands.at(n);

			streamlog_out(DEBUG0) << "Checking if Band " << b << " is in the Quadrant: ";

			EHoughBandInRect e = b->isInQuadrant(mLo, tLo, mHi, tHi);
			if (e == LineCrossing)
			{
				houghBandsPartiallyInQuadrant.push_back(b);
				streamlog_out(DEBUG0) << "LineCrossing" << endl;
			}
			else if (e == RectIncludedInBand)
			{
				houghBandsCompleteInQuadrant.push_back(b);
				streamlog_out(DEBUG0) << "RectIncludedInBand" << endl;
			}
			else
				streamlog_out(DEBUG0) << "NotInRect" << endl;

		}

		// now we know how many of our hough bands are in this rect/quadrant, and if they contain this rect completely,
		// or if they just cross it
		unsigned allIntersections = houghBandsCompleteInQuadrant.size() + houghBandsPartiallyInQuadrant.size();

		streamlog_out(DEBUG1) << "Intersections: (part/complete/tot)"
				<< "(" << setw(3) << houghBandsPartiallyInQuadrant.size()
				<< "/" << setw(3) << houghBandsCompleteInQuadrant.size()
				<< "/" << setw(3) << allIntersections
				<< ")" << endl;


		// if with all houghbands in this quadrant we are not able to beat the biggest interesection so far: don't try
		if (allIntersections <= _biggestIntersection.size() || allIntersections < 2)
			continue;

		// if we cannot divide this quadrant any more to gain more detail
		// (i.e. all bands that intersect with this rect contain it completely)
		// don't divide it anymore, just save the result
		if (houghBandsPartiallyInQuadrant.size() < 2 || maxAccuracy)
		{
			_biggestIntersection.clear();
			streamlog_out(DEBUG2) << "New Peak (" << houghBandsPartiallyInQuadrant.size() + houghBandsCompleteInQuadrant.size() << "): ";
			for (unsigned n=0; n<houghBandsCompleteInQuadrant.size(); n++)
			{
				_biggestIntersection.push_back(houghBandsCompleteInQuadrant.at(n));
				streamlog_out(DEBUG2) << setw(4) << houghBandsCompleteInQuadrant.at(n);
			}
			for (unsigned n=0; n<houghBandsPartiallyInQuadrant.size(); n++)
			{
				_biggestIntersection.push_back(houghBandsPartiallyInQuadrant.at(n));
				streamlog_out(DEBUG2) << setw(4) << houghBandsPartiallyInQuadrant.at(n);
			}

			streamlog_out(DEBUG2) << endl;


			continue;
		}

		// at least one of the hough bands intersects with this quadrant only partially (i.e. this quadrant is not contained completely)
		// we need to divide it more
		divideAndCheckHoughSpace(mIdx[i], tIdx[i], mIdx[i+1], tIdx[i+1]);
	}


	_recursiveCounter--;
}


EVENT::Cluster* HCalTrackFilter::filterTrackHough(EVENT::Cluster *track)
{

	// the xyz coord of the tiles plus their sizes (in the xy plane)
	std::vector< double > x_3d;
	std::vector< double > y_3d;
	std::vector< double > z_3d;
	std::vector< double > size;

	// fill the vectors
	CalorimeterHitVec hits = track->getCalorimeterHits();	// copy constructor --> no 'const'

	streamlog_out(DEBUG0) << "Tiles:" << endl;
	for (unsigned n = 0; n < hits.size(); ++n)
	{
		EVENT::CalorimeterHit* hit = hits.at(n);

		CALICE::CellDescription* d = _cellDescriptions->getByCellID(hit->getCellID0());

		// convert to meters
		x_3d.push_back(d->getX()/1000);
		y_3d.push_back(d->getY()/1000);
		z_3d.push_back(d->getZ()/1000);
		size.push_back(d->getSizeX()/2/1000);


		streamlog_out(DEBUG0)
				<< setw(9) << d->getX() << " / "
				<< setw(9) << d->getY() << " / "
				<< setw(9) << d->getZ() << " / "
				<< setw(9) << d->getSizeX()/2 << endl;

	}
	streamlog_out(DEBUG0) << endl;


	// Will do a Hough Transformation
	// convert everything to the nomenclature of y = mx + t
	std::vector< double >& x = z_3d;
	std::vector< std::vector<double>* > ys;		// we have to check both the x-z (3d) and the y-z (3d). This translates to z(3d) being our x in y=mx+t, and x(3d) and y(3d) both are our y in y=mx+t
	ys.push_back(&x_3d);
	ys.push_back(&y_3d);

	// do this for both 'y'
	for (unsigned n=0; n<ys.size(); n++)
	{
		streamlog_out(DEBUG1) << "Calculating intersections in Hough-Space for " << (n==0 ? "x" : "y") << ":z" << endl;
		std::vector<double>& y = *ys[n];


		#ifdef ROOT_OUT
		_cHough = new TCanvas(TString::Format("cHoughSpace_%d_%d_%d", _statsNrEvents, _statsNrTracks, n),
							  TString::Format("HoughSpace Evt:%d Track:%d x/y:%d", _statsNrEvents, _statsNrTracks, n));
		#endif


		// convert each tile into a hough band (usually it is a line, but here we have tiles with a certain size. This is then a band)
		#ifdef ROOT_OUT
		const char* drawOpt = "";
		#endif

		for (unsigned i=0; i<x.size(); i++)
		{
			HoughBand* h = new HoughBand(x[i], y[i], size[i], i);
			_allHoughBands.push_back(h);

			#ifdef ROOT_OUT
			TF1* upper = new TF1(TString::Format("fUpper_%d_%d_%d_%d", _statsNrEvents, _statsNrTracks, n, i), "pol1", -7, 7);
			upper->SetParameter(1, h->getUpperLine().x());
			upper->SetParameter(0, h->getUpperLine().y());
			upper->SetLineColor(i+1);
			upper->Draw(drawOpt);
			upper->SetTitle(TString::Format("fUpper_%d_%d_%d_%d", _statsNrEvents, _statsNrTracks, n, i));

			drawOpt = "same";

			TF1* lower = new TF1(TString::Format("fLower_%d_%d_%d_%d", _statsNrEvents, _statsNrTracks, n, i), "pol1", -7, 7);
			lower->SetParameter(1, h->getLowerLine().x());
			lower->SetParameter(0, h->getLowerLine().y());
			lower->SetLineColor(i+1);
			lower->Draw(drawOpt);
			lower->SetTitle(TString::Format("fLower_%d_%d_%d_%d", _statsNrEvents, _statsNrTracks, n, i));

			#endif
		}

		// intersect each line in hough space with each other, so that we can divide the hough space in a senseful way
		std::vector < double > houghSpaceIntersectionsM;
		std::vector < double > houghSpaceIntersectionsT;
		houghSpaceIntersectionsM.reserve(x.size());
		houghSpaceIntersectionsT.reserve(x.size());

		// calc all intersections
		for (unsigned i=0; i<x.size(); i++)
		for (unsigned j=i+1; j<x.size(); j++)
		{
			HoughBand* bandA = _allHoughBands.at(i);
			HoughBand* bandB = _allHoughBands.at(j);

			HoughLine& lineAUp =  bandA->getUpperLine();
			HoughLine& lineALow = bandA->getLowerLine();
			HoughLine& lineBUp =  bandB->getUpperLine();
			HoughLine& lineBLow = bandB->getLowerLine();

			double m;
			m = lineAUp.intersect(lineBUp);
			houghSpaceIntersectionsM.push_back(m);
			houghSpaceIntersectionsT.push_back(lineAUp.eval(m));

			m = lineAUp.intersect(lineBLow);
			houghSpaceIntersectionsM.push_back(m);
			houghSpaceIntersectionsT.push_back(lineAUp.eval(m));

			m = lineALow.intersect(lineBUp);
			houghSpaceIntersectionsM.push_back(m);
			houghSpaceIntersectionsT.push_back(lineALow.eval(m));

			m = lineALow.intersect(lineBLow);
			houghSpaceIntersectionsM.push_back(m);
			houghSpaceIntersectionsT.push_back(lineALow.eval(m));
		}

		// sort them
		sort(houghSpaceIntersectionsM.begin(), houghSpaceIntersectionsM.end());
		sort(houghSpaceIntersectionsT.begin(), houghSpaceIntersectionsT.end());

		// now take the point in between two intersections:
		for (unsigned i=0; i<houghSpaceIntersectionsM.size()-1; i++)
		{
			houghSpaceIntersectionsM[i] += (houghSpaceIntersectionsM[i+1] - houghSpaceIntersectionsM[i]) / 2;
			houghSpaceIntersectionsT[i] += (houghSpaceIntersectionsT[i+1] - houghSpaceIntersectionsT[i]) / 2;
		}
		// remove the last one
		houghSpaceIntersectionsM.pop_back();
		houghSpaceIntersectionsT.pop_back();

		_houghSpaceBinningM.clear();
		_houghSpaceBinningT.clear();

		// setup the hough space bins: twice as much the intersections
		for (unsigned i=0; i<houghSpaceIntersectionsM.size()-1; i++)
		{
			_houghSpaceBinningM.push_back( houghSpaceIntersectionsM[i]);
			_houghSpaceBinningM.push_back( (houghSpaceIntersectionsM[i+1] - houghSpaceIntersectionsM[i]) / 3     + houghSpaceIntersectionsM[i] );
			_houghSpaceBinningM.push_back( (houghSpaceIntersectionsM[i+1] - houghSpaceIntersectionsM[i]) * 2 / 3 + houghSpaceIntersectionsM[i] );

			_houghSpaceBinningT.push_back( houghSpaceIntersectionsT[i]);
			_houghSpaceBinningT.push_back( (houghSpaceIntersectionsT[i+1] - houghSpaceIntersectionsT[i]) / 3     + houghSpaceIntersectionsT[i] );
			_houghSpaceBinningT.push_back( (houghSpaceIntersectionsT[i+1] - houghSpaceIntersectionsT[i]) * 2 / 3 + houghSpaceIntersectionsT[i] );
		}
		_houghSpaceBinningM.push_back(houghSpaceIntersectionsM.back());
		_houghSpaceBinningT.push_back(houghSpaceIntersectionsT.back());

		// now check the entire hough space recursively
		// 7 = tan(m_max)
		// m_max is limited by the tracking algorithm (max angle: 81 deg)
		_recursiveCounter = 0;

		_biggestIntersection.clear();
		divideAndCheckHoughSpace(0, 0, _houghSpaceBinningM.size()-1, _houghSpaceBinningT.size()-1);
//		divideAndCheckHoughSpace(-7, -M_PI_2, 7, M_PI_2);

		streamlog_out(DEBUG3) << "New Track: ";
		for (unsigned j = 0; j<_biggestIntersection.size(); j++)
			streamlog_out(DEBUG3) << " " << _biggestIntersection.at(j)->getRefIdx();
		streamlog_out(DEBUG3) << endl;

		// now delete all hits from hits, which are not in the _biggestIntersection

		streamlog_out(DEBUG3) << "Hit Filtering (* = remove) @ " << (n==0 ? "x" : "y") << ":z :";
		for (int i = (int)hits.size()-1; i>=0; i--)
		{
			bool isOkHit = false;
			for (unsigned j = 0; j<_biggestIntersection.size(); j++)
			{
				if (_biggestIntersection.at(j)->getRefIdx() == (unsigned)i)
				{
					isOkHit = true;
					break;
				}
			}

			streamlog_out(DEBUG3) << " " << setw(2) << i;

			if (!isOkHit)
			{
				x_3d.erase(x_3d.begin() + i);
				y_3d.erase(y_3d.begin() + i);
				z_3d.erase(z_3d.begin() + i);
				size.erase(size.begin() + i);
				hits.erase(hits.begin() + i);
				streamlog_out(DEBUG3) << "*";
			}
			else
				streamlog_out(DEBUG3) << " ";
		}
		streamlog_out(DEBUG3) << endl;

		// clear the old hough bands
		for (unsigned i=0; i<_allHoughBands.size(); i++)
			delete _allHoughBands.at(i);
		_allHoughBands.clear();

		// check if x alone already filtered enough to reject this track
		if ((int)hits.size() < _minNumberOfHits)
		{
			break;
		}

		#ifdef ROOT_OUT
		_rootFile->WriteTObject(_cHough);
//		_rootFile->Write();
//		_rootFile->Close();
//		exit(1);
		#endif
	}

	if ((int)hits.size() < _minNumberOfHits)
	{
		_statsNrTracksRejected++;
		return NULL;
	}

	// create the filtered track as LCIO Cluster
	IMPL::ClusterImpl* filteredTrack = new IMPL::ClusterImpl();

	filteredTrack->setIPhi(track->getIPhi());
	filteredTrack->setITheta(track->getITheta());
	for (unsigned n = 0; n < hits.size(); ++n)
		filteredTrack->addHit(hits.at(n), 1);

	_statsNrHitsRejected += track->getCalorimeterHits().size() - hits.size();

	return filteredTrack;
}

std::ostream& operator<<(std::ostream& o, HoughBand* b)
{
	return o
			<< b->_refIdx << "("
			<< setw(8) << b->_upperLine.x() << "/" << setw(8) << (b->_upperLine.y())
			<< " - "
			<< setw(8) << b->_lowerLine.x() << "/" << setw(8) << (b->_lowerLine.y())
			<< ")";
}

