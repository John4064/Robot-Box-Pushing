#include "Robot.h"
#include <pthread.h>
#include <cstdlib>
#include <vector>
#include <utility>
#include <string>
#include <iostream>
#include <tuple>
#include <functional>
#include <cmath> 


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
        RobotCLs.push_back(robotsList);
        cout << "made it here" << endl;
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

        RobotCommandsList* RCList = new RobotCommandsList();

        bool verticalShouldBeFirst = collisionWithBoxAvoider(targetStartingPushPositionAxis, idx);

        if(verticalShouldBeFirst){
            recordMovesY(RCList, targetStartingPushPositionAxis, idx);
            recordMovesX(RCList, targetStartingPushPositionAxis, idx);
        }
        else {
            recordMovesX(RCList, targetStartingPushPositionAxis, idx);
            recordMovesY(RCList, targetStartingPushPositionAxis, idx);
        }      
        return RCList;
    }

    bool collisionWithBoxAvoider(tuple <int, int, bool> targetStartingPushPositionAxis, int idx){

        int distanceFromRobToDestinationX = robotLoc[idx]->second - get<1>(targetStartingPushPositionAxis);
        int distanceFromRobToBoxX = robotLoc[idx]->second - boxLoc[idx]->second;

        if(abs(distanceFromRobToDestinationX) > abs(distanceFromRobToBoxX)){
            if (robotLoc[idx]->first != boxLoc[idx]->first){
                return false;
            }
        }

        int distanceFromRobToDestinationY = robotLoc[idx]->first - get<0>(targetStartingPushPositionAxis);
        int distanceFromRobToBoxY = robotLoc[idx]->first - boxLoc[idx]->first;

        if(abs(distanceFromRobToDestinationY) > abs(distanceFromRobToBoxY)){
            if (robotLoc[idx]->second != boxLoc[idx]->second){
                return true;
            }
        }
        cout << "target loc wasn't on other side for either axis" << endl;
        return true;
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
        for(auto it = RCL->begin(); it != RCL->end(); ++it){
            cout << (*it)->move <<"\t" << (*it)->direction << endl;
            cout << "hello" << endl;
        }
    }

    void makeRegMove(Direction dir, int idx){

       switch (dir){
            case NORTH:
                robotLoc[idx]->first--;
                break;
            case SOUTH:
                robotLoc[idx]->first++;
                break;
            case EAST:
                robotLoc[idx]->second++;
                break;
            case WEST:
                robotLoc[idx]->second--;
                break;
        }
    }
    void makePushMove(Direction dir, int idx){
       switch (dir){
            case NORTH:
                robotLoc[idx]->first--;
                boxLoc[idx]->first--;
                break;
            case SOUTH:
                robotLoc[idx]->first++;
                boxLoc[idx]->first++;
                break;
            case EAST:
                robotLoc[idx]->second++;
                boxLoc[idx]->second++;
                break;
            case WEST:
                robotLoc[idx]->second--;
                boxLoc[idx]->second--;
                break;
        }
    }

    void destroyRobotsCommandsList(RThread* RTinfo){
        

    }

    void recordMovesX(RobotCommandsList* RCList, tuple <int, int, bool> targetStartingPushPositionAxis, int idx){

            int distanceFromRobToDestinationX = robotLoc[idx]->second - get<1>(targetStartingPushPositionAxis);
            if (distanceFromRobToDestinationX > 0){
                for (int i = 0; i < distanceFromRobToDestinationX; i++){
                     RobotCommand* robComm= new RobotCommand();
                     robComm->direction = WEST;
                     robComm->move = MOVE;
                    RCList->push_front(robComm);
                    // cout << "pushed back new Robot command " << endl;
                    // cout << RCList.
                }
            }
            if (distanceFromRobToDestinationX < 0) {
                for (int i = 0; i > distanceFromRobToDestinationX; i--){
                     RobotCommand* robComm= new RobotCommand();
                     robComm->direction = EAST;
                     robComm->move = MOVE;
                    RCList->push_front(robComm);
                }
            }


            if (distanceFromRobToDestinationX == 0){
                cout << "error, robot on top of desintation (recmoves func)" << endl;
            }

    }


    void recordMovesY(RobotCommandsList* RCList, tuple <int, int, bool> targetStartingPushPositionAxis, int idx){

         // if this is NOT no_Y_Diff_Case... horizontal movement
            int distanceFromRobToDestinationY = robotLoc[idx]->first - get<0>(targetStartingPushPositionAxis);
            if (distanceFromRobToDestinationY > 0){
                for (int i = 0; i < distanceFromRobToDestinationY; i++){
                     RobotCommand* robComm= new RobotCommand();
                     robComm->direction = NORTH;
                     robComm->move = MOVE;
                     RCList->push_front(robComm);
                }
            }
            if (distanceFromRobToDestinationY < 0){
                for (int i = 0; i > distanceFromRobToDestinationY; i--){
                     RobotCommand* robComm= new RobotCommand();
                     robComm->direction = SOUTH;
                     robComm->move = MOVE;
                     RCList->push_front(robComm);
                }
            }
            if (distanceFromRobToDestinationY == 0){
                cout << "error, robot on top of desintation (recmoves func)" << endl;
            }
    }

};
            