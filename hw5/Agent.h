// Agent.h
// Agent.h

#ifndef AGENT_H
#define AGENT_H

#include "Action.h"
#include "Percept.h"
#include <vector>
#include "Location.h"
#include "Orientation.h"
#include "Search.h"
#include "WorldState.h"
#include <list>

class Agent
{
public:
    Agent ();
    ~Agent ();
    void Initialize ();
    Action Process (Percept& percept);
    void GameOver (int score);
    
    void UpdateState (Percept& percept);
    void Move();

    WorldState worldState;
    list<Action> actionList;
    Action previousAction;
    SearchEngine searchEngine;
    static int MaxOfMax(vector<Location> x)
    {
        int a = 0;
        for (int i = 0; i < x.size(); i++)
        {
            if (x.at(i).X > a)
            {
                a = x.at(i).X;
            }
            if (x.at(i).Y > x.at(i).X)
            {
                a = x.at(i).Y;
            }
        }
        return a;
    }
    static bool InIt (Location x, vector<Location> y)
    {
        int a = 0;
        for (int i = 0; i < y.size(); i++)
        {
            if (x.X == y.at(i).X && x.Y == y.at(i).Y)
            {
                a += 1;
            }
        }
        if (a == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    static vector<Location> AdjacentLocations(Location x, int max_y)
    {
        int max_x = max_y;
        vector<Location> tmp;
        if(x.X == 1 && x.Y == 1){
            //cout<<"\n girdi 1 \n";
            tmp.push_back(Location(x.X, x.Y+1));
            tmp.push_back(Location(x.X+1, x.Y));
        }
        else if(x.X == 1 && x.Y == max_y){
            //cout<<"\n girdi 2 \n";
            tmp.push_back(Location(x.X+1, x.Y));
            tmp.push_back(Location(x.X, x.Y-1));
        }
        else if(x.X == max_x && x.Y == 1){
            //cout<<"\n girdi 3 \n";
            tmp.push_back(Location(x.X-1, x.Y));
            tmp.push_back(Location(x.X, x.Y+1));
        }
        else if(x.X == max_x && x.Y == max_y){
            //cout<<"\n girdi 4 \n";
            tmp.push_back(Location(x.X, x.Y-1));
            tmp.push_back(Location(x.X-1, x.Y));
        }
        else if(x.X == 1){
         //cout<<"\n girdi 5 \n";
            tmp.push_back(Location(x.X, x.Y+1));
            tmp.push_back(Location(x.X, x.Y-1));
            tmp.push_back(Location(x.X+1, x.Y));
         
        }
        else if(x.Y == 1){
            //cout<<"\n girdi 6 \n";
            tmp.push_back(Location(x.X+1, x.Y));
            tmp.push_back(Location(x.X-1, x.Y));
            tmp.push_back(Location(x.X, x.Y+1));
        }
        else if(x.X == max_x){
            //cout<<"\n girdi 7 \n";
            tmp.push_back(Location(x.X, x.Y+1));
            tmp.push_back(Location(x.X, x.Y-1));
            tmp.push_back(Location(x.X-1, x.Y));
        }
        else if(x.Y == max_y){
            //cout<<"\n girdi 8 \n";
            tmp.push_back(Location(x.X+1, x.Y));
            tmp.push_back(Location(x.X-1, x.Y));
            tmp.push_back(Location(x.X, x.Y-1));
        }
        else{
            //cout<<"\n girdi 9 \n";
            tmp.push_back(Location(x.X-1, x.Y));
            tmp.push_back(Location(x.X+1, x.Y));
            tmp.push_back(Location(x.X, x.Y+1));
            tmp.push_back(Location(x.X, x.Y-1));
        }
        return tmp;
    }
    static vector<Location> AdjacentLocationsALL(Location x)
    {
        vector<Location> tmp;
        tmp.push_back(Location(x.X-1, x.Y));
        tmp.push_back(Location(x.X+1, x.Y));
        tmp.push_back(Location(x.X, x.Y+1));
        tmp.push_back(Location(x.X, x.Y-1));
        return tmp;
    }
    static vector<Location> RemoveDublicateFromVectorLocation(vector<Location> denemeben)
    {
        vector<Location> tmp;

        for (int i = 0; i < denemeben.size(); i++)
        {
            int a = 0;
            for (int y = i+1; y < denemeben.size(); y++)
            {
                if (denemeben.at(i).X == denemeben.at(y).X && denemeben.at(i).Y == denemeben.at(y).Y)
                {
                    a +=1;
                }
            }
            
            if (a == 0)
            {
                tmp.push_back(denemeben.at(i));
            }
            
        }
        
        return tmp;
    }
    static vector<Location> checkifValidLoc(vector<Location> x, int maxy)
    {
        vector<Location> temp;
        temp = RemoveDublicateFromVectorLocation(x);
        
        for (int i=0; i< temp.size(); i++)
        {
            if (x.at(i).X == 0 || x.at(i).Y == 0 || x.at(i).X > maxy ||x.at(i).Y > maxy)
            {
                temp.erase(temp.begin()+i);
            }
        }
        return temp;
    }
    static vector<Location> NotInVector(vector<Location> x, vector<Location> y)
    {
        vector<Location> tmp;

        for (int i = 0; i < x.size(); i++)
        {
            int a = 0;
            for (int j = 0; j < y.size(); j++)
            {
                if (x.at(i).X == y.at(j).X && x.at(i).Y == y.at(j).Y)
                {
                    a +=1;
                }
            }
            if (a == 0)
            {
                tmp.push_back(x.at(i));
            }
        }
        
        return RemoveDublicateFromVectorLocation(tmp);
    }
    
    static vector<Location> NotIn(vector<Location> x, Location y)
    {
        vector<Location> tmp;

        for (int i = 0; i < x.size(); i++)
        {
            int a = 0;
            if (x.at(i).X == y.X && x.at(i).Y == y.Y)
            {
                a +=1;
            }
            if (a == 0)
            {
                tmp.push_back(x.at(i));
            }
        }
        
        return RemoveDublicateFromVectorLocation(tmp);
    }
    static Location FindWumpus(vector<Location> stench, vector<Location> safe)
    {
        vector<Location> tmp;
        vector<Location> tmp2;
        vector<Location> tmp3;
        vector<Location> tmp4;
        Location wumpus = Location(0,0);
        
        tmp = RemoveDublicateFromVectorLocation(stench);
        tmp2 = RemoveDublicateFromVectorLocation(safe);
        
        /*for (int i = 0; i < tmp.size(); i++)//for every stench loc, it checks if any 3 out of 4 of the adjacent locs are in safe locs
        {
            tmp3.clear();
            tmp3 = AdjacentLocationsALL(tmp.at(i));
            int cnt = 0;
            
            for (int j = 0; j < tmp3.size(); j++)
            {
                if (InIt(tmp3.at(j), tmp2))
                {
                    cnt +=1;
                }
            }
            if (cnt == 3)
            {
                wumpus = NotInVector(tmp3, tmp2).at(0);
                return wumpus;
            }
            else
            {
                wumpus = Location(0,0);
            }
        }*/
        
        for (int i = 0; i< tmp.size(); i++)
        {//2-f If there are two stench locations in a diagonal arrangement, and one of their common adjacent locations is safe, then the other commonadjacent location contains the Wumpus.
                if (tmp.size() > 1)
                {
                    for( int j = 0; j< tmp.size(); j++){
                        if (tmp.at(i).X+1 == tmp.at(j).X && tmp.at(i).Y+1 == tmp.at(j).Y){
                            tmp4.push_back(Location(tmp.at(i).X, tmp.at(i).Y+1));
                            tmp4.push_back(Location(tmp.at(i).X+1, tmp.at(i).Y));
                            int cnt = 0;
                            
                            for (int k = 0; k < tmp4.size(); k++)
                            {
                                if (InIt(tmp4.at(k), tmp2))
                                {
                                    cnt +=1;
                                }
                            }
                            
                            if (cnt == 1)
                            {
                                wumpus = NotInVector(tmp4, tmp2).at(0);
                                return wumpus;
                            }
                            else
                            {
                                tmp4.clear();
                                wumpus = Location(0,0);
                            }
                        }else if(tmp.at(i).X+1 == tmp.at(j).X && tmp.at(i).Y-1 == tmp.at(j).Y){
                            tmp4.push_back(Location(tmp.at(i).X, tmp.at(i).Y-1));
                            tmp4.push_back(Location(tmp.at(i).X+1, tmp.at(i).Y));
                            int cnt = 0;
                            
                            for (int k = 0; k < tmp4.size(); k++)
                            {
                                if (InIt(tmp4.at(k), tmp2))
                                {
                                    cnt +=1;
                                }
                            }
                            
                            if (cnt == 1)
                            {
                                wumpus = NotInVector(tmp4, tmp2).at(0);
                                return wumpus;
                            }
                            else
                            {
                                tmp4.clear();
                                wumpus = Location(0,0);
                            }
                        }else if(tmp.at(i).X-1 == tmp.at(j).X && tmp.at(i).Y-1 == tmp.at(j).Y){
                            tmp4.push_back(Location(tmp.at(i).X, tmp.at(i).Y-1));
                            tmp4.push_back(Location(tmp.at(i).X-1, tmp.at(i).Y));
                            int cnt = 0;
                            
                            for (int k = 0; k < tmp4.size(); k++)
                            {
                                if (InIt(tmp4.at(k), tmp2))
                                {
                                    cnt +=1;
                                }
                            }
                            
                            if (cnt == 1)
                            {
                                wumpus = NotInVector(tmp4, tmp2).at(0);
                                return wumpus;
                            }
                            else
                            {
                                tmp4.clear();
                                wumpus = Location(0,0);
                            }
                        }else if(tmp.at(i).X-1 == tmp.at(j).X && tmp.at(i).Y+1 == tmp.at(j).Y){
                            tmp4.push_back(Location(tmp.at(i).X, tmp.at(i).Y+1));
                            tmp4.push_back(Location(tmp.at(i).X-1, tmp.at(i).Y));
                            int cnt = 0;
                            
                            for (int k = 0; k < tmp4.size(); k++)
                            {
                                if (InIt(tmp4.at(k), tmp2))
                                {
                                    cnt +=1;
                                }
                            }
                            
                            if (cnt == 1)
                            {
                                wumpus = NotInVector(tmp4, tmp2).at(0);
                                return wumpus;
                            }
                            else
                            {
                                tmp4.clear();
                                wumpus = Location(0,0);
                            }
                        }
                    }
                }
                
            }
        return wumpus;
    }
    static Location FindPit(vector<Location> breeze, vector<Location> safe)
    {
        vector<Location> tmp;
        vector<Location> tmp2;
        vector<Location> tmp4;
        Location pit = Location(0,0);
        tmp = RemoveDublicateFromVectorLocation(breeze);
        tmp2 = RemoveDublicateFromVectorLocation(safe);
        for (int i = 0; i< tmp.size(); i++)
        {//2-f If there are two breeze locations in a diagonal arrangement, and one of their common adjacent locations is safe, then the other commonadjacent location contains the pit.
                if (tmp.size() > 1)
                {
                    for( int j = 0; j< tmp.size(); j++){
                        if (tmp.at(i).X+1 == tmp.at(j).X && tmp.at(i).Y+1 == tmp.at(j).Y){
                            tmp4.push_back(Location(tmp.at(i).X, tmp.at(i).Y+1));
                            tmp4.push_back(Location(tmp.at(i).X+1, tmp.at(i).Y));
                            int cnt = 0;
                            
                            for (int k = 0; k < tmp4.size(); k++)
                            {
                                if (InIt(tmp4.at(k), tmp2))
                                {
                                    cnt +=1;
                                }
                            }
                            
                            if (cnt == 1)
                            {
                                pit = NotInVector(tmp4, tmp2).at(0);
                                return pit;
                            }
                            else
                            {
                                tmp4.clear();
                            }
                        }else if(tmp.at(i).X+1 == tmp.at(j).X && tmp.at(i).Y-1 == tmp.at(j).Y){
                            tmp4.push_back(Location(tmp.at(i).X, tmp.at(i).Y-1));
                            tmp4.push_back(Location(tmp.at(i).X+1, tmp.at(i).Y));
                            int cnt = 0;
                            
                            for (int k = 0; k < tmp4.size(); k++)
                            {
                                if (InIt(tmp4.at(k), tmp2))
                                {
                                    cnt +=1;
                                }
                            }
                            
                            if (cnt == 1)
                            {
                                pit = NotInVector(tmp4, tmp2).at(0);
                                return pit;
                            }
                            else
                            {
                                tmp4.clear();
                            }
                        }else if(tmp.at(i).X-1 == tmp.at(j).X && tmp.at(i).Y-1 == tmp.at(j).Y){
                            tmp4.push_back(Location(tmp.at(i).X, tmp.at(i).Y-1));
                            tmp4.push_back(Location(tmp.at(i).X-1, tmp.at(i).Y));
                            int cnt = 0;
                            
                            for (int k = 0; k < tmp4.size(); k++)
                            {
                                if (InIt(tmp4.at(k), tmp2))
                                {
                                    cnt +=1;
                                }
                            }
                            
                            if (cnt == 1)
                            {
                                pit = NotInVector(tmp4, tmp2).at(0);
                                return pit;
                            }
                            else
                            {
                                tmp4.clear();
                            }
                        }else if(tmp.at(i).X-1 == tmp.at(j).X && tmp.at(i).Y+1 == tmp.at(j).Y){
                            tmp4.push_back(Location(tmp.at(i).X, tmp.at(i).Y+1));
                            tmp4.push_back(Location(tmp.at(i).X-1, tmp.at(i).Y));
                            int cnt = 0;
                            
                            for (int k = 0; k < tmp4.size(); k++)
                            {
                                if (InIt(tmp4.at(k), tmp2))
                                {
                                    cnt +=1;
                                }
                            }
                            
                            if (cnt == 1)
                            {
                                pit = NotInVector(tmp4, tmp2).at(0);
                                return pit;
                            }
                            else
                            {
                                tmp4.clear();
                            }
                        }
                    }
                }
                
            }
        return pit;
    }
};

#endif // AGENT_H

