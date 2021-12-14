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
        RobotCommandsList *robotsList = genRobotsCommandsList(RTinfo);
        printRobotsCommandsList(robotsList);
        
        return (new void*);
    }

    RobotCommandsList* genRobotsCommandsList(RThread* RTinfo){
        RobotCommandsList* behindBoxList = genCommGetBehindBox(RTinfo);
        // RobotCommandsList toDoorList = genCommPushBoxtoDoor(RTinfo);
        // behindBoxList.insert(behindBoxList.end(), toDoorList.begin(), toDoorList.end());
        return behindBoxList;
    }

    RobotCommandsList* genCommGetBehindBox(RThread* RTinfo){

        tuple <int, int, bool> targetStartingPushPositionAxis = determineStartingPushPositionAxis(RTinfo);
        cout << "targetStartingPushPositionAxis:" << endl;
        cout << "\t y coord = " << get<0> (targetStartingPushPositionAxis) << endl;
        cout << "\t x coord = " << get<1> (targetStartingPushPositionAxis) << endl;
        cout << "\t axis = " << get<2> (targetStartingPushPositionAxis) << endl;
        return recordMovesToBehindBox(targetStartingPushPositionAxis, RTinfo);
    }



    RobotCommandsList* recordMovesToBehindBox(tuple <int, int, bool> targetStartingPushPositionAxis, RThread* RTinfo){

        int idx = RTinfo->index;

        cout << "\n\trobotLoc[idx]->first = " << robotLoc[idx]->first << "\n\trobotLoc[idx]->second = " <<robotLoc[idx]->second <<  endl;

        RobotCommandsList* RCList = new RobotCommandsList();
        
        // could add the distance to destination to tuple if wanted to

        // if this is no_Y_Diff_Case... horizontal movement
        if (get<2>(targetStartingPushPositionAxis)){
            
            int distanceFromRobToDestinationX = robotLoc[idx]->second - get<1>(targetStartingPushPositionAxis);
            if (distanceFromRobToDestinationX > 0){
                for (int i = 0; i < distanceFromRobToDestinationX; i++){
                     RobotCommand* robComm= new RobotCommand();
                     robComm->direction = WEST;
                     robComm->move = MOVE;
                    RCList->push_back(robComm);
                    // cout << "pushed back new Robot command " << endl;
                    // cout << RCList.
                }
            }
            if (distanceFromRobToDestinationX < 0) {
                for (int i = 0; i > distanceFromRobToDestinationX; i--){
                     RobotCommand* robComm= new RobotCommand();
                     robComm->direction = EAST;
                     robComm->move = MOVE;
                    RCList->push_back(robComm);
                }
            }
            if (distanceFromRobToDestinationX == 0){
                cout << "error, robot on top of desintation (recmoves func)" << endl;
            }
        }                // if this is NOT no_Y_Diff_Case... horizontal movement
        if (!get<2>(targetStartingPushPositionAxis)){
            int distanceFromRobToDestinationY = robotLoc[idx]->first - get<0>(targetStartingPushPositionAxis);
            if (distanceFromRobToDestinationY > 0){
                for (int i = 0; i < distanceFromRobToDestinationY; i++){
                     RobotCommand* robComm= new RobotCommand();
                     robComm->direction = NORTH;
                     robComm->move = MOVE;
                     RCList->push_back(robComm);
                }
            }
            if (distanceFromRobToDestinationY < 0){
                for (int i = 0; i > distanceFromRobToDestinationY; i--){
                     RobotCommand* robComm= new RobotCommand();
                     robComm->direction = SOUTH;
                     robComm->move = MOVE;
                     RCList->push_back(robComm);
                }
            }
            if (distanceFromRobToDestinationY == 0){
                cout << "error, robot on top of desintation (recmoves func)" << endl;
            }
        }
        return RCList;
    }

    // may have to generalize this later... we'll see
   tuple<int, int, bool> determineStartingPushPositionAxis(RThread * RTinfo){

        bool no_Y_Diff_Case = false;
        bool& pushingAxisIsHorizontal = no_Y_Diff_Case;

        int idx = RTinfo->index;

        cout << "idx = " << idx << endl;

        int startPushTargY, startPushTargX;

        int yDiffBoxDoor = boxLoc[idx]->first - doorLoc[idx]->first;

        cout << "boxLoc[idx]->first:  \t"<< boxLoc[idx]->first << endl;
        cout << "doorLoc[idx]->first:  \t" << doorLoc[idx]->first << endl;
        cout << "yDiffBoxDoor:  \t" << yDiffBoxDoor << endl;

        if (yDiffBoxDoor > 0){
            startPushTargY = boxLoc[idx]->first + 1;
            cout << "startPushTargY (>) = \t" << startPushTargY << endl;
        }
        if (yDiffBoxDoor < 0){
            startPushTargY = boxLoc[idx]->first -1;
            cout << "startPushTargY (<) = \t" << startPushTargY << endl;
        }
        if (yDiffBoxDoor == 0){
            no_Y_Diff_Case = true;
            cout << "no_Y_Diff_Case = " << no_Y_Diff_Case << endl;
        }

        int xDiffBoxDoor = boxLoc[idx]->second - doorLoc[idx]->second;
        
        cout << "boxLoc[idx]->second:  \t"<< boxLoc[idx]->second << endl;
        cout << "doorLoc[idx]->second:  \t" << doorLoc[idx]->second << endl;
        cout << "xDiffBoxDoor:  \t" << xDiffBoxDoor << endl;

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

        cout << "final values: \n";
        cout << "startPushTargY  \t" << startPushTargY << endl;
        cout << "startPushTargX \t" << startPushTargX << endl;

        return make_tuple(startPushTargY, startPushTargX, pushingAxisIsHorizontal);
    }

    void pushToDoorX(){

    }

    void printRobotsCommandsList(RobotCommandsList* RCL){
        cout << "hello" << endl;
        cout << "Printing RCL List: \n"<< endl;
        cout << "\tMOVE:\tDirection:" << endl;
        for(uint i = 0; i < RCL->size(); i++){
            cout << "item "<< i+1 <<" "<< (*RCL)[i]->move <<"\t" << (*RCL)[i]->direction << endl;
        }
    }

    void destroyRobotsCommandsList(RThread* RTinfo){

    }

};

