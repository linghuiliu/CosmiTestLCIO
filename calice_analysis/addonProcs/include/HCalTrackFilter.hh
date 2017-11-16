/*
 * HCalTrackFilter.h
 *
 *  Created on: 19.09.2011
 *      Author: weuste
 */

#ifndef HCalTrackFilter_H_
#define HCalTrackFilter_H_

//#define ROOT_OUT

#include <marlin/Processor.h>
#include <marlin/EventModifier.h>

#include <EVENT/Cluster.h>

// Calice Software
#include <Mapper.hh>
#include <CellDescriptionProcessor.hh>
#include <CellNeighbours.hh>
#include <CellNeighboursProcessor.hh>
#include <CellIndex.hh>


#include <cmath>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <iostream>
#include <string>

#ifdef ROOT_OUT
class TFile;
class TCanvas;
#endif

class HoughBand;

class HCalTrackFilter : marlin::Processor
{
public:
	HCalTrackFilter();
	virtual ~HCalTrackFilter();

	marlin::Processor* newProcessor() { return new HCalTrackFilter(); }

	// the standard Marlin functions


	/** Called at the begin of the job before anything is read.
	 * Use to initialize the processor, e.g. book histograms.
	 */
	virtual void init();

	/** Called for every run.
	 */
	virtual void processRunHeader(LCRunHeader* run){};

	/** Called for every event - the working horse.
	 */
	virtual void processEvent(LCEvent * evt);

	virtual void check(LCEvent * evt) {};

	/** Called after data processing for clean up.
	 */
	virtual void end();

protected:
	virtual EVENT::Cluster* filterTrackHough(EVENT::Cluster *track);

	std::vector < double >	_houghSpaceBinningM;
	std::vector < double >	_houghSpaceBinningT;
	std::vector < HoughBand* > _allHoughBands;
	std::vector < HoughBand* > _biggestIntersection;
	void divideAndCheckHoughSpace(int mMin, int rMin, int mMax, int rMax);
	int _recursiveCounter;

#ifdef ROOT_OUT
	TCanvas*	_cHough;
	TFile* _rootFile;

	void saveRootTrackDiffCanvas(EVENT::Cluster *preFilterTrack, EVENT::Cluster *postFilterTrack);
#endif

	// CALICE Geometry
	CALICE::MappedContainer<CALICE::CellDescription>* 	_cellDescriptions;
//	CALICE::MappedContainer<CALICE::CellNeighbours>* 	_cellNeighbours;
	std::string	_cellDescriptionProcessorName;
//	std::string	_cellNeighbourProcessorName;

	// settings
	std::string _lccTrackClusterName;
	std::string _lccFilteredTrackClusterName;
	int _minNumberOfHits;
	int _maxNrRejectedHits;

	// stats
	unsigned	_statsNrHitsRejected;
	unsigned	_statsNrTracksRejected;
	unsigned	_statsNrEvents;
	unsigned	_statsNrTracks;



};


// a line in the hough space of t = -mx + y
class HoughLine
{
public:
	HoughLine(double x, double y) : _x(-x), _y(y) {}

	const double x() { return _x; }
	const double y() { return _y; }

	const double eval(const double m) { return m * _x + _y; }
	const double intersect(HoughLine& other) { return (_y - other._y) / (other._x - _x); };
protected:

	double _x;
	double _y;
};

enum EHoughBandInRect
{
	NotInRect = 0,
	LineCrossing = 1,
	UpperLineCrossing = 1,
	LowerLineCrossing = 1,
	RectIncludedInBand = 2
};


class HoughBand
{
public:
	HoughBand(double cellPosX, double cellPosY, double cellSizeY, unsigned refIdx)
		: _upperLine(cellPosX, cellPosY+cellSizeY)
		, _lowerLine(cellPosX, cellPosY-cellSizeY)
		, _refIdx(refIdx)
		{};

	EHoughBandInRect isInQuadrant(double mMin, double tMin, double mMax, double tMax)
	{

		using namespace std;

		// check if upperLine crosses this rect
		double upDeltaTopLeft 		= _upperLine.eval(mMin) - tMin;
		double upDeltaTopRight 		= _upperLine.eval(mMax) - tMin;
		double upDeltaBottomLeft 	= _upperLine.eval(mMin) - tMax;
		double upDeltaBottomRight 	= _upperLine.eval(mMax) - tMax;

		streamlog_out(DEBUG0) << "Quadrant check "
				<< setw(8) << mMin << "-m->" << setw(8) << mMax << "   "
				<< setw(8) << tMin << "-t->" << setw(8) << tMax	<< endl;

		streamlog_out(DEBUG0) << "  Up Delta:"
				<< setw(12) << upDeltaTopLeft << " / "
				<< setw(12) << upDeltaTopRight << " / "
				<< setw(12) << upDeltaBottomLeft << " / "
				<< setw(12) << upDeltaBottomRight << " "
				<< endl;

		if (! ( (upDeltaTopLeft > 0 && upDeltaTopRight > 0 && upDeltaBottomLeft > 0 && upDeltaBottomRight > 0) ||
			    (upDeltaTopLeft < 0 && upDeltaTopRight < 0 && upDeltaBottomLeft < 0 && upDeltaBottomRight < 0)    ) )
		{
			return UpperLineCrossing;
		}

		// upper line did not cross this rect --> check lower line
		double lowDeltaTopLeft 		= _lowerLine.eval(mMin) - tMin;
		double lowDeltaTopRight 	= _lowerLine.eval(mMax) - tMin;
		double lowDeltaBottomLeft 	= _lowerLine.eval(mMin) - tMax;
		double lowDeltaBottomRight 	= _lowerLine.eval(mMax) - tMax;

		streamlog_out(DEBUG0) << "  Low Delta:"
				<< setw(12) << lowDeltaTopLeft << " / "
				<< setw(12) << lowDeltaTopRight << " / "
				<< setw(12) << lowDeltaBottomLeft << " / "
				<< setw(12) << lowDeltaBottomRight << " "
				<< endl;

		if (! ( (lowDeltaTopLeft > 0 && lowDeltaTopRight > 0 && lowDeltaBottomLeft > 0 && lowDeltaBottomRight > 0) ||
			    (lowDeltaTopLeft < 0 && lowDeltaTopRight < 0 && lowDeltaBottomLeft < 0 && lowDeltaBottomRight < 0)    ) )
		{
			return LowerLineCrossing;
		}

		// no crossing between both lines, but maybe this rect is in between both

		if ( upDeltaTopLeft > 0 && upDeltaTopRight > 0 && lowDeltaBottomLeft < 0 && lowDeltaBottomRight < 0 )
			return RectIncludedInBand;
		else
			return NotInRect;
	}

	unsigned getRefIdx() { return _refIdx; }

	HoughLine& getUpperLine() { return _upperLine; }
	HoughLine& getLowerLine() { return _lowerLine; }
protected:

	HoughLine _upperLine;
	HoughLine _lowerLine;

	unsigned _refIdx;

	friend std::ostream& operator<<(std::ostream& o, HoughBand* b);
};

std::ostream& operator<<(std::ostream& o, HoughBand* b);

#endif /* HCalTrackFilter_H_ */
