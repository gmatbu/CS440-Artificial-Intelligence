// Agent.cc
//Gizem Melisa Ates/11525081

#include <iostream>
#include "Agent.h"

using namespace std;

Agent::Agent () {}

Agent::~Agent () {}

void Agent::Initialize ()
{
    dir = 0;
    orientation = 0;
    // x coordinate
    locx = 1;
    // y coordinate
    locy = 1;
    hasGold = false;
    hasArrow = true;
}

void Agent::updateOriLoc (Action action)
{
    if ((action == GOFORWARD) && (orientation == 0)){ //right
        locx += 1;
    }
    else if ((action == GOFORWARD) && (orientation == 1 || orientation == -3)){ //up
        locy += 1;
    }
    else if ((action == GOFORWARD) && (orientation == 2 || orientation == -2)){//left
        locx -= 1;
    }
    else if ((action == GOFORWARD) && (orientation == -1 || orientation == 3)){//down
        locy -= 1;
    }
    else if (action == TURNRIGHT){//turning left is -=1
        dir -= 1;
        orientation = (dir % 4);//orientation is always -3(up),-2(left),-1(down),0(right),1(up),2(left),3(down)
    }
    else if (action == TURNLEFT){//turning right is +=1
        dir += 1;
        orientation = (dir % 4);
    }else{
        locx += 0;
        locy += 0;
    }
}

Action Agent::Process (Percept& percept)
{
	Action action;
    // if the agent bumps into a wall then take back the addition/substraction
    if (percept.Bump && orientation == 0){//take back for right
        locx -= 1;
        action = (Action) (rand() % 3);
        updateOriLoc(action);
    }
    else if (percept.Bump && (orientation == 1 || orientation == -3)){//take back for up
        locy -= 1;
        action = (Action) (rand() % 3);
        updateOriLoc(action);
    }
    else if (percept.Bump && (orientation == 2 || orientation == -2)){//take back for left
        locx += 1;
        action = (Action) (rand() % 3);
        updateOriLoc(action);
    }
    else if (percept.Bump && (orientation == -1 || orientation == 3)){//take back for down
        locy += 1;
        action = (Action) (rand() % 3);
        updateOriLoc(action);
    }
    else if (percept.Glitter){ //hw-a
        action = GRAB;
        hasGold = true;
    }
    else if (hasGold && (locx == 1 && locy == 1)) {//hw-b
        action = CLIMB;
    }
    else if (hasArrow && (locy == 4) && (orientation == 0)) {//hw-c
        action = SHOOT;
        hasArrow = false;
    }
    else if (hasArrow && (locx == 4) && (orientation == 1 || orientation == -3)) {//hw-d
        action = SHOOT;
        hasArrow = false;
    }
    else {
        action = (Action) (rand() % 3);//hw-e
        updateOriLoc(action);
    }
	return action;
}

void Agent::GameOver (int score)
{

}

