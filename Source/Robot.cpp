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

    vector<pair<Moves, Direction>>  robotThreadFunc(void * arg){
        cout << "hi again" << endl;
        fflush(stdout);
        RThread* RTinfo = (RThread*) arg;
        vector<pair<Moves, Direction>> robotsList = genRobotsCommandsList(RTinfo);
        return robotsList;
    }

    vector<pair<Moves, Direction>> genRobotsCommandsList(RThread* RTinfo){
        tuple <int, int, startPushAxis> startingPushPositionAxis;
        vector<pair<Moves, Direction>> behindBoxList = genCommGetBehindBox(RTinfo, startingPushPositionAxis);
        // vector<pair<Moves, Direction>> robtoMovesToSecondPushPosition = genMovetoSecondPushPosition();
        vector<pair<Moves, Direction>> pushFirstLegList = recordMovesPushToDoor(RTinfo, startingPushPositionAxis);
        // vector<pair<Moves, Direction>*> toDoorList = genCommPushBoxtoDoor(RTinfo);
        cout << "behindBoxList->size()" <<  behindBoxList.size() << endl;


        //probably an easier way to do this...
        for (uint i = 0; i < pushFirstLegList.size(); i++){
            cout << "first leg list ["<< i<< "] equals " << pushFirstLegList[i].first << " " 
            << pushFirstLegList[i].second << endl;

            behindBoxList.push_back(pushFirstLegList[i]);
        }

        cout << "behindBoxList->size()" <<  behindBoxList.size() << endl;

        cout << "Thread "<<RTinfo->index <<" has following Commands: " << endl;

        for (uint i=0; i < behindBoxList.size(); i++){
            string moveString = convertMoveEnumToWord( (behindBoxList)[i].first);    
            string directionString = convertDirEnumToWord((behindBoxList)[i].second);
            cout << "'i' = " << i << endl;
            cout << "\t" << moveString << "\t" << directionString << endl;
        }

        return behindBoxList;
    }

    vector<pair<Moves, Direction>> recordMovesPushToDoor(RThread* RTinfo, tuple <int, int, startPushAxis> startingPushPositionAxis){

        vector<pair<Moves, Direction>>* movesPushtoDoor = new vector<pair<Moves, Direction>>();

        int idx = RTinfo->index;
        // If pushing axis is horizontal
        pair<int,int> startingPoint = make_pair(get<0>(startingPushPositionAxis), get<1>(startingPushPositionAxis));
        pair<int, int> destination = make_pair(doorLoc[doorAssign[idx]]->first, doorLoc[doorAssign[idx]]->second);

        int distanceFromRobToDoorX = startingPoint.second - destination.second;
        int distanceFromRobToDoorY = startingPoint.first - destination.first;

        if ((get<2> (startingPushPositionAxis)) == HORIZONTAL){
            recordMovesX(*movesPushtoDoor, startingPoint, destination, PUSH);        
            pushToDoorAxis pushDoorAxis = ptdVERTICAL;
            movesPushtoDoor->pop_back();
            recordMovesToSecondPushPosition(*movesPushtoDoor, distanceFromRobToDoorY,distanceFromRobToDoorX, pushDoorAxis);
            recordMovesY(*movesPushtoDoor, startingPoint, destination, PUSH);
        }

        if ((get<2>(startingPushPositionAxis)) == VERTICAL){
            recordMovesY(*movesPushtoDoor, startingPoint, destination, PUSH);
            pushToDoorAxis pushDoorAxis = ptdHORIZONTAL;
            movesPushtoDoor->pop_back();
            recordMovesToSecondPushPosition(*movesPushtoDoor, distanceFromRobToDoorY, distanceFromRobToDoorX, pushDoorAxis);
            recordMovesX(*movesPushtoDoor, startingPoint, destination, PUSH);
        }   

        return *movesPushtoDoor;
    }

    vector<pair<Moves, Direction>> genCommGetBehindBox(RThread* RTinfo, tuple <int, int, startPushAxis>& startingPushPositionAxis){

        // determine the starting push position 
        startingPushPositionAxis = determineStartingPushPositionAxis(RTinfo);
        cout << "startingPushPositionAxis:" << endl;
        cout << "\t y coord = " << get<0> (startingPushPositionAxis) << endl;
        cout << "\t x coord = " << get<1> (startingPushPositionAxis) << endl;
        cout << "\t axis = " << get<2> (startingPushPositionAxis) << endl;
        return recordMovesToBehindBox(startingPushPositionAxis, RTinfo);
    }


   vector<pair<Moves, Direction>> recordMovesToBehindBox(tuple <int, int, startPushAxis> startingPushPositionAxis, RThread* RTinfo){

        int idx = RTinfo->index;

        // may have to change this later if this doesn't work
        vector<pair<Moves, Direction>>& ref = *(RTinfo->commandsListHolder[RTinfo->index]);

        // bool verticalShouldBeFirst = collisionWithBoxAvoider(startingPushPositionAxis, idx, goAround);

        recordMovesX(ref, make_pair(robotLoc[idx]->first, robotLoc[idx]->second),
        make_pair(get<0>(startingPushPositionAxis), get<1>(startingPushPositionAxis)), MOVE);

        recordMovesY(ref, make_pair(robotLoc[idx]->first, robotLoc[idx]->second),  
        make_pair(get<0>(startingPushPositionAxis), get<1>(startingPushPositionAxis)), MOVE);

        return ref;
    }


    void recordMovesToSecondPushPosition(vector<pair<Moves, Direction>>& RCList, int distanceFromRobToDoorY,
     int distanceFromRobToDoorX, startPushAxis pushToDoorAxis){

         cout << "\n\ndistanceFromRobToDoorX = \n" << distanceFromRobToDoorX << endl;
         cout << "distanceFromRobToDoorY = " << distanceFromRobToDoorY <<"\n\n"<< endl;
         

        pair<int, int> destination;

        if (pushToDoorAxis == ptdVERTICAL && distanceFromRobToDoorY > 0 && distanceFromRobToDoorX == 1){
            pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
            robComm->second = SOUTH;
            robComm->first = MOVE;
            RCList.push_back(*robComm);
            robComm = new pair<Moves, Direction>();
            robComm->second = WEST;
            robComm->first = MOVE;
            RCList.push_back(*robComm);
            return;    
        }
        if (pushToDoorAxis == ptdVERTICAL && distanceFromRobToDoorY > 0 && distanceFromRobToDoorX == -1){
            pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
            robComm->second = SOUTH;
            robComm->first = MOVE;
            RCList.push_back(*robComm);
            robComm = new pair<Moves, Direction>();
            robComm->second = EAST;
            robComm->first = MOVE;
            RCList.push_back(*robComm);
            return;    
        }

        
        if (pushToDoorAxis == ptdVERTICAL && distanceFromRobToDoorY > 0 && distanceFromRobToDoorX == 1){
            pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
            robComm->second = NORTH;
            robComm->first = MOVE;
            RCList.push_back(*robComm);
            robComm = new pair<Moves, Direction>();
            robComm->second = WEST;
            robComm->first = MOVE;
            RCList.push_back(*robComm);
            return;    
        }
        if (pushToDoorAxis == ptdVERTICAL && distanceFromRobToDoorY > 0 && distanceFromRobToDoorX == -1){
            pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
            robComm->second = NORTH;
            robComm->first = MOVE;
            RCList.push_back(*robComm);
            robComm = new pair<Moves, Direction>();
            robComm->second = EAST;
            robComm->first = MOVE;
            RCList.push_back(*robComm);
            return;    
        }


        if (pushToDoorAxis == ptdHORIZONTAL && distanceFromRobToDoorX > 0 && distanceFromRobToDoorY == 1){
            pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
            robComm->second = EAST;
            robComm->first = MOVE;
            RCList.push_back(*robComm);
            robComm = new pair<Moves, Direction>();
            robComm->second = SOUTH;
            robComm->first = MOVE;
            RCList.push_back(*robComm);
            return;    
        }
        if (pushToDoorAxis == ptdHORIZONTAL && distanceFromRobToDoorX > 0 && distanceFromRobToDoorY == -1){
            pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
            robComm->second = EAST;
            robComm->first = MOVE;
            RCList.push_back(*robComm);
            robComm = new pair<Moves, Direction>();
            robComm->second = NORTH;
            robComm->first = MOVE;
            RCList.push_back(*robComm);
            return;    
        }

        
        if (pushToDoorAxis == ptdHORIZONTAL && distanceFromRobToDoorX < 0 && distanceFromRobToDoorY == 1){
            pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
            robComm->second = WEST;
            robComm->first = MOVE;
            RCList.push_back(*robComm);
            robComm = new pair<Moves, Direction>();
            robComm->second = SOUTH;
            robComm->first = MOVE;
            RCList.push_back(*robComm);
            return;    
        }

        if (pushToDoorAxis == ptdHORIZONTAL && distanceFromRobToDoorX < 0 && distanceFromRobToDoorX == -1){
            pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
            robComm->second = WEST;
            robComm->first = MOVE;
            RCList.push_back(*robComm);
            robComm = new pair<Moves, Direction>();
            robComm->second = NORTH;
            robComm->first = MOVE;
            RCList.push_back(*robComm);
            return;    
        }

        if (distanceFromRobToDoorY == 0 || distanceFromRobToDoorX == 0) {
            cout << "error establishing second push position " << endl;
            // exit(99);
        }
        cout << "AN ERROR OCCURRED...." << endl;
        // exit(99);
    }

   tuple<int, int, startPushAxis> determineStartingPushPositionAxis(RThread * RTinfo){

       // assume we will push vertically first, then horizontally

       // first need to figure out if need to push vertically at all

       // variable where we store if this is situation where don't need to push at all in 
       // vertical axis default is that you do need to push vertically

        startPushAxis axis = VERTICAL;

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
            axis = HORIZONTAL;
        }

        int xDiffBoxDoor = boxLoc[idx]->second - doorLoc[doorAssign[idx]]->second;
        
        cout << "boxLoc[idx]->second:  \t"<< boxLoc[idx]->second << endl;
        cout << "doorLoc[idx]->second:  \t" << doorLoc[doorAssign[idx]]->second << endl;
        cout << "xDiffBoxDoor:  \t" << xDiffBoxDoor << endl;

        if (axis == HORIZONTAL){
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
        return make_tuple(startPushTargY, startPushTargX, axis);
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
            default:
                moveAsString ="";
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
            break;
            default:
                dirAsString ="";
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
            case NOMOVEMENT:
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
            case NOMOVEMENT:
                break;
        }
    }

    void destroyRobotsCommandsList(RThread* RTinfo){
        

    }



    void recordMovesX(vector<pair<Moves, Direction>>& RCList, pair<int, int> startingPoint, pair<int, int> destination, Moves argmove){

        int distanceFromRobToDestinationX = startingPoint.second - destination.second;
        if (distanceFromRobToDestinationX > 0){
            for (int i = 0; i < distanceFromRobToDestinationX; i++){
                    pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
                    robComm->second = WEST;
                    robComm->first = argmove;
                    RCList.push_back(*robComm);
            }
        }
        if (distanceFromRobToDestinationX < 0) {
            for (int i = 0; i > distanceFromRobToDestinationX; i--){
                    pair<Moves, Direction>* robComm= new pair<Moves, Direction>();
                    robComm->second = EAST;
                    robComm->first = argmove;
                RCList.push_back(*robComm);
            }
        }

        if (distanceFromRobToDestinationX == 0){
            cout << "no distance to travel X axis" << endl;
        }
    }


    void recordMovesY(vector<pair<Moves, Direction>>& RCList, pair<int, int> startingPoint, pair<int, int> destination, Moves argmove){

            // if this is NOT no_Y_Diff_Case... horizontal movement
        int distanceFromRobToDestinationY = startingPoint.first - destination.first;

        if (distanceFromRobToDestinationY > 0){
            for (int i = 0; i < distanceFromRobToDestinationY; i++){
                    pair<Moves, Direction>* robComm= new pair<Moves, Direction>();
                    robComm->second = NORTH;
                    robComm->first = argmove;
                    RCList.push_back(*robComm);
            }
        }

        if (distanceFromRobToDestinationY < 0){
            for (int i = 0; i > distanceFromRobToDestinationY; i--){
                    pair<Moves, Direction>* robComm= new pair<Moves, Direction>();
                    robComm->second = SOUTH;
                    robComm->first = argmove;
                    RCList.push_back(*robComm);
            }
        }

        if (distanceFromRobToDestinationY == 0){
            cout << "no distance to travel Y axis" << endl;
        }
    }
};
