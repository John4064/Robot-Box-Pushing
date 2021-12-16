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
#include <fstream>


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

    vector<pair<Moves, Direction> >  robotThreadFunc(void * arg){
        cout << "hi again" << endl;
        fflush(stdout);
        RThread* RTinfo = (RThread*) arg;
        vector<pair<Moves, Direction> > robotsList = genRobotsCommandsList(RTinfo);
        return robotsList;
    }

    vector<pair<Moves, Direction> > genRobotsCommandsList(RThread* RTinfo){
        tuple <int, int, startPushAxis> startingPushPositionAxis;
        vector<pair<Moves, Direction> > behindBoxList = genCommGetBehindBox(RTinfo, startingPushPositionAxis);
        vector<pair<Moves, Direction> > pushFirstLegList = recordMovesPushToDoor(RTinfo, startingPushPositionAxis);
        cout << "behindBoxList->size()" <<  behindBoxList.size() << endl;


        //probably an easier way to do this...
        for (uint i = 0; i < pushFirstLegList.size(); i++){
            cout << "first leg list ["<< i<< "] equals " << pushFirstLegList[i].first << " " 
            << pushFirstLegList[i].second << endl;

            behindBoxList.push_back(pushFirstLegList[i]);
        }

        RTinfo->commandsListHolder.push_back(behindBoxList);

        cout << "behindBoxList->size()" <<  behindBoxList.size() << endl;

        cout << "Thread "<<RTinfo->index <<" has following Commands: " << endl;
        /*Old Code
        for (uint i=0; i < behindBoxList.size(); i++){
            string moveString = convertMoveEnumToWord( (behindBoxList)[i].first);    
            string directionString = convertDirEnumToWord((behindBoxList)[i].second);
            cout << "'i' = " << i << endl;
            cout << "\t" << moveString << "\t" << directionString << endl;
            fflush(stdout);
        }
        */ 
        std::ofstream output_file("./robotSimulOut.txt");
		for(uint i = 0; i < behindBoxList.size(); i++){
        		//We iterated through the command list length, and then the size of the data
                string moveString = convertMoveEnumToWord( (behindBoxList)[i].first);    
                string directionString = convertDirEnumToWord((behindBoxList)[i].second);
				output_file << "robot 1 ";
                output_file << moveString<< " " << directionString<< endl;
		}
	    output_file.close();

        return behindBoxList;
    }

    vector<pair<Moves, Direction>> recordMovesPushToDoor(RThread* RTinfo, tuple <int, int, startPushAxis> startingPushPositionAxis){

        vector<pair<Moves, Direction>>* movesPushtoDoor = new vector<pair<Moves, Direction>>();

        int idx = RTinfo->index;
        
        pair<uint,uint>* origStartingPoint = robotLoc[idx];
        pair<uint, uint>* destination = doorLoc[doorAssign[idx]];
        pair<int, int> pushStartingPoint = make_pair(get<0> (startingPushPositionAxis), get<1> (startingPushPositionAxis));

        int origDistanceFromRobToDoorX = origStartingPoint->second - destination->second;
        int origDistanceFromRobToDoorY = origStartingPoint->first - destination->first;

        if ((get<2> (startingPushPositionAxis)) == HORIZONTAL){
            recordMovesX(*movesPushtoDoor, pushStartingPoint, *destination, PUSH);        
            movesPushtoDoor->pop_back();
            // movesPushtoDoor->pop_back();
            recordMovesToSecondPushPosition(*movesPushtoDoor, startingPushPositionAxis, idx);
            recordMovesY(*movesPushtoDoor, pushStartingPoint, *destination, PUSH);
        }

        if ((get<2>(startingPushPositionAxis)) == VERTICAL){
            recordMovesY(*movesPushtoDoor, pushStartingPoint, *destination, PUSH);
            movesPushtoDoor->pop_back();
            // movesPushtoDoor->pop_back();
            recordMovesToSecondPushPosition(*movesPushtoDoor, startingPushPositionAxis, idx);
            recordMovesX(*movesPushtoDoor, pushStartingPoint, *destination, PUSH);
        }   

        return *movesPushtoDoor;
    }

    vector<pair<Moves, Direction>> genCommGetBehindBox(RThread* RTinfo, tuple <int, int, startPushAxis>& startingPushPositionAxis){
        cout << "hi" << endl;
        // determine the starting push position 
        startingPushPositionAxis = determineStartingPushPositionAxis(RTinfo);
        cout << "startingPushPositionAxis:" << endl;
        cout << "\t y coord = " << get<0> (startingPushPositionAxis) << endl;
        cout << "\t x coord = " << get<1> (startingPushPositionAxis) << endl;
        cout << "\t axis = " << get<2> (startingPushPositionAxis) << endl;
        cout << "what's up" << endl;
        return recordMovesToBehindBox(startingPushPositionAxis, RTinfo);
    }


   vector<pair<Moves, Direction>> recordMovesToBehindBox(tuple <int, int, startPushAxis> startingPushPositionAxis, RThread* RTinfo){
       cout << "hellowwwwwww" << endl;
        int idx = RTinfo->index;
        cout << "YESSSS" << endl;

        // may have to change this later if this doesn't work
        vector<pair<Moves, Direction>>* vec = new  vector<pair<Moves, Direction>>();

        cout << "NOOOO" << endl;
        // bool verticalShouldBeFirst = collisionWithBoxAvoider(startingPushPositionAxis, idx, goAround);

        cout << "MAAAADEITHEREEEE" << endl;
        recordMovesX(*vec, make_pair(robotLoc[idx]->first, robotLoc[idx]->second), make_pair(get<0>(startingPushPositionAxis), get<1>(startingPushPositionAxis)), MOVE);
        cout << "HHHHHHHEEEEEYYYY" << endl;
        recordMovesY(*vec, make_pair(robotLoc[idx]->first, robotLoc[idx]->second),  
        make_pair(get<0>(startingPushPositionAxis), get<1>(startingPushPositionAxis)), MOVE);

        return *vec;
    }


    void recordMovesToSecondPushPosition(vector<pair<Moves, Direction>>& RCList, tuple <int, int, startPushAxis> 
        startingPushPositionAxis, int idx){

        int distanceFromOrigBoxToDoorY = get<0>(startingPushPositionAxis) - doorLoc[doorAssign[idx]]->first;
        int distanceFromOrigBoxToDoorX = get<1>(startingPushPositionAxis) - doorLoc[doorAssign[idx]]->second;
        startPushAxis pushToDoorAxis = get<2>(startingPushPositionAxis);

         cout << "\n\ndistanceFromRobToDoorX = \n" << distanceFromOrigBoxToDoorX << endl;
         cout << "distanceFromRobToDoorY = " << distanceFromOrigBoxToDoorY <<"\n\n"<< endl;
         fflush(stdout);
         

        pair<int, int> destination;

        if (pushToDoorAxis == VERTICAL && distanceFromOrigBoxToDoorY > 0 && distanceFromOrigBoxToDoorX > 0){
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
        if (pushToDoorAxis == VERTICAL && distanceFromOrigBoxToDoorY > 0 && distanceFromOrigBoxToDoorX < 0){
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

        
        if (pushToDoorAxis == VERTICAL && distanceFromOrigBoxToDoorY < 0 && distanceFromOrigBoxToDoorX > 0){
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
        if (pushToDoorAxis == VERTICAL && distanceFromOrigBoxToDoorY < 0 && distanceFromOrigBoxToDoorX < 0){
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


        if (pushToDoorAxis == HORIZONTAL && distanceFromOrigBoxToDoorY > 0 && distanceFromOrigBoxToDoorX > 0){
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
        if (pushToDoorAxis == HORIZONTAL && distanceFromOrigBoxToDoorY > 0 && distanceFromOrigBoxToDoorX < 0){
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

        
        if (pushToDoorAxis == HORIZONTAL && distanceFromOrigBoxToDoorY < 0 && distanceFromOrigBoxToDoorX > 0){
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

        if (pushToDoorAxis == HORIZONTAL && distanceFromOrigBoxToDoorY < 0 && distanceFromOrigBoxToDoorY < 0){
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

        if (distanceFromOrigBoxToDoorY == 0 || distanceFromOrigBoxToDoorX == 0) {
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

        cout << "hiii";

        startPushAxis axis = VERTICAL;

        cout << "random word" << endl;
        // get current index of thread/robot
        int idx = RTinfo->index;

        cout << "idx = " << idx << endl;

        int startPushTargY, startPushTargX;

        // to figure out what side of the box we need to push, it needs to be whatever
        // is the opposite side from the door.  So figure out what side the door is on compared
        // with the Box.  In other words if it is a positive diff. Box - door or a negative difference.
        // If box - door is positive, the door is closer to the top row, and vice versa.

        cout << "seg fualt after this line" << endl;
        int yDiffBoxDoor = boxLoc[idx]->first - doorLoc[doorAssign[idx]]->first;

        cout << "hellow" << endl;

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
        cout << "MADEEEEE" <<endl;
        if (distanceFromRobToDestinationX > 0){
            for (int i = 0; i < distanceFromRobToDestinationX; i++){
                    pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
                    robComm->second = WEST;
                    robComm->first = argmove;
                    cout << "seg fault after this...." << endl;
                    RCList.push_back(*robComm);
                    cout << "did i make it here?" << endl;
            }
        }
        cout << "ITTTT" << endl;
        if (distanceFromRobToDestinationX < 0) {
            for (int i = 0; i > distanceFromRobToDestinationX; i--){
                    pair<Moves, Direction>* robComm= new pair<Moves, Direction>();
                    robComm->second = EAST;
                    robComm->first = argmove;
                    cout << "seg fault aft er here?" << endl;
                RCList.push_back(*robComm);
                cout << " i think so" << endl;
            }
        }
        cout << "HEEERREE" << endl;
        if (distanceFromRobToDestinationX == 0){
            cout << "no distance to travel X axis" << endl;
        }
        cout << "DUDEEEE" << endl;
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
