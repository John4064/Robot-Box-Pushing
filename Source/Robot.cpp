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

    extern vector<pair<uint, uint>*> robotLoc;

    void* robotThreadFunc(void * arg){

        RThread* RTinfo = (RThread*) arg;
        RobotCommandsList robotsList = genRobotsCommandsList(RThread* RTinfo);


    }


    void genRobotsCommandsList(RThread* RTinfo){
        RobotCommandsList behindBoxList = genCommGetBehindBox(RThread* RTinfo);
        RobotCommandsList toDoorList = genCommPushBoxtoDoor(RThread* RTinfo);
        behindBoxList.insert(behindBoxList.end(), toDoorList.begin(), toDoorList.end());
    };

    genCommGetBehindBox(RThread* RTinfo){

        int boxVertDiff = robotLoc[RTinfo->index]->first - boxLoc[RTinfo->index]->first;
        int doorVertDiff = robotLoc[RTinfo->index]->first - doorLoc[RTinfo->index]->first;   

        pair<int, Direction> numMovesDirectionRequiredVert = movesDirectionFigureOuter(boxVertDiff, doorVertDiff);


        int boxHorizDiff = robotLoc[RTinfo->index]->second - boxLoc[RTinfo->index]->second;
        int doorHorizDiff = robotLoc[RTinfo->index]->second - doorLoc[RTinfo->index]->second;

        pair<int, Direction> numMovesDirectionRequiredVert = movesDirectionFigureOuter(boxHorizDiff, doorHorizDiff);
    
    }

    genCommPushBoxtoDoor(RThread* RTinfo){
        
    }

    void printRobotsCommandsList(RThread* RTinfo){



    }

    void destroyRobotsCommandsList(RThread* RTinfo){

    }



};

