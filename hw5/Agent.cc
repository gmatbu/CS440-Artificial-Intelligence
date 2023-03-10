// Agent.cc

#include <iostream>
#include <algorithm>
#include <list>
#include <vector>
#include "Agent.h"

#include "Action.h"
#include "Location.h"
#include "Orientation.h"

using namespace std;

Agent::Agent ()
{

}

Agent::~Agent ()
{

}

void Agent::Initialize ()
{
    worldState.agentLocation = Location(1,1);
    searchEngine.AddSafeLocation(1,1);
    worldState.agentOrientation = RIGHT;
    worldState.agentHasArrow = true;
    worldState.agentHasGold = false;
    previousAction = CLIMB; // dummy action to start
    //agentHasGold = false;
    actionList.clear();
}

vector<Location> UnVisitedSafeLoc;
vector<Location> SafeLocs;
vector<Location> UnSafeLocs;
vector<Location> UnSafeLocsDueToStench;
vector<Location> UnSafeLocsDueToBreeze;
vector<Location> SafeLocsDueToWumpus;
vector<Location> SafeLocsDueToBreeze;
vector<Location> VisitedLocs;
Location WumpusLoc = Location(0,0);
vector<Location> PitLocs;
Location tmpPitLocs = Location(0,0);
vector<Location> StenchLocs;
vector<Location> BreezeLocs;
int max_y = 0;
int Rand2 = 0;
Location SelectedInUnVisited;
bool checker = true;


Action Agent::Process (Percept& percept)
{
    list<Action> actionList2;
    actionList2.clear();
    UpdateState(percept);
    VisitedLocs.push_back(worldState.agentLocation);
    VisitedLocs = RemoveDublicateFromVectorLocation(VisitedLocs);
    
    
    for (int i = 0; i< VisitedLocs.size(); i++)//printing visited locations
    {
        cout<<"visitedLocs: ("<< VisitedLocs.at(i).X<<","<<VisitedLocs.at(i).Y<<")\n";
    }
    UnVisitedSafeLoc = RemoveDublicateFromVectorLocation(UnVisitedSafeLoc);
    for (int i = 0; i< UnVisitedSafeLoc.size(); i++)//printing unvisited locations
    {
        cout<<"UNVISITED SAFE LOCS: ("<< UnVisitedSafeLoc.at(i).X<<","<<UnVisitedSafeLoc.at(i).Y<<")\n";
    }
    for (int i = 0; i< PitLocs.size(); i++)//printing pit locations
    {
        cout<<"PIT LOCS: ("<< PitLocs.at(i).X<<","<<PitLocs.at(i).Y<<")\n";
    }
    

    max_y = MaxOfMax(VisitedLocs);
    
    if (max_y < 3){//2c Updating the world size to max(X,Y) of my current location.
        max_y = 3;
    }
    else if(max_y >= 3 && max_y <= 9){
        max_y = max_y;
    }
    else{
        max_y = 9;
    }

    for(int i=0; i< VisitedLocs.size(); i++)//add visited loc to safe locs
    {
        SafeLocs.push_back(VisitedLocs.at(i));
    }
    SafeLocs = RemoveDublicateFromVectorLocation(SafeLocs);//remove duplicates
    
    if (percept.Stench || percept.Breeze)//if agent feels stench or breeze, the adjacents of current loc will become unsafelocs
    {
        if (percept.Stench && !percept.Breeze)
        {
            StenchLocs.push_back(worldState.agentLocation);
            for(int i=0; i< AdjacentLocationsALL(worldState.agentLocation).size(); i++)//adds every adjacent of a stench loc to unsafe
            {
                UnSafeLocs.push_back(AdjacentLocationsALL(worldState.agentLocation).at(i));
                if (AdjacentLocations(worldState.agentLocation, max_y).size() > i)
                {
                    UnSafeLocsDueToStench.push_back(AdjacentLocations(worldState.agentLocation, max_y).at(i));
                }
                
            }
            UnSafeLocs = RemoveDublicateFromVectorLocation(UnSafeLocs);
            UnSafeLocs = (NotInVector(UnSafeLocs, SafeLocs));//set unsafe locs to locations that exists in unsafelocs but not in safelocs
            cout<<"unsafe loc size:"<< UnSafeLocs.size()<< "\n";
            RemoveDublicateFromVectorLocation(StenchLocs);
            UnSafeLocsDueToStench = RemoveDublicateFromVectorLocation(UnSafeLocsDueToStench);
            for (int i = 0; i< UnSafeLocs.size(); i++)//printing unsafe locs
            {
                searchEngine.RemoveSafeLocation(UnSafeLocs.at(i).X, UnSafeLocs.at(i).Y);
                cout<<"unsafe Locs: ("<< UnSafeLocs.at(i).X<<","<<UnSafeLocs.at(i).Y<<")\n";
            }
    
            cout<<"wumpus is at loc: ("<< WumpusLoc.X<<","<<WumpusLoc.Y<<")\n";
        }
        else if (percept.Stench)
        {
            StenchLocs.push_back(worldState.agentLocation);
            for(int i=0; i< AdjacentLocationsALL(worldState.agentLocation).size(); i++)//adds every adjacent of a stench loc to unsafe
            {
                UnSafeLocs.push_back(AdjacentLocationsALL(worldState.agentLocation).at(i));
            }
            UnSafeLocs = RemoveDublicateFromVectorLocation(UnSafeLocs);
            UnSafeLocs = (NotInVector(UnSafeLocs, SafeLocs));//set unsafe locs to locations that exists in unsafelocs but not in safelocs
            cout<<"unsafe loc size:"<< UnSafeLocs.size()<< "\n";
            RemoveDublicateFromVectorLocation(StenchLocs);
            UnSafeLocsDueToStench = RemoveDublicateFromVectorLocation(UnSafeLocsDueToStench);
            for (int i = 0; i< UnSafeLocs.size(); i++)//printing unsafe locs
            {
                searchEngine.RemoveSafeLocation(UnSafeLocs.at(i).X, UnSafeLocs.at(i).Y);
                cout<<"unsafe LocsStench: ("<< UnSafeLocs.at(i).X<<","<<UnSafeLocs.at(i).Y<<")\n";
            }
    
            cout<<"wumpus is at loc: ("<< WumpusLoc.X<<","<<WumpusLoc.Y<<")\n";
        }
        if (percept.Breeze && !percept.Stench)
        {
            BreezeLocs.push_back(worldState.agentLocation);
            BreezeLocs = RemoveDublicateFromVectorLocation(BreezeLocs);
            for(int j=0; j< AdjacentLocationsALL(worldState.agentLocation).size(); j++)//adds every adjacent of a stench loc to unsafe
            {
                UnSafeLocs.push_back(AdjacentLocationsALL(worldState.agentLocation).at(j));
                if (AdjacentLocations(worldState.agentLocation, max_y).size() > j)
                {
                    UnSafeLocsDueToBreeze.push_back(AdjacentLocations(worldState.agentLocation, max_y).at(j));
                }
                
            }
            UnSafeLocs = RemoveDublicateFromVectorLocation(UnSafeLocs);
            UnSafeLocs = (NotInVector(UnSafeLocs, SafeLocs));//set unsafe locs to locations that exists in unsafelocs but not in safelocs
            cout<<"unsafe loc size:"<< UnSafeLocs.size()<< "\n";
            BreezeLocs = RemoveDublicateFromVectorLocation(BreezeLocs);
            UnSafeLocsDueToBreeze = RemoveDublicateFromVectorLocation(UnSafeLocsDueToBreeze);
            for (int i = 0; i< UnSafeLocs.size(); i++)//printing unsafe locs
            {
                searchEngine.RemoveSafeLocation(UnSafeLocs.at(i).X, UnSafeLocs.at(i).Y);
                cout<<"unsafe Locs after breeze : ("<< UnSafeLocs.at(i).X<<","<<UnSafeLocs.at(i).Y<<")\n";
            }
        }
        
        SafeLocs = NotInVector(SafeLocs, UnSafeLocs);//update safe locs
        
        if (SafeLocs.size() != 0 && StenchLocs.size() != 0 && checker)//finds wumpus loc
        {
            if (FindWumpus(StenchLocs, SafeLocs).X != 0 )
            {
                WumpusLoc = FindWumpus(StenchLocs, SafeLocs);
                checker = false;
                SafeLocsDueToWumpus = NotIn(UnSafeLocsDueToStench, WumpusLoc);
                for (int i = 0; i < SafeLocsDueToWumpus.size(); i++)
                {
                    SafeLocs.push_back(SafeLocsDueToWumpus.at(i));
                    searchEngine.AddSafeLocation(SafeLocsDueToWumpus.at(i).X, SafeLocsDueToWumpus.at(i).Y);
                }
            }
        }
        SafeLocs = NotInVector(SafeLocs, UnSafeLocs);//update safe locs
        for(int i=0; i< VisitedLocs.size(); i++)//add visited loc to safe locs
        {
            SafeLocs.push_back(VisitedLocs.at(i));
        }
        SafeLocs = RemoveDublicateFromVectorLocation(SafeLocs);
        if (SafeLocs.size() != 0 && BreezeLocs.size() > 1) //finds pit loc
        {
            if (FindPit(BreezeLocs, SafeLocs).X != 0 )
            {
                tmpPitLocs = FindPit(BreezeLocs, SafeLocs);//pit location
                PitLocs.push_back(tmpPitLocs);
                PitLocs = RemoveDublicateFromVectorLocation(PitLocs);
                for (int i=0; i<PitLocs.size(); i++)
                {
                    for(int j=0; j<AdjacentLocationsALL(PitLocs.at(i)).size(); j++)
                    {
                        SafeLocsDueToBreeze.push_back(AdjacentLocationsALL(PitLocs.at(i)).at(j));
                    }
                }
                SafeLocsDueToBreeze = checkifValidLoc(SafeLocsDueToBreeze, max_y);
                SafeLocsDueToBreeze = NotInVector(SafeLocsDueToBreeze , PitLocs);
                SafeLocsDueToBreeze = RemoveDublicateFromVectorLocation(SafeLocsDueToBreeze);
                for (int i = 0; i < SafeLocsDueToBreeze.size(); i++)
                {
                    SafeLocs.push_back(SafeLocsDueToBreeze.at(i));
                    searchEngine.AddSafeLocation(SafeLocsDueToBreeze.at(i).X, SafeLocsDueToBreeze.at(i).Y);
                }
                for (int i = 0; i< PitLocs.size(); i++)//printing safe locs
                {
                    cout<<"pit Locs: ("<< PitLocs.at(i).X<<","<<PitLocs.at(i).Y<<")\n";
                }
                //BreezeLocs = NotInVector(BreezeLocs, SafeLocsDueToBreeze);
            }
            
        }
        
        SafeLocs = RemoveDublicateFromVectorLocation(SafeLocs);
        
    }
    cout<<"wumpus locs: ("<< WumpusLoc.X<<","<<WumpusLoc.Y<<")\n";
    SafeLocs = RemoveDublicateFromVectorLocation(SafeLocs);
    for (int i = 0; i< SafeLocs.size(); i++)//printing safe locs
    {
        cout<<"Safe Locs: ("<< SafeLocs.at(i).X<<","<<SafeLocs.at(i).Y<<")\n";
    }
    if (!percept.Stench && !percept.Breeze)//if no stench and breeze, add all adjacents to SafeLocs
    {
        for(int i=0; i< AdjacentLocations(worldState.agentLocation, max_y).size(); i++)//add every adjacent loc to safelocs
        {
            SafeLocs.push_back(AdjacentLocations(worldState.agentLocation, max_y).at(i));
            searchEngine.AddSafeLocation(AdjacentLocations(worldState.agentLocation, max_y).at(i).X, AdjacentLocations(worldState.agentLocation, max_y).at(i).Y);
        }
        SafeLocs = NotInVector(SafeLocs, UnSafeLocs);//set safelocs to locations that exists in safelocs but not in unsafe locs
        if (SafeLocs.size() != 0 && StenchLocs.size() != 0 && checker)
        {
            if (FindWumpus(StenchLocs, SafeLocs).X != 0 )
            {
                WumpusLoc = FindWumpus(StenchLocs, SafeLocs);
                checker = false;
                SafeLocsDueToWumpus = NotIn(UnSafeLocsDueToStench, WumpusLoc);
                for (int i = 0; i < SafeLocsDueToWumpus.size(); i++)
                {
                    SafeLocs.push_back(SafeLocsDueToWumpus.at(i));
                    searchEngine.AddSafeLocation(SafeLocsDueToWumpus.at(i).X, SafeLocsDueToWumpus.at(i).Y);
                }
                
            }
        }
        SafeLocs = NotInVector(SafeLocs, UnSafeLocs);//update safe locs
        for(int i=0; i< VisitedLocs.size(); i++)//add visited loc to safe locs
        {
            SafeLocs.push_back(VisitedLocs.at(i));
        }
        SafeLocs = RemoveDublicateFromVectorLocation(SafeLocs);
        if (SafeLocs.size() != 0 && BreezeLocs.size() > 1)//finds pit loc
        {
            if (FindPit(BreezeLocs, SafeLocs).X != 0 )
            {
                tmpPitLocs = FindPit(BreezeLocs, SafeLocs);//pit location
                PitLocs.push_back(tmpPitLocs);
                PitLocs = RemoveDublicateFromVectorLocation(PitLocs);
                for (int i=0; i<PitLocs.size(); i++)
                {
                    for(int j=0; j<AdjacentLocationsALL(PitLocs.at(i)).size(); j++)
                    {
                        SafeLocsDueToBreeze.push_back(AdjacentLocationsALL(PitLocs.at(i)).at(j));
                    }
                }
                SafeLocsDueToBreeze = checkifValidLoc(SafeLocsDueToBreeze, max_y);
                SafeLocsDueToBreeze = NotInVector(SafeLocsDueToBreeze , PitLocs);
                SafeLocsDueToBreeze = RemoveDublicateFromVectorLocation(SafeLocsDueToBreeze);
                for (int i = 0; i < SafeLocsDueToBreeze.size(); i++)
                {
                    SafeLocs.push_back(SafeLocsDueToBreeze.at(i));
                    searchEngine.AddSafeLocation(SafeLocsDueToBreeze.at(i).X, SafeLocsDueToBreeze.at(i).Y);
                }
                for (int i = 0; i< PitLocs.size(); i++)//printing safe locs
                {
                    cout<<"pit Locs: ("<< PitLocs.at(i).X<<","<<PitLocs.at(i).Y<<")\n";
                }
                //BreezeLocs = NotInVector(BreezeLocs, SafeLocsDueToBreeze);
            }
            
        }
        SafeLocs = RemoveDublicateFromVectorLocation(SafeLocs);
        cout<<"safe loc size:"<< SafeLocs.size()<< "\n";
        UnSafeLocs = NotInVector(UnSafeLocs, SafeLocs);
    }
    
    UnVisitedSafeLoc = NotInVector(SafeLocs, VisitedLocs);//set unvisited safe locs to locations that exists in safelocs but not in visited locs
    
    
    if (percept.Glitter)
    {
        actionList.clear();
        actionList2 = searchEngine.FindPath(worldState.agentLocation, worldState.agentOrientation, Location(1,1), RIGHT);
        actionList.splice(actionList.end(), actionList2);
        previousAction = GRAB;
        worldState.agentHasGold = true;
        return GRAB;
    }
    
    if (worldState.agentHasGold && worldState.agentLocation.X == 1 && worldState.agentLocation.Y == 1)
    {
        return CLIMB;
    }
    
    if (actionList.empty())
    {
        cout<<"actionlist empty \n";
        if (UnVisitedSafeLoc.size() == 0)
        {
            cout<<"UnVisitedSafeLoc.size() == 0 \n";
            max_y += 1;
            
            for(int i=max_y; i>=1; i--)//expand world size in the y-direction and save as safelocs
            {
                searchEngine.AddSafeLocation(i, max_y);
                SafeLocs.push_back(Location(i, max_y));
            }
            
            
            
            for(int i=max_y; i>=1; i--)//expand world size in the x-direction and save as safelocs
            {
                searchEngine.AddSafeLocation(max_y, i);
                SafeLocs.push_back(Location(max_y, i));
            }
            for (int i = 0; i< UnSafeLocs.size(); i++)//printing unsafe locs
            {
                searchEngine.RemoveSafeLocation(UnSafeLocs.at(i).X, UnSafeLocs.at(i).Y);
                cout<<"unsafe Locs: ("<< UnSafeLocs.at(i).X<<","<<UnSafeLocs.at(i).Y<<")\n";
            }
            SafeLocs = NotInVector(SafeLocs, UnSafeLocs);//set safe locs to locations that exists in safelocs but not in unsafe
            
            SafeLocs = RemoveDublicateFromVectorLocation(SafeLocs);
            
            if (SafeLocs.size() != 0 && StenchLocs.size() != 0 && checker)
            {
                if (FindWumpus(StenchLocs, SafeLocs).X != 0 )
                {
                    WumpusLoc = FindWumpus(StenchLocs, SafeLocs);
                    checker = false;
                    SafeLocsDueToWumpus = NotIn(UnSafeLocsDueToStench, WumpusLoc);
                    for (int i = 0; i < SafeLocsDueToWumpus.size(); i++)
                    {
                        SafeLocs.push_back(SafeLocsDueToWumpus.at(i));
                        searchEngine.AddSafeLocation(SafeLocsDueToWumpus.at(i).X, SafeLocsDueToWumpus.at(i).Y);
                    }
                    
                }
            }
            SafeLocs = NotInVector(SafeLocs, UnSafeLocs);//update safe locs
            for(int i=0; i< VisitedLocs.size(); i++)//add visited loc to safe locs
            {
                SafeLocs.push_back(VisitedLocs.at(i));
            }
            SafeLocs = RemoveDublicateFromVectorLocation(SafeLocs);
            if (SafeLocs.size() != 0 && BreezeLocs.size() > 1)//finds pit loc
            {
                if (FindPit(BreezeLocs, SafeLocs).X != 0 )
                {
                    tmpPitLocs = FindPit(BreezeLocs, SafeLocs);//pit location
                    PitLocs.push_back(tmpPitLocs);
                    PitLocs = RemoveDublicateFromVectorLocation(PitLocs);
                    for (int i=0; i<PitLocs.size(); i++)
                    {
                        for(int j=0; j<AdjacentLocationsALL(PitLocs.at(i)).size(); j++)
                        {
                            SafeLocsDueToBreeze.push_back(AdjacentLocationsALL(PitLocs.at(i)).at(j));
                        }
                    }
                    SafeLocsDueToBreeze = checkifValidLoc(SafeLocsDueToBreeze, max_y);
                    SafeLocsDueToBreeze = NotInVector(SafeLocsDueToBreeze , PitLocs);
                    SafeLocsDueToBreeze = RemoveDublicateFromVectorLocation(SafeLocsDueToBreeze);
                    for (int i = 0; i < SafeLocsDueToBreeze.size(); i++)
                    {
                        SafeLocs.push_back(SafeLocsDueToBreeze.at(i));
                        searchEngine.AddSafeLocation(SafeLocsDueToBreeze.at(i).X, SafeLocsDueToBreeze.at(i).Y);
                    }
                    for (int i = 0; i< PitLocs.size(); i++)//printing safe locs
                    {
                        cout<<"pit Locs: ("<< PitLocs.at(i).X<<","<<PitLocs.at(i).Y<<")\n";
                    }
                }
                
            }
            UnVisitedSafeLoc = NotInVector(SafeLocs, VisitedLocs);
            Rand2 = rand() % (UnVisitedSafeLoc.size());
            cout<<"UnVisited safe loc selected1: ("<< UnVisitedSafeLoc.at(Rand2).X<< ","<<UnVisitedSafeLoc.at(Rand2).Y<<")\n";
            actionList2 = searchEngine.FindPath(worldState.agentLocation, worldState.agentOrientation, UnVisitedSafeLoc.at(Rand2), RIGHT);
            if(actionList2.empty())//if there is not safe path to unvisited safe loc then go to another unvisited safe loc
            {
                cout<<"actionlist2 is empty";
                while(actionList2.empty() && UnVisitedSafeLoc.size() != 1)
                {
                    Rand2 = rand() % (UnVisitedSafeLoc.size());
                    actionList2 = searchEngine.FindPath(worldState.agentLocation, worldState.agentOrientation, UnVisitedSafeLoc.at(Rand2), RIGHT);
                    
                }
                
            }
            actionList.splice(actionList.end(), actionList2);
            
            Action action = actionList.front();
            actionList.pop_front();
            previousAction = action;
            return action;

        }
        else
        {
            Rand2 = rand() % (UnVisitedSafeLoc.size());
            cout<<"UnVisited safe loc selected2: ("<< UnVisitedSafeLoc.at(Rand2).X<< ","<<UnVisitedSafeLoc.at(Rand2).Y<<")\n";
            actionList2 = searchEngine.FindPath(worldState.agentLocation, worldState.agentOrientation, UnVisitedSafeLoc.at(Rand2), RIGHT);
            if(actionList2.empty())//if there is not safe path to unvisited safe loc then go to another unvisited safe loc
            {
                cout<<"actionlist2 is empty";
                while(actionList2.empty() && UnVisitedSafeLoc.size() != 1)
                {
                    Rand2 = rand() % (UnVisitedSafeLoc.size());
                    actionList2 = searchEngine.FindPath(worldState.agentLocation, worldState.agentOrientation, UnVisitedSafeLoc.at(Rand2), RIGHT);
                    
                }
                
            }
            actionList.splice(actionList.end(), actionList2);
            
            Action action = actionList.front();
            actionList.pop_front();
            previousAction = action;
            return action;
        }
    }
    Action action = actionList.front();
    actionList.pop_front();
    previousAction = action;
    return action;
}

void Agent::GameOver (int score)
{

}
void Agent::UpdateState (Percept& percept) {
    int orientationInt = (int) worldState.agentOrientation;
    switch (previousAction) {
    case GOFORWARD:
        if (! percept.Bump) {
            Move();
        }
        break;
    case TURNLEFT:
        worldState.agentOrientation = (Orientation) ((orientationInt + 1) % 4);
        break;
    case TURNRIGHT:
        orientationInt--;
        if (orientationInt < 0) orientationInt = 3;
        worldState.agentOrientation = (Orientation) orientationInt;
        break;
    case GRAB:
        worldState.agentHasGold = true; // Only GRAB when there's gold
        break;
    case SHOOT:
        worldState.agentHasArrow = false;
    case CLIMB:
        break;
    }
}
    
void Agent::Move() {
    switch (worldState.agentOrientation) {
    case RIGHT:
        worldState.agentLocation.X++;
        break;
    case UP:
        worldState.agentLocation.Y++;
        break;
    case LEFT:
        worldState.agentLocation.X--;
        break;
    case DOWN:
        worldState.agentLocation.Y--;
        break;
    }
}


