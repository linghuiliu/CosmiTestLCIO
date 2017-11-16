/*
 * HCalTrackAnalyse.h
 *
 *  Created on: Sep 29, 2008
 *      Author: weuste
 */

/**
 *
 */

#ifndef HCALTRACKINGPROCESSOR_H_
#define HCALTRACKINGPROCESSOR_H_

#include "marlin/Processor.h"
#include "IMPL/LCCollectionVec.h"

#include <EVENT/CalorimeterHit.h>
#include <EVENT/Cluster.h>
#include <IMPL/ClusterImpl.h>

#include <Mapper.hh>
#include <CellDescriptionProcessor.hh>
#include <CellNeighbours.hh>
#include <CellNeighboursProcessor.hh>
#include <CellIndex.hh>

#include <map>
#include <vector>
#include <list>
#include <cassert>
#include <set>

#include <boost/unordered_map.hpp>

using namespace marlin;
using namespace lcio;

/**
 * This marlin processor tries to find tracks in the calorimeter.
 * Currently optimised for the AHCal of CALICE.
 *
 * The algorithm works the following way:
 * -# Check all hits whether they are isolated (e.g. have no neighbour hits in the same layer)
 * -# Take an isolated hit and advance to the next layer. Check, if there is an isolated hit at the same position. If there is: join them together to a track.
 * -# For analysis, the energy distribution for all found tracks is saved. (both total, and per tile)
 */
class HCalTrackingNNProcessor : public marlin::Processor
{
public:

	HCalTrackingNNProcessor();	/** constructor */
	virtual ~HCalTrackingNNProcessor();

	marlin::Processor* newProcessor() { return new HCalTrackingNNProcessor(); }

	// the standard Marlin functions

	/** Called at the begin of the job before anything is read.
	 * Use to initialize the processor, e.g. book histograms.
	 */
	virtual void init();

	/** Called for every run.
	 */
	virtual void processRunHeader(LCRunHeader* run);

	/** Called for every event - the working horse.
	 */
	virtual void processEvent(LCEvent * evt);

	virtual void check(LCEvent * evt);

	/** Called after data processing for clean up.
	 */
	virtual void end();


protected:
    class HCalTrackedHit;

protected:
	/** returns: no. of found tracks */
	void analyzeSingleEvent(LCCollection* event);

	// CALICE Geometry
	CALICE::MappedContainer<CALICE::CellDescription>* 	_cellDescriptions;
	CALICE::MappedContainer<CALICE::CellNeighbours>* 	_cellNeighbours;
	std::string	_cellDescriptionProcessorName;
	std::string	_cellNeighbourProcessorName;

	// settings
	std::string	_lccCalorimeterHitsName;
	std::string _lccTrackClusterOutName;

	int	_maxGapSize;
	int	_minTrackLength;
	double		_maxTrackGapRatio;

	// the map
//	std::map < unsigned, HCalTrackedHit* > _hits;
//	boost::unordered_map < unsigned, HCalTrackedHit* > _hits;
	// first: z coordinate (layer k)
	typedef boost::unordered_map < unsigned, HCalTrackedHit* > hashMapHits_t;
//	typedef std::map < unsigned, HCalTrackedHit* > hashMapHits_t;
	typedef std::map< unsigned,  hashMapHits_t > mapHashmapHits_t;
	mapHashmapHits_t _hits;

	IMPL::LCCollectionVec* _tracks;

	void setTrackAngle(EVENT::Cluster* track);

	int	_statNProcessedEvents;
	int	_statNEmptyEvents;
	int	_statNRuns;
	int	_statNTracks;

	void printStatus();

protected:
	void fillHitMap(LCCollection* evt);
	void countNeighbours();
	void findTracks();
	void cleanUp();

	EVENT::Cluster* findSingleTrack(HCalTrackedHit* firstHit);
	void addTrack(EVENT::Cluster* track);
	int getTrackLayerLength(EVENT::Cluster* track);

	void deleteTrack(EVENT::Cluster* track);

protected:
	int _dbgIndentSize;

protected:
	/**
	 * Wrapper around the actual CalorimeterHit, s.t. we can add additional information like the neighbors
	 */
	class HCalTrackedHit
	{
	public:
		HCalTrackedHit(EVENT::CalorimeterHit* h) : _hit(h), _track(NULL)  {};

		EVENT::CalorimeterHit* getHit() { return _hit; }

		void addNeighbour(HCalTrackedHit* h) { _neighbours.push_back(h); }
		const std::list< HCalTrackedHit* > getNeighbours() { return _neighbours; }

		const unsigned getK() { CALICE::CellIndex idx(_hit->getCellID0()); return idx.getLayerIndex(); }

		const inline bool isIsolated() { return getNeighbours().size() == 0; }

		void setTrack(EVENT::Cluster* track) { assert(_track == NULL || track == NULL ); _track = track; }
		const bool hasTrack() { return _track != NULL; }
		const EVENT::Cluster* getTrack() { return _track; }
	protected:
		EVENT::CalorimeterHit* _hit;
		std::list< HCalTrackedHit* >	_neighbours;
		EVENT::Cluster*			_track;
	};


	friend std::ostream& operator<<(std::ostream& out, EVENT::Cluster* track);
};

std::ostream& operator<<(std::ostream& out, EVENT::CalorimeterHit* hit);
std::ostream& operator<<(std::ostream& out, EVENT::Cluster* track);

#endif /* HCALTRACKINGPROCESSOR_H_ */
