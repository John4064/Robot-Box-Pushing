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
#include <unistd.h>

extern int robotSleepTime;
extern uint** grid;
extern uint numRows;	//	height of the grid
extern uint numCols;
extern uint numBoxes;
extern uint numDoors;
	//	width
// extern uint numBoxes;	//	also the number of robots
// extern uint numDoors;	//	The number of doors.
// extern uint& numRobots;

extern vector<pair<uint, uint>*> boxLoc;
extern vector<uint> doorAssign;
extern vector<pair<uint, uint>*> doorLoc;
extern bool* GUIStartedP;

typedef unsigned int uint;

namespace Robot{

    const int NORTHSOUTH = 0, EASTWEST = 1;

    extern vector<pair<uint, uint>*> robotLoc;

    void* robotThreadFunc(void * arg){

        cout << "inside thread function \n\n\n\n\n\n\n\n" <<endl;
        fflush(stdout);
        RThread* RTinfo = (RThread*) arg;
        vector<pair<Moves, Direction>> threadsCommandList;
        RTinfo->genRobotsCommandsList(RTinfo);
        while(GUIStartedP[0] != 1){};
        RTinfo->robotMakeMoves();
        RTinfo->printARobotsCommandList();
        return (new void*);
    }

    void RThread::printARobotsCommandList(){

        ofstream myfile;
        myfile.open("robotSimulOut.txt");

        for(int i=0; i < thisRobotsMoves.size(); i++){
            myfile << "rows: "<< numRows <<"; cols: "<<
            numCols <<", "<< numBoxes <<", " << numDoors << endl;
            myfile << "\n";
            for(int i=0; i < doorLoc.size(); i++){
                myfile << "Door " << i << "; row: " << doorLoc[i]->first 
                << "col: "<< doorLoc[i]->second << endl;
            }

           for(int i=0; i < boxLoc.size(); i++){
                myfile << "Box " << i << "; row: " << boxLoc[i]->first 
                << "col: "<< boxLoc[i]->second << endl;
            }

            for(int i=0; i < robotLoc.size(); i++){
                myfile << "Robot " << i << "; row: " << robotLoc[i]->first 
                << "col: "<< robotLoc[i]->second << endl;
            }
            myfile << "\n"

            for (int i = 0; i < )
                myfile << "Robot Program: Robot" << << "\n";


        }
      
        myfile.close();
    }

    void RThread::robotMakeMoves(){
        while(!(thisRobotsMoves.empty())){
            cout << "trying to make a move!!!!!" << endl;
            cout << "the size of the robot moves vector is " << thisRobotsMoves.size() << endl;
            pair<Moves,Direction> temp = thisRobotsMoves.front();
            cout << "The move we are trying to make is "<< temp.first << " " << temp.second << endl;
            pair<Moves, Direction> command = thisRobotsMoves.front();
            cout << "made it here .... yay2" << endl;
            if (command.first == MOVE){
                cout << "a" << endl;
                pthread_mutex_lock(&RThread::mutex);
                makeRegMove(command.second, idx_of_robot);
                pthread_mutex_unlock(&RThread::mutex);
                cout << "b" << endl;
            }
            if (command.first == PUSH){
                cout << "c" << endl;
                pthread_mutex_lock(&RThread::mutex);
                makePushMove(command.second, idx_of_robot);
                pthread_mutex_unlock(&RThread::mutex);
                cout << "d" << endl;
            }
            if (command.first == END){
                cout << "e" << endl;
                pthread_mutex_lock(&RThread::mutex);
                thisRobotsMoves.clear();
                pthread_mutex_unlock(&RThread::mutex);
                cout << "d" << endl;
            }
                cout << "e" << endl;
            pthread_mutex_lock(&RThread::mutex);
            if(!thisRobotsMoves.empty()){
                thisRobotsMoves.erase(thisRobotsMoves.begin());
            }
            pthread_mutex_unlock(&RThread::mutex);
            cout << "f" << endl;
            cout << "made it here .... yay7" << endl;
            fflush(stdout);
            usleep(robotSleepTime);
        }
        cout << "finished with the VECTOR in the THread" << idx_of_robot << endl;
    }

 void RThread::genRobotsCommandsList(RThread* RTinfo){

    tuple <int, int, startPushAxis> startingPushPositionAxis;

    thisRobotsMoves = genCommGetBehindBox(RTinfo, startingPushPositionAxis);

    vector<pair<Moves, Direction>> movesToDoor = recordMovesPushToDoor(RTinfo, startingPushPositionAxis);
    cout << "thisRobotsMoves->size()" <<  thisRobotsMoves.size() << endl;

    //probably an easier way to do this...
    for (uint i = 0; i < movesToDoor.size(); i++){
        cout << "first leg list ["<< i<< "] equals " << movesToDoor[i].first << " " 
        << movesToDoor[i].second << endl;
        thisRobotsMoves.push_back(movesToDoor[i]);
    }


    pair<Moves, Direction>* lastCommand = new pair<Moves, Direction>();
    lastCommand->first = END;
    lastCommand->second = NOMOVEMENT;

    thisRobotsMoves.push_back(*lastCommand);

    RTinfo->commandsListHolder.push_back(thisRobotsMoves);

    cout << "thisRobotsMoves->size()" <<  thisRobotsMoves.size() << endl;

    cout << "Thread "<<RTinfo->idx_of_robot <<" has following Commands: " << endl;

    for (uint i=0; i < thisRobotsMoves.size(); i++){
        string moveString = convertMoveEnumToWord( (thisRobotsMoves)[i].first);    
        string directionString = convertDirEnumToWord((thisRobotsMoves)[i].second);
        cout << "'i' = " << i << endl;
        cout << "\t" << moveString << "\t" << directionString << endl;
        fflush(stdout);
    }
}

    vector<pair<Moves, Direction>> recordMovesPushToDoor(RThread* RTinfo, tuple <int, int, startPushAxis> startingPushPositionAxis){

        bool secondPushLeg = false;

        vector<pair<Moves, Direction>>* movesPushtoDoor = new vector<pair<Moves, Direction>>();

        int idx = RTinfo->idx_of_robot;
        
        pair<uint,uint>* origStartingPoint = robotLoc[idx];
        pair<uint, uint>* destination = doorLoc[doorAssign[idx]];
        pair<int, int> pushStartingPoint = make_pair(get<0> (startingPushPositionAxis), get<1> (startingPushPositionAxis));

        int origDistanceFromRobToDoorX = origStartingPoint->second - destination->second;
        int origDistanceFromRobToDoorY = origStartingPoint->first - destination->first;

        if ((get<2> (startingPushPositionAxis)) == HORIZONTAL){
            secondPushLeg = false;
            recordMovesX(*movesPushtoDoor, pushStartingPoint, *destination, PUSH, secondPushLeg);        
            if (!movesPushtoDoor->empty()){
                movesPushtoDoor->pop_back();
            }
            // movesPushtoDoor->pop_back();
            recordMovesToSecondPushPosition(*movesPushtoDoor, startingPushPositionAxis, idx);
            secondPushLeg = true;
            recordMovesY(*movesPushtoDoor, pushStartingPoint, *destination, PUSH, secondPushLeg);
        }

        if ((get<2>(startingPushPositionAxis)) == VERTICAL){
            secondPushLeg = false;
            recordMovesY(*movesPushtoDoor, pushStartingPoint, *destination, PUSH, secondPushLeg);
            if (!movesPushtoDoor->empty()){
                movesPushtoDoor->pop_back();
            }
            // movesPushtoDoor->pop_back();
            recordMovesToSecondPushPosition(*movesPushtoDoor, startingPushPositionAxis, idx);
            secondPushLeg = true;
            recordMovesX(*movesPushtoDoor, pushStartingPoint, *destination, PUSH, secondPushLeg);
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
        int idx = RTinfo->idx_of_robot;
        cout << "YESSSS" << endl;

        // may have to change this later if this doesn't work
        vector<pair<Moves, Direction>>* vec = new  vector<pair<Moves, Direction>>();

        cout << "NOOOO" << endl;
        // bool verticalShouldBeFirst = collisionWithBoxAvoider(startingPushPositionAxis, idx, goAround);

        cout << "MAAAADEITHEREEEE" << endl;
        recordMovesX(*vec, make_pair(robotLoc[idx]->first, robotLoc[idx]->second), make_pair(get<0>(startingPushPositionAxis), get<1>(startingPushPositionAxis)), MOVE, false);
        cout << "HHHHHHHEEEEEYYYY" << endl;
        recordMovesY(*vec, make_pair(robotLoc[idx]->first, robotLoc[idx]->second),  
        make_pair(get<0>(startingPushPositionAxis), get<1>(startingPushPositionAxis)), MOVE, false);

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


        startPushAxis axis = VERTICAL;
        // get current index of thread/robot
        int idx = RTinfo->idx_of_robot;

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
            cout << "the location of the robot before reg move was " << robotLoc[idx]->first << ", " << robotLoc[idx]->second << 
            "before the move\n";

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
        cout << " the location of robot after reg the move is " << robotLoc[idx]->first <<robotLoc[idx]->second <<endl;
    }
    void makePushMove(Direction dir, int idx){
       switch (dir){
            cout << "the location of the robot was " << robotLoc[idx]->first << ", " << robotLoc[idx]->second << 
            "before the move\n";
            cout << "the location of the box was " << boxLoc[idx]->first << ", " << boxLoc[idx]->second << 
            "before the move\n";
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
        cout << " the location of the robot after the move is " << robotLoc[idx]->first <<robotLoc[idx]->second <<endl;
        cout << "after move box is " << boxLoc[idx]->first << ", " << boxLoc[idx]->second << endl;

    }

    void destroyRobotsCommandsList(RThread* RTinfo){
        

    }



    void recordMovesX(vector<pair<Moves, Direction>>& RCList, pair<int, int> startingPoint, pair<int, int> destination, Moves argmove, bool pushCaseSecondLeg){

        int distanceFromRobToDestinationX = startingPoint.second - destination.second;
        if (distanceFromRobToDestinationX > 0){

            for (int i = 0; i < distanceFromRobToDestinationX; i++){
                    pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
                    robComm->second = WEST;
                    robComm->first = argmove;
                    RCList.push_back(*robComm);
                    if (pushCaseSecondLeg == true && i == distanceFromRobToDestinationX - 1){
                        pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
                        robComm->second = WEST;
                        robComm->first = MOVE;
                        RCList.push_back(*robComm);
                    }
            }
        }
        if (distanceFromRobToDestinationX < 0) {
            for (int i = 0; i > distanceFromRobToDestinationX; i--){
                    pair<Moves, Direction>* robComm= new pair<Moves, Direction>();
                    robComm->second = EAST;
                    robComm->first = argmove;
                    RCList.push_back(*robComm);
                    if (pushCaseSecondLeg == true && i == distanceFromRobToDestinationX + 1){
                        pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
                        robComm->second = EAST;
                        robComm->first = MOVE;
                        RCList.push_back(*robComm);
                    }
            }
        }
        if (distanceFromRobToDestinationX == 0){
        }
    }


    void recordMovesY(vector<pair<Moves, Direction>>& RCList, pair<int, int> startingPoint, pair<int, int> destination, 
    Moves argmove, bool pushCaseSecondLeg){

        // if this is NOT no_Y_Diff_Case... horizontal movement

        int distanceFromRobToDestinationY = startingPoint.first - destination.first;

        if (distanceFromRobToDestinationY > 0){
            for (int i = 0; i < distanceFromRobToDestinationY; i++){
                    pair<Moves, Direction>* robComm= new pair<Moves, Direction>();
                    robComm->second = NORTH;
                    robComm->first = argmove;
                    RCList.push_back(*robComm);
                    if (pushCaseSecondLeg == true && i == distanceFromRobToDestinationY - 1){
                        pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
                        robComm->second = NORTH;
                        robComm->first = MOVE;
                        RCList.push_back(*robComm);
                    }
            }
        }

        if (distanceFromRobToDestinationY < 0){
            for (int i = 0; i > distanceFromRobToDestinationY; i--){
                    pair<Moves, Direction>* robComm= new pair<Moves, Direction>();
                    robComm->second = SOUTH;
                    robComm->first = argmove;
                    RCList.push_back(*robComm);
                    if (pushCaseSecondLeg == true && i == distanceFromRobToDestinationY + 1){
                        pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
                        robComm->second = SOUTH;
                        robComm->first = MOVE;
                        RCList.push_back(*robComm);
                    }
            }
        }

        if (distanceFromRobToDestinationY == 0){
            cout << "no distance to travel Y axis" << endl;
        }
    }
};
