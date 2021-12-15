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
#include <set>
#include <iostream>


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
        tuple <int, int, bool> startingPushPositionAxis;
        RobotCommandsList* behindBoxList = genCommGetBehindBox(RTinfo, startingPushPositionAxis);
        RobotCommandsList* pushFirstLegList = recordMovesFirstLeg(RTinfo, startingPushPositionAxis);
        // RobotCommandsList toDoorList = genCommPushBoxtoDoor(RTinfo);
        behindBoxList->insert(behindBoxList->end(), pushFirstLegList->begin(), pushFirstLegList->end());
        return behindBoxList;
    }

    RobotCommandsList* recordMovesFirstLeg(RThread* RTinfo, tuple <int, int ,bool> startingPushPositionAxis){

        RobotCommandsList* movesToDogLegPoint = new RobotCommandsList();

        int idx = RTinfo->index;
        // If pushing axis is horizontal
        pair<int,int> startingPoint = make_pair(get<0>(startingPushPositionAxis), get<1>(startingPushPositionAxis));
        pair<int, int> destination = make_pair(doorLoc[doorAssign[idx]]->first, doorLoc[doorAssign[idx]]->second);

        recordMovesX(movesToDogLegPoint, startingPoint, destination, PUSH);
        recordMovesY(movesToDogLegPoint, startingPoint, destination, PUSH);

        return movesToDogLegPoint;
    }

    RobotCommandsList* genCommGetBehindBox(RThread* RTinfo, tuple <int, int, bool>& startingPushPositionAxis){

        // determine the starting push position 
        startingPushPositionAxis = determineStartingPushPositionAxis(RTinfo);
        cout << "startingPushPositionAxis:" << endl;
        cout << "\t y coord = " << get<0> (startingPushPositionAxis) << endl;
        cout << "\t x coord = " << get<1> (startingPushPositionAxis) << endl;
        cout << "\t axis = " << get<2> (startingPushPositionAxis) << endl;
        return recordMovesToBehindBox(startingPushPositionAxis, RTinfo);
    }


   RobotCommandsList* recordMovesToBehindBox(tuple <int, int, bool> startingPushPositionAxis, RThread* RTinfo){

        int idx = RTinfo->index;

        RobotCommandsList* RCList = new RobotCommandsList();

        // bool verticalShouldBeFirst = collisionWithBoxAvoider(startingPushPositionAxis, idx, goAround);

        recordMovesX(RCList, make_pair(robotLoc[idx]->first, robotLoc[idx]->second),
        make_pair(get<0>(startingPushPositionAxis), get<1>(startingPushPositionAxis)), MOVE);

        recordMovesY(RCList, make_pair(robotLoc[idx]->first, robotLoc[idx]->second),  
        make_pair(get<0>(startingPushPositionAxis), get<1>(startingPushPositionAxis)), MOVE);

        return RCList;
    }


   tuple<int, int, bool> determineStartingPushPositionAxis(RThread * RTinfo){

       // assume we will push vertically first, then horizontally

       // first need to figure out if need to push vertically at all

       // variable where we store if this is situation where don't need to push at all in 
       // vertical axis default is that you do need to push vertically

        bool no_Y_Diff_Case = false;
        bool& pushingAxisIsHorizontal = no_Y_Diff_Case;

        // get current index of thread/robot
        int idx = RTinfo->index;

        cout << "idx = " << idx << endl;

        int startPushTargY, startPushTargX;

        // to figure out what side of the box we need to push, it needs to be whatever
        // is the opposite side from the door.  So figure out what side the door is on compared
        // with the Box.  In other words if it is a positive diff. Box - door or a negative difference.
        // If box - door is positive, the door is closer to the top row, and vice versa.
        int yDiffBoxDoor = boxLoc[idx]->first - doorLoc[doorAssign[idx]]->first;

        cout << "boxLoc[idx]->first:  \t"<< boxLoc[idx]->first << endl;
        cout << "doorLoc[doorAssign[idx]]->first:  \t" << doorLoc[doorAssign[idx]]->first << endl;
        cout << "yDiffBoxDoor:  \t" << yDiffBoxDoor << endl;

        //if door closer to top, we want to push the box from one greater row number than the box
        if (yDiffBoxDoor > 0){
            startPushTargY = boxLoc[idx]->first + 1;
            cout << "startPushTargY (>) = \t" << startPushTargY << endl;
        }
          // if door closer to top, we want to push the box from one lesser row number than the box
        if (yDiffBoxDoor < 0){
            startPushTargY = boxLoc[idx]->first -1;
            cout << "startPushTargY (<) = \t" << startPushTargY << endl;
        }
        // if door is the same, there is no distance to travel
        if (yDiffBoxDoor == 0){
            startPushTargY = boxLoc[idx]->first;
            no_Y_Diff_Case = true;
            cout << "no_Y_Diff_Case = " << no_Y_Diff_Case << endl;
        }

        int xDiffBoxDoor = boxLoc[idx]->second - doorLoc[doorAssign[idx]]->second;
        
        cout << "boxLoc[idx]->second:  \t"<< boxLoc[idx]->second << endl;
        cout << "doorLoc[idx]->second:  \t" << doorLoc[doorAssign[idx]]->second << endl;
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

        // record start values and return tuple
        return make_tuple(startPushTargY, startPushTargX, pushingAxisIsHorizontal);
    }


    void printRobotsCommandsList(RobotCommandsList* RCL){
        cout << "hello" << endl;
        cout << "Printing RCL List: \n"<< endl;
        cout << "\tMOVE:\tDirection:" << endl;
        for(auto it = RCL->begin(); it != RCL->end(); ++it){
            string moveString = convertMoveEnumToWord((*it)->move );
            string directionString = convertDirEnumToWord((*it)->direction);
            cout <<"\t"<< moveString <<"\t" << directionString << endl;
        }
    }

    string convertMoveEnumToWord(Moves move){
        string moveAsString;
        switch(move){
            case PUSH:
                moveAsString = "PUSH";
            break;
            case MOVE:
                moveAsString = "MOVE";
            break;
            case END:
                moveAsString = "END";
            break;
        }
        return moveAsString;
    }

    string convertDirEnumToWord(Direction dir){
        string dirAsString;
        switch(dir){
            case NORTH:
                dirAsString = "NORTH";
            break;
            case SOUTH:
                dirAsString = "SOUTH";
            break;
            case EAST:
                dirAsString = "EAST";
            break;
            case WEST:
                dirAsString = "WEST";
            break;
            case NOMOVEMENT:
                dirAsString = "NOMOVEMENT";
        }
        return dirAsString;
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



    void recordMovesX(RobotCommandsList* RCList, pair<int, int> startingPoint, pair<int, int> destination, Moves argmove){

        int distanceFromRobToDestinationX = startingPoint.second - destination.second;
        if (distanceFromRobToDestinationX > 0){
            for (int i = 0; i < distanceFromRobToDestinationX; i++){
                    RobotCommand* robComm= new RobotCommand();
                    robComm->direction = WEST;
                    robComm->move = argmove;
                RCList->push_front(robComm);
                // cout << "pushed back new Robot command " << endl;
                // cout << RCList.
            }
        }
        if (distanceFromRobToDestinationX < 0) {
            for (int i = 0; i > distanceFromRobToDestinationX; i--){
                    RobotCommand* robComm= new RobotCommand();
                    robComm->direction = EAST;
                    robComm->move = argmove;
                RCList->push_front(robComm);
            }
        }

        if (distanceFromRobToDestinationX == 0){
            cout << "no distance to travel X axis" << endl;
        }
    }


    void recordMovesY(RobotCommandsList* RCList, pair<int, int> startingPoint, pair<int, int> destination, Moves argmove){

                // if this is NOT no_Y_Diff_Case... horizontal movement
            int distanceFromRobToDestinationY = startingPoint.first - destination.first;

            if (distanceFromRobToDestinationY > 0){
                for (int i = 0; i < distanceFromRobToDestinationY; i++){
                     RobotCommand* robComm= new RobotCommand();
                     robComm->direction = NORTH;
                     robComm->move = argmove;
                     RCList->push_front(robComm);
                }
            }

            if (distanceFromRobToDestinationY < 0){
                for (int i = 0; i > distanceFromRobToDestinationY; i--){
                     RobotCommand* robComm= new RobotCommand();
                     robComm->direction = SOUTH;
                     robComm->move = argmove;
                     RCList->push_front(robComm);
                }
            }

            if (distanceFromRobToDestinationY == 0){
                cout << "no distance to travel Y axis" << endl;
            }
    }

};


//  set<pair<uint, uint>> genSetofCoordsToAvoid(int currIdx){
//     set<pair<uint, uint>> coordsToAvoid;
//     for(int i=0; i < boxLoc.size(); i++){
    
//         if (i = currIdx){
//             continue;
//         }
//             coordsToAvoid.insert(make_pair(boxLoc[i]->first, boxLoc[i]->second));
//         }

//         for(int i=0; i < doorLoc.size(); i++){
//             coordsToAvoid.insert(make_pair(boxLoc[i]->first, boxLoc[i]->second));
//         }
//  }

/*

The algo for this box pushing is actually pretty complicated... You will have to push the box twice if it does on start off in 

the same plane as the door.  So you will have to push it to a sub-target.  So you'll have to figure out the subtarget, get it there, 

and then switch angles.

*/

    // There's obviously some bugs with this function... Need to figure out why robots are going to wrong side sometimes.
    // bool collisionWithBoxAvoider(tuple <int, int, bool> startingPushPositionAxis, int idx, NeedToGoAround& goAround){{

    //     int robotRow = robotLoc[idx]->first;
    //     int boxRow = boxLoc[idx]->first;
    //     int destRow = get<0>(startingPushPositionAxis);

    //     int robotCol = robotLoc[idx]->first;
    //     int boxCol = boxLoc[idx]->first;
    //     int destCol = get<0>(startingPushPositionAxis);

    //     set<pair<uint, uint>> coordsToAvoid = genSetofCoordsToAvoid(idx);

    //     int distanceFromRobToDestinationX = robotLoc[idx]->second - get<1>(startingPushPositionAxis);
    //     int distanceFromRobToBoxX = robotLoc[idx]->second - boxLoc[idx]->second;

        
    //     // (1) if the destination is on the other side of the box from the robot on the X axis
    //     if(abs(distanceFromRobToDestinationX) > abs(distanceFromRobToBoxX)){
    //         // (2) if the robot's current path is not going to collide with coordinate to avoid

    //         if (robotLoc[idx]->first != boxLoc[idx]->first && ){
    //             // (1) and (2) are true so we do not necessarily need to do vertical first... 
    //             return false;
    //         }
    //         // 
    //             goAround = YES_X;
    //             return false;
    //         }
    //     }

    //     int distanceFromRobToDestinationY = robotLoc[idx]->first - get<0>(startingPushPositionAxis);
    //     int distanceFromRobToBoxY = robotLoc[idx]->first - boxLoc[idx]->first;

    //     if(abs(distanceFromRobToDestinationY) > abs(distanceFromRobToBoxY)){
    //         if (robotLoc[idx]->second != boxLoc[idx]->second){
    //             return true;
    //         }
    //         else {
    //             goAround = YES_Y;
    //             return true;
    //         }
    //     }
    //     cout << "target loc wasn't on other side for either axis" << endl;
    //     return true;
    // }

    // may have to generalize this later... we'll see
    // function returns a tuple that provides the starting push position and the axis down which
    // the robot needs to travel to get there
            