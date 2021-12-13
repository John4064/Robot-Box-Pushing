#include "Robot.h"
#include <pthread.h>
#include <cstdlib>
#include <vector>
#include <utility>
#include <string>


extern uint** grid;
extern uint numRows;	//	height of the grid
extern uint numCols;	//	width
// extern uint numBoxes;	//	also the number of robots
// extern uint numDoors;	//	The number of doors.
// extern uint& numRobots;

extern vector<pair<uint, uint>*> boxLoc;
extern vector<uint> doorAssign;
extern vector<pair<uint, uint>*> doorLoc;

typedef unsigned int uint;

namespace Robot{

    const int NORTHSOUTH = 0, EASTWEST = 1;

    extern vector<pair<uint, uint>*> robotLoc;

    void* robotThreadFunc(void * arg){

        RThread* RTinfo = (RThread*) arg;
        RobotCommandsList robotsList = genRobotsCommandsList(RTinfo);

        return (new void*);
    }

    RobotCommandsList genRobotsCommandsList(RThread* RTinfo){
        RobotCommandsList behindBoxList = genCommGetBehindBox(RTinfo);
        RobotCommandsList toDoorList = genCommPushBoxtoDoor(RTinfo);
        behindBoxList.insert(behindBoxList.end(), toDoorList.begin(), toDoorList.end());
        return behindBoxList;
    }

    RobotCommandsList genCommGetBehindBox(RThread* RTinfo){

        int axis = NORTHSOUTH; // vertical axis is 0

        int boxVertDiff = boxLoc[RTinfo->index]->first - robotLoc[RTinfo->index]->first;
        int doorVertDiff =  doorLoc[RTinfo->index]->first - robotLoc[RTinfo->index]->first;   

        pair<int, Direction> numMovesDirectionRequiredVert = movesDirectionFigureOuter(boxVertDiff, doorVertDiff, 0);

        axis = EASTWEST; // horizontal axis is 1

        int boxHorizDiff = boxLoc[RTinfo->index]->second - robotLoc[RTinfo->index]->second;
        int doorHorizDiff = doorLoc[RTinfo->index]->second - robotLoc[RTinfo->index]->second;

        pair<int, Direction> numMovesDirectionRequiredHoriz = movesDirectionFigureOuter(boxHorizDiff, doorHorizDiff, 1);

    }


    /* We'll say we'll always go to the opposite side of the box on the vertical axis no matter what,
        AND we'll always get level to the box on the horizontal axis no matter what */

    pair<int, Direction> movesDirectionFigureOuter(int boxDiff, int doorDiff, int axis){

        int magnitude = -999;

        // would never happen...
        if (boxDiff == 0 && doorDiff == 0){
            return(make_pair (0,  Direction(NOMOVEMENT)));
        }

        // what if you run into another door though?
        if (boxDiff == 0 && doorDiff > 0){
            magnitude = 1;
        }

        if (boxDiff == 0 && doorDiff < 0){
            magnitude = -1;
        }

        if (boxDiff > 0 && doorDiff == 0){
            magnitude = ;
        }




        if(axis == NORTHSOUTH){
            if (magnitude == 1){
                return make_pair(1, Direction );
            }
            if (magnitude == -1){
                return make_pair(1, Direction );
            }
        }
        if(axis == EASTWEST){
            if (magnitude == 1){
                return make_pair(1, Direction );
            }
            if (magnitude == -1){
                return make_pair(1, Direction );
            }
        }

    }


    RobotCommandList genCommPushBoxtoDoor(RThread* RTinfo){
        
    }

    void printRobotsCommandsList(RThread* RTinfo){



    }

    void destroyRobotsCommandsList(RThread* RTinfo){

    }



};

