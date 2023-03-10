// Agent.h
//
// Fall 2020 HW5
//
// Author: Larry Holder

#ifndef AGENT_H
#define AGENT_H

#include "Action.h"
#include "Percept.h"

#include "WorldState.h"
#include "Location.h"
#include "Orientation.h"
#include "Search.h"
#include <list>

class Agent
{
public:
	Agent ();
	~Agent ();
	void Initialize ();
	Action Process (Percept& percept);
	void GameOver (int score);

	void UpdateState(Action lastAction, Percept& percept);
	Location SafeUnvisitedLocation();
	bool MemberLocation(Location& location, list<Location>& locationList);
	void UpdateSafeLocations(Location& location);
	void RemoveOutsideLocations();
	void AdjacentLocations(Location& location, list<Location>& adjacentLocations);
	void LocateWumpus();
    void LocatePit();
    Location findSPtoShoot();
    Orientation findSPOri(Location& wump, Location& saf);
    void AllLocs(int x, int y, list<Location>& allLocations);

	WorldState worldState;
    Orientation wumpOri;
	bool worldSizeKnown;
	list<Location> stenchLocations;
    list<Location> wumpus;
	list<Location> breezeLocations;
	list<Location> visitedLocations;
	list<Location> safeLocations;
    list<Location> unvisitedLocations;
	list<Action> actionList;
    list<Location> pitLocations;
	SearchEngine searchEngine;
	Action lastAction;
};

#endif // AGENT_H
