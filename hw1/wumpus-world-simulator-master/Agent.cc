// Agent.cc

#include <iostream>
#include "Agent.h"

using namespace std;

Agent::Agent ()
{

}

Agent::~Agent ()
{

}

void Agent::Initialize ()
{

}

Action Agent::Process (Percept& percept)
{
	
	Action action;
    if (percept.Glitter) {
        action = GRAB;
    }else {
        action = (Action)(rand() % 3);
    }
	return action;
}

void Agent::GameOver (int score)
{

}

