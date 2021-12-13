#include "Robot.h"
#include <pthread.h>
#include <cstdlib>
#include <vector>
#include <utility>
#include <string>
#include <iostream>
#include <tuple>
#include <functional>


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

        tuple <int, int, bool> targetStartingPushPositionAxis = determineStartingPushPositionAxis(RTinfo);
        recordMovesToBehindBox(targetStartingPushPositionAxis);

    }

    void recordMovesToBehindBox(tuple <int, int, bool> targetStartingPushPositionAxis){
        
        // could add the distance to destination to tuple if wanted to
        if (get<2>(targetStartingPushPositionAxis)){


        }

        int distanceToDestination = 

        for(int i = 0; i < )

    }


    // may have to generalize this later... we'll see
   tuple<int, int, bool> determineStartingPushPositionAxis(RThread * RTinfo){

        bool no_Y_Diff_Case = false;

        int idx = RTinfo->index;

        int startPushTargY, startPushTargX;

        int yDiffBoxDoor = boxLoc[idx]->first - doorLoc[idx]->first;

        if (yDiffBoxDoor > 0){
            startPushTargY = boxLoc[idx]->first + 1;
        }
        if (yDiffBoxDoor < 0){
            startPushTargY = boxLoc[idx]->first -1;
        }
        if (yDiffBoxDoor == 0){
            no_Y_Diff_Case = true;
        }

        int xDiffBoxDoor = boxLoc[idx]->second - doorLoc[idx]->second;

        if (no_Y_Diff_Case){
            if (xDiffBoxDoor > 0){
                startPushTargX = boxLoc[idx]->second + 1;
            }
            else if (xDiffBoxDoor < 0){
                startPushTargX = boxLoc[idx]->second - 1;
            }
            else {
                cerr << "Error, the robot is on top of its target box" << endl;
            }
        }
        else {
            startPushTargX = boxLoc[idx]->second;
        }
        return make_tuple(startPushTargY, startPushTargX, no_Y_Diff_Case);
    }

    void pushToDoorX(){

    }


           //Diff's are relative to the robot
            // int boxVertDiff = boxLoc[RTinfo->index]->first - robotLoc[RTinfo->index]->first;
            // int doorVertDiff =  doorLoc[RTinfo->index]->first - robotLoc[RTinfo->index]->first;   
            // int boxHorizDiff = boxLoc[RTinfo->index]->second - robotLoc[RTinfo->index]->second;
            // int doorHorizDiff = doorLoc[RTinfo->index]->second - robotLoc[RTinfo->index]->second;
    // /* We'll say we'll always go to the opposite side of the box on the vertical axis no matter what,
    //     AND we'll always get level to the box on the horizontal axis no matter what
    //     so we'll get it lined up first, then if its in the vertical axis we'll add one to the magnitude */

    // pair<int, Direction> movesDirectionFigureOuter(int boxDiff, int doorDiff, int axis){

    //     int distanceRequiredThisAxis = -999;

    //     if (boxDiff == 0 && doorDiff == 0){
    //         return(make_pair (0,  Direction(NOMOVEMENT)));
    //     }

    //     // what if you run into another door though?
    //     if (boxDiff == 0 && doorDiff > 0){
    //         distanceRequiredThisAxis = 0;
    //     }

    //     if (boxDiff == 0 && doorDiff < 0){
    //         distanceRequiredThisAxis = 0;
    //     }

    //     if (doorDiff == 0){
    //         distanceRequiredThisAxis = -boxDiff;
    //     }





    // }


    RobotCommandList genCommPushBoxtoDoor(RThread* RTinfo){
        
    }

    void printRobotsCommandsList(RThread* RTinfo){



    }

    void destroyRobotsCommandsList(RThread* RTinfo){

    }



};

