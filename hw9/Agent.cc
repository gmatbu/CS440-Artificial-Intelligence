// Agent.cc

#include <iostream>
#include <list>
#include <algorithm>
#include "Agent.h"
#include "Location.h"

using namespace std;

Agent::Agent ()
{
	// Initialize new agent based on new, unknown world
    worldState.agentLocation = Location(1,1);
	worldState.agentOrientation = RIGHT;
	worldState.agentHasGold = false;
    worldState.agentHasArrow = true;
    worldState.wumpusAlive = true;
    wumpOri = RIGHT;
    unvisitedLocations.clear();
	lastAction = CLIMB; // dummy action
    worldState.worldSize = 3; // HW5: somewhere between 3x3 and 9x9
    worldSizeKnown = false; 
    worldState.wumpusLocation = Location(0,0); // unknown
	worldState.goldLocation = Location(0,0); // unknown
    pitLocations.push_back(Location(-4,-4)); // unknown
}

Agent::~Agent ()
{

}

void Agent::Initialize ()
{
	// Initialize agent back to the beginning of the world
	worldState.agentLocation = Location(1,1);
	worldState.agentOrientation = RIGHT;
	worldState.agentAlive = true;
    worldState.agentHasArrow = true;
    worldState.wumpusAlive = true;
    wumpOri = RIGHT;
    unvisitedLocations.clear();
	worldState.agentHasGold = false;
	lastAction = CLIMB; // dummy action
	actionList.clear();
}

Action Agent::Process (Percept& percept)
{
	list<Action> actionList2;
	UpdateState(lastAction, percept);
    LocatePit();
	if (actionList.empty()) {
        Location safeUnvisitedLocation1 = SafeUnvisitedLocation();
		if (percept.Glitter) {
			// If there is gold, then GRAB it
			cout << "Found gold. Grabbing it.\n";
			actionList.push_back(GRAB);
		} else if (worldState.agentHasGold && (worldState.agentLocation == Location(1,1))) {
			// If agent has gold and is in (1,1), then CLIMB
			cout << "Have gold and in (1,1). Climbing.\n";
			actionList.push_back(CLIMB);
		} else if (!worldState.agentHasGold && !(worldState.goldLocation == Location(0,0))) {
			// If agent doesn't have gold, but knows its location, then navigate to that location
			cout << "Moving to known gold location (" << worldState.goldLocation.X << "," << worldState.goldLocation.Y << ").\n";
			actionList2 = searchEngine.FindPath(worldState.agentLocation, worldState.agentOrientation, worldState.goldLocation, worldState.agentOrientation);
			actionList.splice(actionList.end(), actionList2);
		} else if (worldState.agentHasGold && !(worldState.agentLocation == Location(1,1))) {
			// If agent has gold, but isn't in (1,1), then navigate to (1,1)
			cout << "Having gold. Moving to (1,1).\n";
			actionList2 = searchEngine.FindPath(worldState.agentLocation, worldState.agentOrientation, Location(1,1), worldState.agentOrientation);
			actionList.splice(actionList.end(), actionList2);
		}
        else if (!worldState.agentHasGold && safeUnvisitedLocation1 == Location(0,0)) {
            //If the agent does not have the gold, and there are no remaining safe unvisited locations
            cout << "no gold, no safe unvisited location!\n";
            Location safeLoctoShoot = findSPtoShoot();
            wumpOri = findSPOri(worldState.wumpusLocation, safeLoctoShoot);
            if (worldState.agentHasArrow && worldState.wumpusAlive && !(worldState.wumpusLocation == Location(0,0))) {
                if (!(worldState.wumpusLocation == Location(0,0)) && !(safeLoctoShoot == Location(0,0))) {
                    //If the agent knows the location of the live Wumpus, and there is a safe location facing the Wumpus
                    cout << "know the wumpus location, and will shoot wumpus\n";
                    actionList2 = searchEngine.FindPath(worldState.agentLocation, worldState.agentOrientation, safeLoctoShoot, wumpOri);
                    actionList.splice(actionList.end(), actionList2);//move there
                    actionList.push_back(SHOOT);//shoot
                    safeLocations.push_back(worldState.wumpusLocation);
                    searchEngine.AddSafeLocation(worldState.wumpusLocation.X, worldState.wumpusLocation.Y);
                    worldState.agentHasArrow = false;
                    worldState.wumpusAlive = false;
                    
                }
            }
            else {//If the Wumpus is dead or cannot be killed, then the agent should move to an unvisited location that is not known to be unsafe
                if (!worldState.wumpusAlive ||  worldState.wumpusLocation == Location(0,0) || safeLoctoShoot == Location(0,0) ) {
                    list<Location> allLocations;
                    AllLocs(worldState.worldSize, worldState.worldSize, allLocations);
                    list<Location>::iterator iur = allLocations.begin();
                    wumpus.push_back(worldState.wumpusLocation);
                    for (int i = 1; i< allLocations.size(); i++) {
                        advance(iur, i);
                        if (!(MemberLocation(*iur, wumpus))) {
                            if (!(MemberLocation(*iur, pitLocations))) {
                                if (!(MemberLocation(*iur, visitedLocations))) {
                                    if (!(MemberLocation(*iur, unvisitedLocations))) {
                                        unvisitedLocations.push_back(*iur);
                                        cout << "unvisited loc (" << iur->X << "," << iur->Y << ").\n";
                                    }
                                }
                            }
                        }
                    }
                    searchEngine.AddSafeLocation(unvisitedLocations.front().X, unvisitedLocations.front().Y);
                    safeLocations.push_back(unvisitedLocations.front());
                    actionList2 = searchEngine.FindPath(worldState.agentLocation, worldState.agentOrientation, unvisitedLocations.front(), wumpOri);
                    actionList.splice(actionList.end(), actionList2);
                    unvisitedLocations.pop_front();
                }
                else {
                    cout << "ERROR1: no path to unvisited location that is not known to be unsafe\n"; // for debugging
                    exit(1);
                }
            }
        }
        else {
			// Determine safe unvisited location and navigate there
			Location safeUnvisitedLocation = SafeUnvisitedLocation();
			cout << "Moving to safe unvisited location (" << safeUnvisitedLocation.X << "," << safeUnvisitedLocation.Y << ").\n";
			actionList2 = searchEngine.FindPath(worldState.agentLocation, worldState.agentOrientation, safeUnvisitedLocation, worldState.agentOrientation);
			if (!(actionList2.empty())) {
				actionList.splice(actionList.end(), actionList2);
			} else {
				cout << "ERROR2: no path to safe unvisited location\n"; // for debugging
				exit(1);
			}
		}
	}
	Action action = actionList.front();
	actionList.pop_front();
	lastAction = action;
	return action;

}

void Agent::GameOver (int score)
{
	// Nothing to do here
}

void Agent::UpdateState(Action lastAction, Percept& percept) {
	int X = worldState.agentLocation.X;
	int Y = worldState.agentLocation.Y;
	Orientation orientation = worldState.agentOrientation;

	if (lastAction == TURNLEFT) {
		worldState.agentOrientation = (Orientation) ((orientation + 1) % 4);
	}
	if (lastAction == TURNRIGHT) {
		if (orientation == RIGHT) {
			worldState.agentOrientation = DOWN;
		} else {
			worldState.agentOrientation = (Orientation) (orientation - 1);
		}
	}
	if (lastAction == GOFORWARD) {
		if (percept.Bump) {
			if ((orientation == RIGHT) || (orientation == UP)) {
				cout << "World size known to be " << worldState.worldSize << "x" << worldState.worldSize << endl;
				worldSizeKnown = true;
				RemoveOutsideLocations();
			}
		} else {
			switch (orientation) {
			case UP:
				worldState.agentLocation.Y = Y + 1;
				break;
			case DOWN:
				worldState.agentLocation.Y = Y - 1;
				break;
			case LEFT:
				worldState.agentLocation.X = X - 1;
				break;
			case RIGHT:
				worldState.agentLocation.X = X + 1;
				break;
			}
		}
	}
	if (lastAction == GRAB) { // Assume GRAB only done if Glitter was present
		worldState.agentHasGold = true;
	}
	if (lastAction == CLIMB) {
		// do nothing; if CLIMB worked, this won't be executed anyway
	}
	// HW5.2 state update requirements (note: 2g no longer required)
	if ((percept.Stench) && (! MemberLocation(worldState.agentLocation, stenchLocations))) {
		stenchLocations.push_back(worldState.agentLocation);
	}
	if ((percept.Breeze) && (! MemberLocation(worldState.agentLocation, breezeLocations))) {
		breezeLocations.push_back(worldState.agentLocation);
	}
	if (percept.Glitter) {
		worldState.goldLocation = worldState.agentLocation;
		cout << "Found gold at (" << worldState.goldLocation.X << "," << worldState.goldLocation.Y << ")\n";
	}
	int new_max = max(worldState.agentLocation.X, worldState.agentLocation.Y);
	if (new_max > worldState.worldSize) {
		worldState.worldSize = new_max;
	}
	if (worldState.wumpusLocation == Location(0,0)) {
		LocateWumpus();
	}
	UpdateSafeLocations(worldState.agentLocation);
	if (!(MemberLocation(worldState.agentLocation, visitedLocations))) {
		visitedLocations.push_back(worldState.agentLocation);
	}
}

bool Agent::MemberLocation(Location& location, list<Location>& locationList) {
	if (find(locationList.begin(), locationList.end(), location) != locationList.end()) {
		return true;
	}
	return false;
}

Location Agent::SafeUnvisitedLocation() {
	// Find and return safe unvisited location.
	list<Location>::iterator loc_itr;
	for (loc_itr = safeLocations.begin(); loc_itr != safeLocations.end(); ++loc_itr) {
		if (!(MemberLocation(*loc_itr, visitedLocations))) {
			return *loc_itr;
		}
	}
	return Location(0,0);
}

void Agent::UpdateSafeLocations(Location& location) {
	// Add location to safe locations. Also add adjacent locations, if no stench or breeze.
	// Or just no breeze, if stench to be ignored.
	if (!(MemberLocation(location, safeLocations))) {
		safeLocations.push_back(location);
		searchEngine.AddSafeLocation(location.X, location.Y);
	}
	bool ignoreStench = false;
	if (!(worldState.wumpusLocation == Location(0,0))) {
		ignoreStench = true; // we know location of wumpus
	}
	if ((!(MemberLocation(location, stenchLocations)) || ignoreStench) && (!(MemberLocation(location, breezeLocations)))) {
		list<Location> adj_locs;
		AdjacentLocations(location, adj_locs);
		list<Location>::iterator loc_itr;
		for (loc_itr = adj_locs.begin(); loc_itr != adj_locs.end(); ++loc_itr) {
			if ((!(*loc_itr == worldState.wumpusLocation)) && (!(MemberLocation(*loc_itr, safeLocations)))) {
				safeLocations.push_back(*loc_itr);
				searchEngine.AddSafeLocation(loc_itr->X, loc_itr->Y);
			}
		}
	}
}

void Agent::RemoveOutsideLocations() {
	// Know exact world size, so remove locations outside the world.
	int boundary = worldState.worldSize + 1;
	for (int i = 1; i < boundary; ++i) {
		safeLocations.remove(Location(i,boundary));
		searchEngine.RemoveSafeLocation(i,boundary);
		safeLocations.remove(Location(boundary,i));
		searchEngine.RemoveSafeLocation(boundary,i);
	}
	safeLocations.remove(Location(boundary,boundary));
	searchEngine.RemoveSafeLocation(boundary,boundary);
}

void Agent::AdjacentLocations(Location& location, list<Location>& adjacentLocations) {
	// Append locations adjacent to given location on to given locations list.
	// One row/col beyond unknown world size is okay. Locations outside the world
	// will be removed later.
	int X = location.X;
	int Y = location.Y;
	if (X > 1) {
		adjacentLocations.push_back(Location(X-1,Y));
	}
	if (Y > 1) {
		adjacentLocations.push_back(Location(X,Y-1));
	}
	if (worldSizeKnown) {
		if (X < worldState.worldSize) {
			adjacentLocations.push_back(Location(X+1,Y));
		}
		if (Y < worldState.worldSize) {
			adjacentLocations.push_back(Location(X,Y+1));
		}
	} else {
		adjacentLocations.push_back(Location(X+1,Y));
		adjacentLocations.push_back(Location(X,Y+1));
	}
}

void Agent::AllLocs(int x, int y, list<Location>& allLocations) {
    for (int i =1; i<x+1; i++) {
        for (int j = 1; j<y+1; j++) {
            allLocations.push_back(Location(i,j));
        }
    }
}

Location Agent::findSPtoShoot () {
    Orientation wumpOri;
    list<Location>::iterator safe_itr = safeLocations.begin();
    for (int i =0; i< safeLocations.size(); i++) {
        advance(safe_itr, i);
        if (worldState.wumpusLocation.X == safe_itr->X) {
            if (worldState.wumpusLocation.Y > safe_itr->Y) {
                cout << "Found safeloc to shoot at (" << safe_itr->X << "," << safe_itr->Y << ")\n";
                return *safe_itr;
                break;
            }
        }
        if (worldState.wumpusLocation.Y == safe_itr->Y) {
            if (worldState.wumpusLocation.X > safe_itr->X) {
                cout << "Found safeloc to shoot at (" << safe_itr->X << "," << safe_itr->Y << ")\n";
                return *safe_itr;
                break;
            }
        }
    }
    return Location(0,0);
}

Orientation Agent::findSPOri(Location& wump, Location& saf) {
    if (wump.X == saf.X) {
        if (wump.Y > saf.Y) {
            wumpOri = UP;
        }
        else {
            wumpOri = DOWN;
        };
    }
    if (wump.Y == saf.Y) {
        if (wump.X > saf.X) {
            wumpOri = RIGHT;
        }
        else {
            wumpOri = LEFT;
        }
    }
    return wumpOri;
}



void Agent::LocateWumpus() {
	// Check stench and safe location info to see if wumpus can be located.
	// If so, then stench locations should be safe.
	list<Location>::iterator loc_itr_1;
	list<Location>::iterator loc_itr_2;
	for (loc_itr_1 = stenchLocations.begin(); loc_itr_1 != stenchLocations.end(); ++loc_itr_1) {
		int x1 = loc_itr_1->X;
		int y1 = loc_itr_1->Y;
		for (loc_itr_2 = stenchLocations.begin(); loc_itr_2 != stenchLocations.end(); ++loc_itr_2) {
			int x2 = loc_itr_2->X;
			int y2 = loc_itr_2->Y;
			Location loc = Location(x1+1,y1);
			if ((x1 == x2-1) && (y1 == y2 - 1) && (MemberLocation(loc, safeLocations))) {
				worldState.wumpusLocation = Location(x1,y1+1);
			}
			loc.X = x1;
			loc.Y = y1+1;
			if ((x1 == x2-1) && (y1 == y2 - 1) && (MemberLocation(loc, safeLocations))) {
				worldState.wumpusLocation = Location(x1+1,y1);
			}
			loc.X = x1-1;
			loc.Y = y1;
			if ((x1 == x2+1) && (y1 == y2 - 1) && (MemberLocation(loc, safeLocations))) {
				worldState.wumpusLocation = Location(x1,y1+1);
			}
			loc.X = x1;
			loc.Y = y1+1;
			if ((x1 == x2+1) && (y1 == y2 - 1) && (MemberLocation(loc, safeLocations))) {
				worldState.wumpusLocation = Location(x1-1,y1);
			}
		}
	}
	if (!(worldState.wumpusLocation == Location(0,0))) {
		cout << "Found wumpus at (" << worldState.wumpusLocation.X << "," << worldState.wumpusLocation.Y << ")\n";
	}
	
}
void Agent::LocatePit() {
    list<Location>::iterator loc_itr_1;
    list<Location>::iterator loc_itr_2;
    int sizeOfPit = pitLocations.size();
    for (loc_itr_1 = breezeLocations.begin(); loc_itr_1 != breezeLocations.end(); ++loc_itr_1) {
        int x1 = loc_itr_1->X;
        int y1 = loc_itr_1->Y;
        for (loc_itr_2 = breezeLocations.begin(); loc_itr_2 != breezeLocations.end(); ++loc_itr_2) {
            int x2 = loc_itr_2->X;
            int y2 = loc_itr_2->Y;
            Location loc = Location(x1+1,y1);
            Location loc1 = Location(x1,y1+1);
            if ((x1 == x2-1) && (y1 == y2 - 1) && (MemberLocation(loc, safeLocations)) && !(MemberLocation(loc1, pitLocations)) && !(MemberLocation(loc1, visitedLocations))) {
                pitLocations.push_back(Location(x1,y1+1));
            }
            loc.X = x1;
            loc.Y = y1+1;
            loc1 = Location(x1+1,y1);
            if ((x1 == x2-1) && (y1 == y2 - 1) && (MemberLocation(loc, safeLocations)) && !(MemberLocation(loc1, pitLocations)) && !(MemberLocation(loc1, visitedLocations))) {
                pitLocations.push_back(Location(x1+1,y1));
            }
            loc.X = x1-1;
            loc.Y = y1;
            loc1 = Location(x1,y1+1);
            if ((x1 == x2+1) && (y1 == y2 - 1) && (MemberLocation(loc, safeLocations)) && !(MemberLocation(loc1, pitLocations)) && !(MemberLocation(loc1, visitedLocations))) {
                pitLocations.push_back(Location(x1,y1+1));
                
            }
            loc.X = x1;
            loc.Y = y1+1;
            loc1 = Location(x1-1,y1);
            if ((x1 == x2+1) && (y1 == y2 - 1) && (MemberLocation(loc, safeLocations))
                && !(MemberLocation(loc1, pitLocations)) && !(MemberLocation(loc1, visitedLocations))) {
                pitLocations.push_back(Location(x1-1,y1));
            }
        }
    }
    if ((pitLocations.size() > sizeOfPit)) {
        cout << "Found pit at (" << pitLocations.back().X << "," << pitLocations.back().Y << ")\n";
    }
}
