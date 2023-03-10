// Agent.h
//Gizem Melisa Ates/11525081
#ifndef AGENT_H
#define AGENT_H

#include "Action.h"
#include "Percept.h"

class Agent
{
public:
	Agent ();
	~Agent ();
	void Initialize ();
    void updateOriLoc (Action action);
	Action Process (Percept& percept);
	void GameOver (int score);
private:
    int orientation;
    int dir;
    int locx;
    int locy;
    bool hasArrow;
    bool hasGold;
};

#endif // AGENT_H
