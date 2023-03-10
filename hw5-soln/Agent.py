# Agent.py
#
# Fall 2020 HW5 Solution
#
# Author: Larry Holder

import Action
import Orientation
import Search
import sys
    
class Agent:
    def __init__(self):
        # Initialize new agent based on new, unknown world
        self.agentLocation = [1,1]
        self.agentOrientation = Orientation.RIGHT
        self.agentHasGold = False
        self.lastAction = Action.CLIMB # dummy action
        self.actionList = []
        self.searchEngine = Search.SearchEngine()
        self.worldSize = 3 # HW5: somewhere between 3x3 and 9x9
        self.worldSizeKnown = False 
        self.wumpusLocation = [0,0] # unknown
        self.goldLocation = [0,0] # unknown
        self.stenchLocations = []
        self.breezeLocations = []
        self.visitedLocations = []
        self.safeLocations = []
    
    def Initialize(self):        
        # Initialize agent back to the beginning of the world
        self.agentLocation = [1,1]
        self.agentOrientation = Orientation.RIGHT
        self.agentHasGold = False
        self.lastAction = Action.CLIMB # dummy action
        self.actionList = []
        
    def Process (self, percept):
        actionList2 = []
        self.UpdateState(self.lastAction, percept)
        if (self.actionList == []):
            if (percept.glitter):
                # If there is gold, then GRAB it
                print("Found gold. Grabbing it.")
                self.actionList.append(Action.GRAB)
            elif (self.agentHasGold and (self.agentLocation == [1,1])):
                # If agent has gold and is in (1,1), then CLIMB
                print("Have gold and in (1,1). Climbing.")
                self.actionList.append(Action.CLIMB)
            elif ((not self.agentHasGold) and (self.goldLocation != [0,0])):
                # If agent doesn't have gold, but knows its location, then navigate to that location
                print("Moving to known gold location (" + str(self.goldLocation[0]) + "," + str(self.goldLocation[1]) + ").")
                actionList2 = self.searchEngine.FindPath(self.agentLocation, self.agentOrientation, self.goldLocation, self.agentOrientation)
                self.actionList.extend(actionList2)
            elif (self.agentHasGold and (self.agentLocation != [1,1])):
                # If agent has gold, but isn't in (1,1), then navigate to (1,1)
                print("Having gold. Moving to (1,1).")
                actionList2 = self.searchEngine.FindPath(self.agentLocation, self.agentOrientation, [1,1], self.agentOrientation)
                self.actionList.extend(actionList2)
            else:
                # Determine safe unvisited location and navigate there (there will always be one for HW5)
                safeUnvisitedLocation = self.SafeUnvisitedLocation()
                print("Moving to safe unvisited location " + str(safeUnvisitedLocation))
                actionList2 = self.searchEngine.FindPath(self.agentLocation, self.agentOrientation, safeUnvisitedLocation, self.agentOrientation)
                if actionList2:
                    self.actionList.extend(actionList2)
                else:
                    print("ERROR: no path to safe unvisited location") # for debugging
                    sys.exit(1)
        action = self.actionList.pop(0)
        self.lastAction = action
        return action
    
    def UpdateState(self, lastAction, percept):
        X = self.agentLocation[0]
        Y = self.agentLocation[1]
        orientation = self.agentOrientation
        if (lastAction == Action.TURNLEFT):
            self.agentOrientation = (orientation + 1) % 4
        if (lastAction == Action.TURNRIGHT):
            if (orientation == Orientation.RIGHT):
                self.agentOrientation = Orientation.DOWN
            else:
                self.agentOrientation = orientation - 1
        if (lastAction == Action.GOFORWARD):
            if percept.bump:
                if (orientation == Orientation.RIGHT) or (orientation == Orientation.UP):
                    print("World size known to be " + str(self.worldSize) + "x" + str(self.worldSize))
                    self.worldSizeKnown = True
                    self.RemoveOutsideLocations()
            else:
                if orientation == Orientation.UP:
                    self.agentLocation = [X,Y+1]
                elif orientation == Orientation.DOWN:
                    self.agentLocation = [X,Y-1]
                elif orientation == Orientation.LEFT:
                    self.agentLocation = [X-1,Y]
                elif orientation == Orientation.RIGHT:
                    self.agentLocation = [X+1,Y]
        if (lastAction == Action.GRAB): # Assume GRAB only done if Glitter was present
                self.agentHasGold = True
        if (lastAction == Action.CLIMB):
            pass # do nothing; if CLIMB worked, this won't be executed anyway
        # HW5.2 state update requirements (note: 2g no longer required)
        if percept.stench and (self.agentLocation not in self.stenchLocations):
            self.stenchLocations.append(self.agentLocation)
        if percept.breeze and (self.agentLocation not in self.breezeLocations):
            self.breezeLocations.append(self.agentLocation)
        if percept.glitter:
            self.goldLocation = self.agentLocation
            print("Found gold at " + str(self.goldLocation))
        new_max = max(self.agentLocation[0], self.agentLocation[1])
        if new_max > self.worldSize:
            self.worldSize = new_max
        if (self.wumpusLocation == [0,0]):
            self.LocateWumpus()
        self.UpdateSafeLocations(self.agentLocation)
        if self.agentLocation not in self.visitedLocations:
            self.visitedLocations.append(self.agentLocation)
        
        
    def GameOver(self, score):
        pass
    
    def UpdateSafeLocations(self, location):
        # Add location to safe locations. Also add adjacent locations, if no stench or breeze.
        # Or just no breeze, if stench to be ignored
        if location not in self.safeLocations:
            self.safeLocations.append(location)
            self.searchEngine.AddSafeLocation(location[0], location[1])
        ignore_stench = False
        if (self.wumpusLocation != [0,0]):
            ignore_stench = True # we know location of wumpus
        if ((location not in self.stenchLocations) or ignore_stench) and (location not in self.breezeLocations):
            for adj_loc in self.AdjacentLocations(location):
                if (adj_loc != self.wumpusLocation) and (adj_loc not in self.safeLocations):
                    self.safeLocations.append(adj_loc)
                    self.searchEngine.AddSafeLocation(adj_loc[0], adj_loc[1])
        
    def SafeUnvisitedLocation(self):
        # Find and return safe unvisited location
        for location in self.safeLocations:
            if location not in self.visitedLocations:
                return location
        return None
    
    def RemoveOutsideLocations(self):
        # Know exact world size, so remove locations outside the world.
        boundary = self.worldSize + 1
        for i in range(1,boundary):
            if [i,boundary] in self.safeLocations:
                self.safeLocations.remove([i,boundary])
                self.searchEngine.RemoveSafeLocation(i, boundary)
            if [boundary, i] in self.safeLocations:
                self.safeLocations.remove([boundary, i])
                self.searchEngine.RemoveSafeLocation(boundary, i)
        if [boundary, boundary] in self.safeLocations:
            self.safeLocations.remove([boundary, boundary])
            self.searchEngine.RemoveSafeLocation(boundary, boundary)
    
    def AdjacentLocations(self, location):
        # Return list of locations adjacent to given location. One row/col beyond unknown
        # world size is okay. Locations outside the world will be removed later.
        X = location[0]
        Y = location[1]
        adj_locs = []
        if X > 1:
            adj_locs.append([X-1,Y])
        if Y > 1:
            adj_locs.append([X,Y-1])
        if self.worldSizeKnown:
            if (X < self.worldSize):
                adj_locs.append([X+1,Y])
            if (Y < self.worldSize):
                adj_locs.append([X,Y+1])
        else:
            adj_locs.append([X+1,Y])
            adj_locs.append([X,Y+1])
        return adj_locs
    
    def LocateWumpus(self):
        # Check stench and safe location info to see if wumpus can be located.
        # If so, then stench locations should be safe.
        for stench_location_1 in self.stenchLocations:
            x1 = stench_location_1[0]
            y1 = stench_location_1[1]
            for stench_location_2 in self.stenchLocations:
                x2 = stench_location_2[0]
                y2 = stench_location_2[1]
                if (x1 == x2-1) and (y1 == y2 - 1) and ([x1+1,y1] in self.safeLocations):
                    self.wumpusLocation = [x1,y1+1]
                if (x1 == x2-1) and (y1 == y2 - 1) and ([x1,y1+1] in self.safeLocations):
                    self.wumpusLocation = [x1+1,y1]
                if (x1 == x2+1) and (y1 == y2 - 1) and ([x1-1,y1] in self.safeLocations):
                    self.wumpusLocation = [x1,y1+1]
                if (x1 == x2+1) and (y1 == y2 - 1) and ([x1,y1+1] in self.safeLocations):
                    self.wumpusLocation = [x1-1,y1]
        if (self.wumpusLocation != [0,0]):
            print("Found wumpus at " + str(self.wumpusLocation))
