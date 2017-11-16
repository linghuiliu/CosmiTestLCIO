/*
 * HCalTrackAnalyse.cxx
 *
 *  Created on: Sep 29, 2008
 *      Author: weuste
 */

// my class
#include "HcalTrackingNN.hh"

// Marlin & LCIO
#include "marlin/VerbosityLevels.h"
#include <EVENT/LCCollection.h>
#include <EVENT/CalorimeterHit.h>
#include <EVENT/Cluster.h>
#include <EVENT/LCIO.h>
#include <IMPL/ClusterImpl.h>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCFlagImpl.h>

#include <boost/foreach.hpp>

// std c++ header
#include <cmath>
#include <vector>
#include <set>
#include <iomanip>
#include <time.h>
#include <algorithm>

#include <TriggerBits.hh>	// TODO: HACK! Don't use CALICE specific software
#include <string>
#include <Exceptions.h>
#include <HcalCellIndex.hh>

using namespace std;
using namespace CALICE;

HCalTrackingNNProcessor aHCalTrackingNNProcessor;

HCalTrackingNNProcessor::HCalTrackingNNProcessor()
	: Processor("HCalTrackingNNProcessor")
{

//	// setting the default values:
	registerProcessorParameter("minimumTrackLength", "A track has to be this long (1st <-> last layer) to be recognised as real track", _minTrackLength, 6);
	registerProcessorParameter("maxGapSizePerTrack", "A track may skip this many layers without isolated at once. This is not the total gap number, but the size of a single gap", _maxGapSize, 1);
	registerProcessorParameter("maxGapRatioPerTrack", "The number of gaps divided by the length (1st <-> last layer) of a track may not be greater than this value.", _maxTrackGapRatio, .50);
//	registerProcessorParameter("handleNonIsolatedAsGap", "A track can only consist of isolated hits. If a track can be extended over a layer containing a nonisolated hit, shall we do it (true) or shall we stop the track (false)?", m_handleNonIsolatedAsGap, true);
//	registerProcessorParameter("firstLayerForTrackLength", "the histogram StartLayerTrackLengthLayerDiff lists the length of all tracks that startet in layer <firstLayerForTrackLength> or lower", m_firstLayerForTrackLength, 1);

	registerProcessorParameter("cellNeighboursProcessor", "the name of the CellNeighboursProcessor for geometry information", _cellNeighbourProcessorName, std::string("AhcCellNeighboursProcessor"));
	registerProcessorParameter("cellDescriptionProcessor", "the name of the CellDescriptionProcessor for geometry information", _cellDescriptionProcessorName, std::string("AhcCellDescriptionProcessor"));

	registerInputCollection(LCIO::CALORIMETERHIT, "AhcCalorimeterHitsCollection", "Name of the AhcCalorimeter_Hits collection", _lccCalorimeterHitsName, std::string("AhcCalorimeter_Hits"));
	registerOutputCollection(LCIO::CLUSTER, "AhcTrackCollection", "Name of the output track collection", _lccTrackClusterOutName, std::string("AhcTracks"));
}

HCalTrackingNNProcessor::~HCalTrackingNNProcessor()
{
}

void HCalTrackingNNProcessor::init()
{

	streamlog_out(MESSAGE4) << "Init Processor HCalTrackAnalysisProcessor" << endl;

	printParameters();

	// connect to CALICE geometry procs

    _cellDescriptions = CALICE::CellDescriptionProcessor::getCellDescriptions(_cellDescriptionProcessorName);
	if (!_cellDescriptions)
		throw Exception("We have no CALICE::CellDescriptionProcessor");

    _cellNeighbours = CALICE::CellNeighboursProcessor::getNeighbours(_cellNeighbourProcessorName);
	if (!_cellNeighbours)
		throw Exception("We have no CALICE::CellNeighboursProcessor");


	_statNEmptyEvents = 0;
	_statNProcessedEvents = 0;
	_statNRuns = 0;
	_statNTracks = 0;

	streamlog_out(MESSAGE4) << "Successfully loaded HCalTrackAnalysisProcessor" << endl;

}

void HCalTrackingNNProcessor::end()
{
	streamlog_out(MESSAGE) << "DONE" << endl;
	printStatus();
}

/**
 *
 * LCIO input: iterates through all events. The tracking algorithm will try to find
 * tracks for each event.
 */
void HCalTrackingNNProcessor::processEvent(LCEvent* evt)
{

	try
	{
		LCCollection* lcc = evt->getCollection(_lccCalorimeterHitsName);	// get the collection
		_cellDescriptions->getDecoder()->setCellIDEncoding( lcc->getParameters().getStringVal("CellIDEncoding") );
		analyzeSingleEvent(lcc);		// analyse it and add results to event

		evt->addCollection(_tracks, _lccTrackClusterOutName);
		_statNTracks += _tracks->getNumberOfElements();
		_statNProcessedEvents++;

		if (_statNProcessedEvents % 1000 == 0)
			printStatus();
	}
	catch (DataNotAvailableException& e)
	{
		_statNEmptyEvents++;
		streamlog_out(WARNING) << "Missing input collection" << endl;
	}

}

void HCalTrackingNNProcessor::processRunHeader(LCRunHeader* run)
{
	_statNRuns++;
	printStatus();
}

void HCalTrackingNNProcessor::check(LCEvent * evt)
{

}

void HCalTrackingNNProcessor::printStatus()
{
	streamlog_out(MESSAGE) << setw(20) << "Empty Events: " << _statNEmptyEvents << endl;
	streamlog_out(MESSAGE) << setw(20) << "Processed Events: " << _statNProcessedEvents << endl;
	streamlog_out(MESSAGE) << setw(20) << "Processed Runs: " << _statNRuns << endl;
	streamlog_out(MESSAGE) << setw(20) << "Found Tracks: " << _statNTracks << " (" << 1.0 * _statNTracks / _statNProcessedEvents << " tracks/evt)" << endl;
}

/**
 * does the actual tracking
 * gets called from processEvent after Trigger / other options have been checked and this event considered to be ok
 */
void HCalTrackingNNProcessor::analyzeSingleEvent(LCCollection* event)
{

	/*
	 * Analysis consists of these steps:
	 * for each event:
	 * 1.) convert the LCC into a HashMap with indices ijk for faster access
	 * 2.) search for tracks
	 * 3.) filter some of the found tracks: remove first/last tile if whished
	 */

	if (!event)
		return;

	fillHitMap(event);

	countNeighbours();

	findTracks();

	streamlog_out(DEBUG4) << "Found " << _tracks->getNumberOfElements() << " tracks before filtering. Tracks: (number = ok, s = too short, r = too many gaps)" << endl;
	for (int n = 0; n < _tracks->getNumberOfElements(); ++n)
	{
		EVENT::Cluster* track = dynamic_cast< EVENT::Cluster* >(_tracks->getElementAt(n));

		int length = getTrackLayerLength(track);
		double gapRatio = 1.0 - (1.0 * track->getCalorimeterHits().size() / length);

		if (length < _minTrackLength)
		{
			streamlog_out(DEBUG4) << "s:\t" << setw(10) << gapRatio << " " << setw(3) << length << " " << track << endl;
			_tracks->removeElementAt(n);
			delete track;
			n--;
		}
		else if(gapRatio > _maxTrackGapRatio)
		{
			streamlog_out(DEBUG4) << "r:\t" << setw(10) << gapRatio << " " << setw(3) << length << " " << track << endl;
			_tracks->removeElementAt(n);
			delete track;
			n--;
		}
		else
		{
			streamlog_out(DEBUG4) << n << ":\t" << setw(10) << gapRatio << " " << setw(3) << length << " " << track << endl;
			setTrackAngle(track);
		}

		BOOST_FOREACH(CalorimeterHit* h, track->getCalorimeterHits())
		{
			CALICE::HcalCellIndex idx(h->getCellID0());
			CALICE::CellDescription* d = _cellDescriptions->getByCellID(h->getCellID0());

			streamlog_out(DEBUG4) << idx.getTileColumn() << "/" << idx.getTileRow() << "/" << idx.getLayerIndex();
			streamlog_out(DEBUG4) << "(" << d->getX() << "/" << d->getY() << "/" << d->getZ() << ") ";
		}

		streamlog_out(DEBUG4) << endl;

	}


	// cleanup
	cleanUp();


	streamlog_out(DEBUG4) << "======================================================================" << endl;
}

void HCalTrackingNNProcessor::fillHitMap(LCCollection* event)
{
	_hits.clear();

	// convert each calorimeter hit into a wrapper, and fill it into the _hits map,
	// which is first sorted by layer id and then by cellID. This will speedUp the lookup
	for (int n = 0; n < event->getNumberOfElements(); ++n)
	{
		EVENT::CalorimeterHit* h = dynamic_cast< EVENT::CalorimeterHit* >( event->getElementAt(n) );

		HCalTrackedHit* trackedHit = new HCalTrackedHit(h);

		_hits[trackedHit->getK()][trackedHit->getHit()->getCellID0()] = trackedHit;
	}
}

void HCalTrackingNNProcessor::countNeighbours()
{

	// loop over all hits we have
	BOOST_FOREACH(mapHashmapHits_t::value_type &layer, _hits)
	{
		BOOST_FOREACH(hashMapHits_t::value_type &hit, layer.second)
		{
			// get a list of the neighbouring ids
			int cellID0 = hit.second->getHit()->getCellID0();

			CellNeighbours* neighbours = _cellNeighbours->getByCellID(cellID0);

			// check each id if it contains a hit. If it does, add ourselves to the list of its neighbours
			BOOST_FOREACH(int cellID0Neigbour, neighbours->getNeighbours(CALICE::CellNeighbours::direct, CALICE::CellNeighbours::module ))
			{
				CellIndex idx(cellID0Neigbour);
				int layerK = idx.getLayerIndex();

				if (_hits.find(layerK) != _hits.end())
				{
					if (_hits[layerK].find(cellID0Neigbour) != _hits[layerK].end())
					{
						_hits[layerK][cellID0Neigbour]->addNeighbour(hit.second);
					}
				}
			}
			// now the same for the cornered neihbours. This is done via copy & paste, as the function call to getNeighbours() is faster this way
			BOOST_FOREACH(int cellID0Neigbour, neighbours->getNeighbours(CALICE::CellNeighbours::corner, CALICE::CellNeighbours::module ))
			{
				CellIndex idx(cellID0Neigbour);
				int layerK = idx.getLayerIndex();

				if (_hits.find(layerK) != _hits.end())
				{
					if (_hits[layerK].find(cellID0Neigbour) != _hits[layerK].end())
					{
						_hits[layerK][cellID0Neigbour]->addNeighbour(hit.second);
					}
				}
			}
		}
	}
}

void HCalTrackingNNProcessor::findTracks()
{
	// list of tracks
	_tracks = new LCCollectionVec(LCIO::CLUSTER);
	/*To save also the associated hits, i.e. to be able to use cluster->getCalorimeterHits*/
	_tracks->setFlag(_tracks->getFlag()|( 1 << LCIO::CLBIT_HITS)) ;

	// loop over all hits we have
	BOOST_FOREACH(mapHashmapHits_t::value_type &layer, _hits)
	{
		BOOST_FOREACH(hashMapHits_t::value_type &hit, layer.second)
		{
			// check if this is an isolated hit
			if (hit.second->isIsolated() && !hit.second->hasTrack())
			{
				_dbgIndentSize = 0;

				// if yes: start collecting a list of possible successors
				EVENT::Cluster* track = findSingleTrack(hit.second);

				addTrack(track);
			}
		}
	}

}

int	_g_curPosI;
int	_g_curPosJ;
int	_g_curPosK;
bool cmpCellIdPositionToCurPosition(int a, int b)
{
	CellIndex idx_a(a);
	CellIndex idx_b(b);

	int a_i = idx_a.getPadColumn();
	int a_j = idx_a.getPadRow();
	int a_k = idx_a.getLayerIndex();
	int b_i = idx_b.getPadColumn();
	int b_j = idx_b.getPadRow();
	int b_k = idx_b.getLayerIndex();

	double score_a = labs(_g_curPosK - a_k) * 100000 + pow(fabs(_g_curPosI - a_i),2) + pow(fabs(_g_curPosJ - a_j),2);
	double score_b = labs(_g_curPosK - b_k) * 100000 + pow(fabs(_g_curPosI - b_i),2) + pow(fabs(_g_curPosJ - b_j),2);

//	cout << _g_curPosI << "/" << _g_curPosJ << "/" << _g_curPosK << endl;
//	cout << a_i << "/" << a_j << "/" << a_k << " -> " << score_a << endl;
//	cout << b_i << "/" << b_j << "/" << b_k << " -> " << score_b << endl;

	return score_a < score_b;
}
bool cmpTracks(EVENT::Cluster* a, EVENT::Cluster* b) { return a->getCalorimeterHits().size() < b->getCalorimeterHits().size(); }

EVENT::Cluster* HCalTrackingNNProcessor::findSingleTrack(HCalTrackedHit* firstHit)
{
	_dbgIndentSize++;

	// the track
	IMPL::ClusterImpl* track = new IMPL::ClusterImpl();
	streamlog_out(DEBUG0) << "New Track: " << (void*)track << endl;

	// add this hit as first member
	track->addHit(firstHit->getHit(), 1);
	firstHit->setTrack(track);

	// we search for each neigbour of the successing cell for tracks. These will be saved here
	std::vector < EVENT::Cluster* >	subTracks;

	// get the cellIndex for this hit
//	int cellIdCentralTile = firstHit->getHit()->getCellID0();

	std::set<int> curLayerAllCellIds;	// the (position only, we don't yet know if there exist a hit) cellIds of the current layer of the following loop
	std::set<int> nextLayerAllCellIds; // the cellIds of the successing layer to the current
	std::vector<int> allSuccessorCellIds;	// after the loop has finished, this contains the union of all nextLayerCellIds,
										// i.e. all positions at which the track could possibly continue
										// this list is dependant on the _maxGapSize. If we allow no gaps, than this will contain O(5) entries
										// for larger gaps the search cone is increased
	nextLayerAllCellIds.insert(firstHit->getHit()->getCellID0());

	streamlog_out(DEBUG2) << setw(_dbgIndentSize) << _dbgIndentSize << "|" << "findSingleTrack for " << firstHit->getHit() << endl;

	// for each allowed gap within the track
	for (int gapNr=0; gapNr <= _maxGapSize; gapNr++)
	{

		// advance one layer by: copying all cellIds from the (old) next layer to the (now) current layer
		curLayerAllCellIds.clear();
		BOOST_FOREACH(int cellIndex, nextLayerAllCellIds)
			curLayerAllCellIds.insert(cellIndex);
		nextLayerAllCellIds.clear();

		streamlog_out(DEBUG0) << setw(_dbgIndentSize) << _dbgIndentSize << "|" << "Gap @ " << gapNr << "/" << _maxGapSize << ": " << nextLayerAllCellIds.size() << " cellIDs in currentLayer:" << endl;

		BOOST_FOREACH(int cellIndex, curLayerAllCellIds)
		{
			CellIndex idx(cellIndex);
			streamlog_out(DEBUG0) << idx.getPadColumn() << "/" << idx.getPadRow()  << "/" << idx.getLayerIndex() << "  ";
		}
		streamlog_out(DEBUG0) << endl;

		// get the cellIDs of the neighbours in the next layer for all cellIDs of the current layer
		// this loop is dependent on the number of gaps we consider
		// I.e. if this track has now gap, then currentLayer has only 1 single entry and we will get all (typically: 5)
		// positions of the successing layer
		// if we consider 1 gap, then we have to increase the search radius, otherwise we are limiting our angle of the track
		// Of course we will have double counting, but this will be taken care of by using a set
		BOOST_FOREACH(int cellIndex, curLayerAllCellIds)
		{
			const std::vector<int> nextLayerCellIds = _cellNeighbours->getByCellID(cellIndex)
					->getNeighbours(CALICE::CellNeighbours::direct, CALICE::CellNeighbours::forward);

			BOOST_FOREACH(int cellIndexNext, nextLayerCellIds)
			{
				nextLayerAllCellIds.insert(cellIndexNext);
			}
		}


		// if there are no successors (because we reached the end of the calorimeter): end
		if (nextLayerAllCellIds.size() == 0)
			break;

		streamlog_out(DEBUG0) << setw(_dbgIndentSize) << _dbgIndentSize << "|" << "New neighbours: " << endl;

		// copy the unique list of cellIds of the successing layer into the list of all possible successor positions
		BOOST_FOREACH(int cellIndex, nextLayerAllCellIds)
		{
			allSuccessorCellIds.push_back(cellIndex);
			CellIndex idx(cellIndex);
			streamlog_out(DEBUG0) << idx.getPadColumn() << "/" << idx.getPadRow()  << "/" << idx.getLayerIndex() << "  ";
		}

		streamlog_out(DEBUG0) << endl;
	}

	// get some debug messages out
	streamlog_out(DEBUG0) << setw(_dbgIndentSize) << _dbgIndentSize << "|" << firstHit->getHit() << " has " << allSuccessorCellIds.size() << " successors in the next layers" << endl;
	BOOST_FOREACH(int cellId, allSuccessorCellIds)
	{
		CellIndex idx(cellId);
		streamlog_out(DEBUG0) << idx.getPadColumn() << "/" << idx.getPadRow() << "/" << idx.getLayerIndex() << " ";
	}
	streamlog_out(DEBUG0) << endl;


	// now we have a list of positions, which are possible successors
	// we will check each position one one after another if a hit is existent, but first we have to sort them.
	// If we try to construct a track the used calorimeter hits will be removed from the list of available hits
	// Hence we should favour the hits which are closest to the original position and should consider them first,
	// otherwise we would have unexpected gaps and strange kinks
	CellIndex firstHitIdx(firstHit->getHit()->getCellID0());
	_g_curPosI = firstHitIdx.getPadColumn() - 1;
	_g_curPosJ = firstHitIdx.getPadRow() - 1;
	_g_curPosK = firstHitIdx.getLayerIndex();

//	allSuccessorCellIds.sort(cmpCellIdPositionToCurPosition);
	sort(allSuccessorCellIds.begin(), allSuccessorCellIds.end(), cmpCellIdPositionToCurPosition);


	// get some debug messages out
	streamlog_out(DEBUG0) << setw(_dbgIndentSize) << _dbgIndentSize << "|" << "after sorting:" << endl;
	BOOST_FOREACH(int cellId, allSuccessorCellIds)
	{
		CellIndex idx(cellId);
		streamlog_out(DEBUG0) << idx.getPadColumn() << "/" << idx.getPadRow() << "/" << idx.getLayerIndex() << " ";
	}
	streamlog_out(DEBUG0) << endl;

	// for each of these cell positions: check if the hit exist and if it is not used within a track already. If everything's ok:
	// use it to search for a new track recursively, and save the result in subTracks list

	streamlog_out(DEBUG1) << setw(_dbgIndentSize) << _dbgIndentSize << "|" << "List of successor hits: ";

	std::list< HCalTrackedHit* > successorHits;
	BOOST_FOREACH(int cellID, allSuccessorCellIds)
	{
		CellIndex curCellIdx(cellID);
		int layerIndex = curCellIdx.getLayerIndex();
		if (_hits[layerIndex].find(cellID) != _hits[layerIndex].end())
		{
			HCalTrackedHit* trackedHit = _hits[layerIndex][cellID];

			if (trackedHit->isIsolated() && !trackedHit->hasTrack())
			{
				successorHits.push_back(trackedHit);
				streamlog_out(DEBUG1) << trackedHit->getHit() << " ";
			}
		}
	}
	streamlog_out(DEBUG1) << endl;

	BOOST_FOREACH(HCalTrackedHit* trackedHit, successorHits)
	{
		// check if this hit is already part of a track
		// we did this check before, but by calling findSingleTrack from this position might set (for the 2nd hit in successorHits onwards)
		// this property
		if (trackedHit->hasTrack())
			continue;
		EVENT::Cluster* subTrack = findSingleTrack(trackedHit);
		subTracks.push_back(subTrack);	// save the found subtracks according to their length
	}

	streamlog_out(DEBUG3) << setw(_dbgIndentSize) << _dbgIndentSize << "|" << firstHit->getHit() << " - Found " << subTracks.size() << " sub track(s) with length: " ;

	// sort the subtracks according to their length
//	subTracks.sort(cmpTracks);
	sort(subTracks.begin(), subTracks.end(), cmpTracks);

	BOOST_FOREACH(EVENT::Cluster* t, subTracks)
	{
		streamlog_out(DEBUG3) << getTrackLayerLength(t) << " ";
	}
	streamlog_out(DEBUG3) << endl;

	// now iterate through the sorted list and search for the longest track that is shorter than the minimum track length
	// (if no such track exist, take the next longest one instead) and merge with this
	// the remaining will be added to the global list of found tracks
	EVENT::Cluster* longestTrackBelowMinSizeThreshold = NULL;


	BOOST_FOREACH(EVENT::Cluster* t, subTracks)
	{
		if (t->getCalorimeterHits().size() == 0)
		{
			streamlog_out(DEBUG1) << setw(_dbgIndentSize) << _dbgIndentSize << "|" << "Empty Track - discarding: " << endl;
			delete t;
		}
		else if (getTrackLayerLength(t) < _minTrackLength)	// if this track is too short: identify it as the one to merge with
		{

			if (longestTrackBelowMinSizeThreshold)
			{
				streamlog_out(DEBUG1) << setw(_dbgIndentSize) << _dbgIndentSize << "|" << "Track too short - discarding: " << longestTrackBelowMinSizeThreshold << endl;
				deleteTrack(longestTrackBelowMinSizeThreshold);
				longestTrackBelowMinSizeThreshold = NULL;
			}

			longestTrackBelowMinSizeThreshold = t;
			streamlog_out(DEBUG1) << setw(_dbgIndentSize) << _dbgIndentSize << "|" << "New longest track below threshold candidate: " << t << endl;
		}
		else if (longestTrackBelowMinSizeThreshold)	// if this track is longer, and we already have one to merge with: just add the track to the list
		{
			addTrack(t);
			streamlog_out(DEBUG1) << setw(_dbgIndentSize) << _dbgIndentSize << "|" << "Adding track " << t << endl;
		}
		else	// if we are longer than the minimum required length, but have not yet any track to merge with: use this track
		{
			longestTrackBelowMinSizeThreshold = t;
			streamlog_out(DEBUG1) << setw(_dbgIndentSize) << _dbgIndentSize << "|" << "No longest track below threshold available. Replacement: " << t << endl;
		}
	}


	if (longestTrackBelowMinSizeThreshold)
	{
		streamlog_out(DEBUG1) << setw(_dbgIndentSize) << _dbgIndentSize << "|" << "Merging with " << longestTrackBelowMinSizeThreshold << endl;

		// now merge the track with the longestTrackBelowMinSizeThreshold
		BOOST_FOREACH(EVENT::CalorimeterHit* hit, longestTrackBelowMinSizeThreshold->getCalorimeterHits())
		{
			track->addHit(hit, 1);
		}

		deleteTrack(longestTrackBelowMinSizeThreshold);
		longestTrackBelowMinSizeThreshold = NULL;
	}

	streamlog_out(DEBUG3) << setw(_dbgIndentSize) << _dbgIndentSize << "|" << endl << endl;

	_dbgIndentSize--;
	return track;
}

void HCalTrackingNNProcessor::addTrack(EVENT::Cluster* track)
{
	int nrHits = track->getCalorimeterHits().size();
	int trackLayerLength = getTrackLayerLength(track);

	if (trackLayerLength < _minTrackLength)
	{
		streamlog_out(DEBUG2) << "Will not add track (too short): " << track << endl;
		deleteTrack(track);
		return;
	}

	if (1.0 * nrHits / trackLayerLength < _maxTrackGapRatio)
	{
		streamlog_out(DEBUG2) << "Will not add track (too many gaps): " << track << endl;
		deleteTrack(track);
		return;
	}

	_tracks->addElement(track);
	streamlog_out(DEBUG2) << "Added track:" << track << endl;
}

int HCalTrackingNNProcessor::getTrackLayerLength(EVENT::Cluster* track)
{
	EVENT::CalorimeterHit* firstHit = track->getCalorimeterHits().front();
	EVENT::CalorimeterHit* lastHit = track->getCalorimeterHits().back();

	CellIndex idxFirst(firstHit->getCellID0());
	CellIndex idxLast(lastHit->getCellID0());

	return idxLast.getLayerIndex() - idxFirst.getLayerIndex() + 1;
}

void HCalTrackingNNProcessor::deleteTrack(EVENT::Cluster* track)
{
	// delete any old references of HCalTrackHits to this track
	BOOST_FOREACH(EVENT::CalorimeterHit* h, track->getCalorimeterHits())
	{
		CellIndex idx(h->getCellID0());

		if (_hits.find(idx.getLayerIndex()) != _hits.end() &&
			_hits[idx.getLayerIndex()].find(idx.getCellID()) != _hits[idx.getLayerIndex()].end())
		{
			_hits[idx.getLayerIndex()][idx.getCellID()]->setTrack(NULL);
		}
	}

	streamlog_out(DEBUG0) << "Delete " << (void*)track  << endl;

	// and delete it
	delete track;


}

void HCalTrackingNNProcessor::setTrackAngle(EVENT::Cluster* track)
{
	int cellIdBegin = track->getCalorimeterHits().front()->getCellID0();
	int cellIdEnd = track->getCalorimeterHits().back()->getCellID0();

	CALICE::CellDescription* descBegin = _cellDescriptions->getByCellID(cellIdBegin);
	CALICE::CellDescription* descEnd = _cellDescriptions->getByCellID(cellIdEnd);

	double x1 = descBegin->getX(); // + (descBegin->getSizeX()/2);
	double y1 = descBegin->getY(); // + (descBegin->getSizeY()/2);
	double z1 = descBegin->getZ();

	double x2 = descEnd->getX(); // + (descEnd->getSizeX()/2);
	double y2 = descEnd->getY(); // + (descEnd->getSizeY()/2);
	double z2 = descEnd->getZ();

	double dX = x2 - x1;
	double dY = y2 - y1;
	double dZ = z2 - z1;

//	streamlog_out(DEBUG4) << "begin: cellID: " << cellIdBegin << " - x/y/z=" << x1 << "/" << y1 << "/" << z1 << " size(x/y)=" << descBegin->getSizeX() << "/" << descBegin->getSizeY() << endl;
//	streamlog_out(DEBUG4) << "end  : cellID: " << cellIdEnd   << " - x/y/z=" << x2 << "/" << y2 << "/" << z2 << " size(x/y)=" << descEnd  ->getSizeX() << "/" << descEnd  ->getSizeY() << endl;

	((IMPL::ClusterImpl*)track)->setIPhi( dZ / sqrt(dX*dX + dY*dY + dZ*dZ ) );

//	streamlog_out(DEBUG4) << "cos phi: " << track->getIPhi() << endl;
}


void HCalTrackingNNProcessor::cleanUp()
{
	// loop over all hits we have
	BOOST_FOREACH(mapHashmapHits_t::value_type &layer, _hits)
	{
		BOOST_FOREACH(hashMapHits_t::value_type &hit, layer.second)
		{
			delete hit.second;
			hit.second = NULL;
		}
	}
	_hits.clear();
}

std::ostream& operator<<(std::ostream& out, EVENT::CalorimeterHit* hit)
{
	CellIndex idx(hit->getCellID0());
	return out << idx.getPadColumn() << "/" << idx.getPadRow() << "/" << idx.getLayerIndex();
}

std::ostream& operator<<(std::ostream& out, EVENT::Cluster* track)
{

	out << "Track with length " << track->getCalorimeterHits().size() << ": ";
	BOOST_FOREACH(EVENT::CalorimeterHit* h, track->getCalorimeterHits())
	{
		out << h << " ";
	}

	return out;
}

