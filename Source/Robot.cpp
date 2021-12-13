#include "Robot.h"
#include <pthread.h>
#include <cstdlib>
#include <vector>
#include <utility>
#include <string>



extern uint** grid;
extern uint numRows;	//	height of the grid
extern uint numCols;	//	width
extern uint numBoxes;	//	also the number of robots
extern uint numDoors;	//	The number of doors.
extern uint& numRobots;

extern vector<pair<uint, uint>*> boxLoc;
extern vector<uint> doorAssign;
extern vector<pair<uint, uint>*> doorLoc;

typedef unsigned int uint;

namespace Robot{

    extern vector<pair<uint, uint>*> robotLoc;

    void* robotThreadFunc(void * arg){

        RThread* RTinfo = (RThread*) arg;
        genRobotsCommandsList()

    }

    void printRobotsCommandsList(RThread* RTInfo){

    }

    void genRobotsCommandsList(RThread* RTinfo){

        

    };

    void destroyRobotsCommandsList(RThread* RTinfo){

    }



};

