#include "RThread.h"
#include <pthread.h>
#include <cstdlib>
#include <vector>
#include <utility>
#include <string>



extern uint** grid;
extern uint numRows = -1;	//	height of the grid
extern uint numCols = -1;	//	width
extern uint numBoxes = -1;	//	also the number of robots
extern uint numDoors = -1;	//	The number of doors.
extern uint& numRobots = numBoxes;
extern vector<pair<uint, uint>*> robotLoc;
extern vector<pair<uint, uint>*> boxLoc;
extern vector<uint> doorAssign;
extern vector<pair<uint, uint>*> doorLoc;

typedef unsigned int uint;

namespace Robot{

    void printRobotsCommandsList(RThread* RTInfo){

    }

    void generateRobotsCommandsList(RThread* RTinfo){








        

        


    }

    void freeRobotsCommandsList(RThread& RTinfo){

    }

};

