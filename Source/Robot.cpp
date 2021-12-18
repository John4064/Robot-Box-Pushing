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

typedef unsigned int uint;

namespace Robot{

    const int NORTHSOUTH = 0, EASTWEST = 1;

    extern vector<pair<uint, uint>*> robotLoc;

    void* robotThreadFunc(void * arg){

        // block until the GUI sets up.
        pthread_mutex_lock(&RThread::mutex);
        pthread_mutex_unlock(&RThread::mutex);
        RThread* RTinfo = (RThread*) arg;
        vector<pair<Moves, Direction>> threadsCommandList;
        RTinfo->genRobotsCommandsList(RTinfo);
        cout << "before make moves" << endl;
        RTinfo->robotMakeMoves();
        
        // RTinfo->printARobotsCommandList();
        return (new void*);
    }

    void RThread::fprintRobotMove(Moves move, Direction direction){
        pthread_mutex_lock(&file_mutex);
        ofstream myfile;
        myfile.open("robotSimulOut.txt", ios_base::app);
        string moveString = convertMoveEnumToWord( move);    
        string directionString = convertDirEnumToWord(direction);
        myfile << "robot " << idx_of_robot << "   \t"<< moveString << "   \t" << directionString << "\n";
        myfile.close();
        pthread_mutex_unlock(&file_mutex);
    }

    void RThread::printARobotsCommandList(){

        pthread_mutex_lock(&file_mutex);
        ofstream myfile;
        myfile.open("robotSimulOut.txt", ios_base::app);

        myfile << "Robot Program: Robot" << idx_of_robot << "\n";

        for(int i=0; i < copy_of_robot_moves.size(); i++){
            string moveString = convertMoveEnumToWord( copy_of_robot_moves[i].first);    
            string directionString = convertDirEnumToWord(copy_of_robot_moves[i].second);
            myfile << "\t" << moveString << "\t" << directionString << endl;
            myfile << "\n";
        }
        myfile.close();
        pthread_mutex_unlock(&file_mutex);
    }

    // Check if the cell you want to move 
    bool RThread::checkLocAlreadyExists(pair<uint, uint> locMovingTo, bool isReader){
        
        // test if we can even access this mutex location
        cout << "hellow2222" << endl;

        // switch this to true if find a conflict in occupancy of the cell moving to
        bool exists = false;
        cout << "12312312" << endl;
        // read/check all the boxes

        // now read/check all the robots
    for(int i=0; i<robotLoc.size(); i++){
        cout << "made it here" << endl;

    
        if (isReader == true){
            pthread_mutex_lock(robotLocProtectReaderCountMutexVec[i]);
            robotLocReaderCountVec[i]++;
			 cout << "reader count vec from inside function: i =" << i << "RCV = "<< RThread::robotLocReaderCountVec[i] << endl;;

            if (robotLocReaderCountVec[i] == 1){
                pthread_mutex_lock(robotLocWritingMutexVec[i]);
                cout << "exists acquired the writing lock" << endl;
            }
                    cout << "did I make it here" << endl;
            pthread_mutex_unlock(robotLocProtectReaderCountMutexVec[i]);
        }

        if ((locMovingTo.first == robotLoc[i]->first && locMovingTo.second == robotLoc[i]->second
            || locMovingTo.first == boxLoc[i]->first && locMovingTo.second == boxLoc[i]->second) && i != idx_of_robot){
                exists = true;
                cout << "it exists" << endl;
        }
                    cout << "did I make it here" << endl;
        if (isReader == true){
            pthread_mutex_lock(robotLocProtectReaderCountMutexVec[i]);
            robotLocReaderCountVec[i]--;
             cout << "reader count vec from inside function second: i =" << i << "RCV = "<< RThread::robotLocReaderCountVec[i] << endl;;

            if (robotLocReaderCountVec[i] == 0){
                pthread_mutex_unlock(robotLocWritingMutexVec[i]);
            }
            pthread_mutex_unlock(robotLocProtectReaderCountMutexVec[i]);
                    cout << "did I make it here all the way" << endl;
            }
        }

        // if bool is true, will need to write in the next function call
        // now that you are going to be writing, will need to place lock on and check everything again
        // while the lock still on.
        cout << "returning " << exists << endl;
        return exists;
    }


    pair<uint, uint> RThread::determineLocCommBringsUsToMOVE(Direction direction){
      
        int locY = robotLoc[idx_of_robot]->first;
        int locX = robotLoc[idx_of_robot]->second;

        cout << "original robotLoc " << locY << " " << locX << endl;

            switch (direction){
                    case NORTH:
                        locY = robotLoc[idx_of_robot]->first - 1;
                        break;
                    case SOUTH:
                        locY = robotLoc[idx_of_robot]->first + 1;
                        break;
                    case EAST:
                        locX = robotLoc[idx_of_robot]->second + 1;
                        break;
                    case WEST:
                        locX = robotLoc[idx_of_robot]->second - 1;
                        break;
                    case NOMOVEMENT:
                        break;
                }
        return (make_pair(locY, locX));
    }

   pair<uint, uint> RThread::determineLocCommBringsUsToPUSH(Direction direction){
        uint locY = boxLoc[idx_of_robot]->first;
        uint locX = boxLoc[idx_of_robot]->second;

         cout << "original boxLoc " << locY << " " << locX << endl;
        

        switch (direction){
            case NORTH:
                locY = boxLoc[idx_of_robot]->first - 1;
                break;
            case SOUTH:
                locY = boxLoc[idx_of_robot]->first + 1;
                break;
            case EAST:
                locX = boxLoc[idx_of_robot]->second + 1;
                break;
            case WEST:
                locX = boxLoc[idx_of_robot]->second - 1;
                break;
            case NOMOVEMENT:
                break;
        }
        return (make_pair(locY, locX));
    }


    /**
     * @brief The synchronization here is basically a reader writer problem.
     *         the worker threads are writing to the robot and box arrays 
     *         the gui is reading them. but the worker threads also act
     *         as readers as well, when they have to iterate through 
     *         every other location and test if a cell is occupied.
     * 
     */
    void RThread::robotMakeMoves(){
        while(!(thisRobotsMoves.empty())){
   
            pair<Moves, Direction> command = thisRobotsMoves.front();
            if (command.first == END){
                break;
            }

            pair<uint, uint> newLocBox;
            bool couldMakeMove = false;
            
            pair<uint, uint> newLocRobot = determineLocCommBringsUsToMOVE(command.second);

            int oldLocRobotY = robotLoc[idx_of_robot]->first;
            int oldLocRobotX = robotLoc[idx_of_robot]->second;

            if(command.first == PUSH){
                newLocBox = determineLocCommBringsUsToPUSH(command.second);
                cout << "the new B location is " << newLocBox.first << newLocBox.second << endl;
            }
            cout << "the new R location is " << newLocRobot.first << newLocRobot.second << endl;

            if (command.first == MOVE){
                while (checkLocAlreadyExists(newLocRobot, true)){};
                pthread_mutex_lock((*gridMutexVector[newLocRobot.first])[newLocRobot.second]);
            }
            if (command.first == PUSH){
                while (checkLocAlreadyExists(newLocBox, true)){};
                pthread_mutex_lock((*gridMutexVector[newLocBox.first])[newLocBox.second]);
            }
            if (command.first == END){
                pthread_mutex_unlock((*gridMutexVector[boxLoc[idx_of_robot]->first])[boxLoc[idx_of_robot]->second]);
            }
            pthread_mutex_lock(robotLocWritingMutexVec[idx_of_robot]);
            if (command.first == MOVE){
                if(!checkLocAlreadyExists(newLocRobot, false)){
                    makeRegMove(command.second, idx_of_robot);
                    couldMakeMove = true;
                }
            }
            if (command.first == PUSH){
                if(!checkLocAlreadyExists(newLocBox, false)){
                    makePushMove(command.second, idx_of_robot);
                    couldMakeMove = true;
                }
            }
            pthread_mutex_unlock(robotLocWritingMutexVec[idx_of_robot]);
            pthread_mutex_unlock((*gridMutexVector[oldLocRobotY])[oldLocRobotX]);
            

            if(!thisRobotsMoves.empty() && couldMakeMove == true){
                cout << "hi561" << endl;
                thisRobotsMoves.erase(thisRobotsMoves.begin());
            }
                   cout << "hi671" << endl;
            usleep(robotSleepTime);
        }
    }

 void RThread::genRobotsCommandsList(RThread* RTinfo){

    tuple <int, int, axis> startingPushPositionAxis;

    thisRobotsMoves = genCommGetBehindBox(RTinfo, startingPushPositionAxis);

    vector<pair<Moves, Direction>> movesToDoor = recordMovesPushToDoor(RTinfo, startingPushPositionAxis);

    //probably an easier way to do this...
    for (uint i = 0; i < movesToDoor.size(); i++){
        thisRobotsMoves.push_back(movesToDoor[i]);
    }

    pair<Moves, Direction>* lastCommand = new pair<Moves, Direction>();
    lastCommand->first = END;
    lastCommand->second = NOMOVEMENT;

    thisRobotsMoves.push_back(*lastCommand);

    RTinfo->commandsListHolder.push_back(thisRobotsMoves);


    for (uint i=0; i < thisRobotsMoves.size(); i++){

        pair<Moves, Direction>* pairP = new pair<Moves, Direction>();
        pairP->first = thisRobotsMoves[i].first;
        pairP->second = thisRobotsMoves[i].second;
        copy_of_robot_moves.push_back(*pairP);
    }
    pthread_mutex_lock(&file_mutex);
    cout << "robot command list generated" <<endl;
    pthread_mutex_unlock(&file_mutex);
}

    vector<pair<Moves, Direction>> recordMovesPushToDoor(RThread* RTinfo, tuple <int, int, axis> startingPushPositionAxis){

        // the default is that we are on the first leg of the push trip
        legStatus leg_status = legStatus::ON_MOVE_OR_FIRST_LEG_PUSH;

        vector<pair<Moves, Direction>>* movesPushtoDoor = new vector<pair<Moves, Direction>>();

        int idx = RTinfo->idx_of_robot;
        
        // pair<uint,uint>* origStartingPoint = robotLoc[idx];
        pair<uint, uint>* destination = doorLoc[doorAssign[idx]];
        pair<int, int> pushStartingPoint = make_pair(get<0> (startingPushPositionAxis), get<1> (startingPushPositionAxis));

        if (pushStartingPoint.first - destination->first == 0 || pushStartingPoint.second - destination->second == 0){
            leg_status = legStatus::NO_SECOND_LEG;
        }

        if ((get<2> (startingPushPositionAxis)) == HORIZONTAL){
            recordMovesX(*movesPushtoDoor, pushStartingPoint, *destination, PUSH, leg_status); 
            if (leg_status == NO_SECOND_LEG){
                return *movesPushtoDoor;
             }       
            if (!movesPushtoDoor->empty() && leg_status != NO_SECOND_LEG){
                movesPushtoDoor->pop_back();
            }
            // movesPushtoDoor->pop_back();
            pair<uint, uint> startingPushPosAfterRotation = recordRotationToSecondPushPos(*movesPushtoDoor, startingPushPositionAxis, idx);
            leg_status = legStatus::ON_SECOND_LEG;
            recordMovesY(*movesPushtoDoor,startingPushPosAfterRotation, *destination, PUSH, leg_status);
        }

        if ((get<2>(startingPushPositionAxis)) == VERTICAL){
            recordMovesY(*movesPushtoDoor, pushStartingPoint, *destination, PUSH, leg_status);
            if (leg_status == NO_SECOND_LEG){
                return *movesPushtoDoor;
            }
            if (!movesPushtoDoor->empty()&& leg_status != NO_SECOND_LEG){
                movesPushtoDoor->pop_back();
            }
            // movesPushtoDoor->pop_back();
            pair<uint, uint> startingPushPosAfterRotation =  recordRotationToSecondPushPos(*movesPushtoDoor, startingPushPositionAxis, idx);
            leg_status = legStatus::ON_SECOND_LEG;
            recordMovesX(*movesPushtoDoor, startingPushPosAfterRotation, *destination, PUSH, leg_status);
        }

        return *movesPushtoDoor;
    }

    vector<pair<Moves, Direction>> genCommGetBehindBox(RThread* RTinfo, tuple <int, int, axis>& startingPushPositionAxis){
        // determine the starting push position 
        startingPushPositionAxis = determineStartingPushPositionAxis(RTinfo);
        return recordMovesToBehindBox(startingPushPositionAxis, RTinfo);
    }


   vector<pair<Moves, Direction>> recordMovesToBehindBox(tuple <int, int, axis> startingPushPositionAxis, RThread* RTinfo){\
        int idx = RTinfo->idx_of_robot;
        legStatus leg_status = ON_MOVE_OR_FIRST_LEG_PUSH;

        // may have to change this later if this doesn't work
        vector<pair<Moves, Direction>>* vec = new  vector<pair<Moves, Direction>>();
        // bool verticalShouldBeFirst = collisionWithBoxAvoider(startingPushPositionAxis, idx, goAround);
        recordMovesX(*vec, make_pair(robotLoc[idx]->first, robotLoc[idx]->second), make_pair(get<0>(startingPushPositionAxis), get<1>(startingPushPositionAxis)), MOVE, leg_status);
        recordMovesY(*vec, make_pair(robotLoc[idx]->first, robotLoc[idx]->second),  
        make_pair(get<0>(startingPushPositionAxis), get<1>(startingPushPositionAxis)), MOVE, leg_status);

        return *vec;
    }


    pair <uint, uint> recordRotationToSecondPushPos(vector<pair<Moves, Direction>>& RCList, tuple <int, int, axis> 
        startingPushPositionAxis, int idx){
        
        uint startY = get<0>(startingPushPositionAxis);
        uint startX = get<1>(startingPushPositionAxis);

        uint newPushPosX = startX, newPushPosY = startY;

        int distanceFromOrigBoxToDoorY = startY - doorLoc[doorAssign[idx]]->first;
        int distanceFromOrigBoxToDoorX = startX -  doorLoc[doorAssign[idx]]->second;
        axis pushToDoorAxis = get<2>(startingPushPositionAxis);
         fflush(stdout);
         

        pair<int, int> destination;

        if (pushToDoorAxis == VERTICAL && distanceFromOrigBoxToDoorY > 0 && distanceFromOrigBoxToDoorX > 0){
            pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
            robComm->second = EAST;
            robComm->first = MOVE;
            newPushPosX++;
            RCList.push_back(*robComm);
            robComm = new pair<Moves, Direction>();
            robComm->second = NORTH;
            robComm->first = MOVE;
            newPushPosY--;
            RCList.push_back(*robComm);
            return (make_pair(newPushPosY, newPushPosX));    
        }
        if (pushToDoorAxis == VERTICAL && distanceFromOrigBoxToDoorY > 0 && distanceFromOrigBoxToDoorX < 0){
            pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
            robComm->second = WEST;
            robComm->first = MOVE;
            newPushPosX--;
            RCList.push_back(*robComm);
            robComm = new pair<Moves, Direction>();
            robComm->second = NORTH;
            robComm->first = MOVE;
            newPushPosY--;
            RCList.push_back(*robComm);
           return (make_pair(newPushPosY, newPushPosX));    
        }

        
        if (pushToDoorAxis == VERTICAL && distanceFromOrigBoxToDoorY < 0 && distanceFromOrigBoxToDoorX > 0){
            pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
            robComm->second = EAST;
            robComm->first = MOVE;
            newPushPosX++;
            RCList.push_back(*robComm);
            robComm = new pair<Moves, Direction>();
            robComm->second = SOUTH;
            robComm->first = MOVE;
            newPushPosY++;
            RCList.push_back(*robComm);
            return (make_pair(newPushPosY, newPushPosX));      
        }
        if (pushToDoorAxis == VERTICAL && distanceFromOrigBoxToDoorY < 0 && distanceFromOrigBoxToDoorX < 0){
            pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
            robComm->second = WEST;
            robComm->first = MOVE;
            newPushPosX--;
            RCList.push_back(*robComm);
            robComm = new pair<Moves, Direction>();
            robComm->second = SOUTH;
            robComm->first = MOVE;
            newPushPosY++;
            RCList.push_back(*robComm);
            return (make_pair(newPushPosY, newPushPosX));    
        }


        if (pushToDoorAxis == HORIZONTAL && distanceFromOrigBoxToDoorY > 0 && distanceFromOrigBoxToDoorX > 0){
            pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
            robComm->second = SOUTH;
            robComm->first = MOVE;
            newPushPosY++;
            RCList.push_back(*robComm);
            robComm = new pair<Moves, Direction>();
            robComm->second = WEST;
            robComm->first = MOVE;
            newPushPosX--;
            RCList.push_back(*robComm);
            return (make_pair(newPushPosY, newPushPosX));    
        }
        if (pushToDoorAxis == HORIZONTAL && distanceFromOrigBoxToDoorY > 0 && distanceFromOrigBoxToDoorX < 0){
            pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
            robComm->second = SOUTH;
            robComm->first = MOVE;
            newPushPosY++;
            RCList.push_back(*robComm);
            robComm = new pair<Moves, Direction>();
            robComm->second = EAST;
            robComm->first = MOVE;
            newPushPosX++;
            RCList.push_back(*robComm);
            return (make_pair(newPushPosY, newPushPosX));       
        }

        
        if (pushToDoorAxis == HORIZONTAL && distanceFromOrigBoxToDoorY < 0 && distanceFromOrigBoxToDoorX > 0){
            pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
            robComm->second = NORTH;
            robComm->first = MOVE;
            newPushPosY--;
            RCList.push_back(*robComm);
            robComm = new pair<Moves, Direction>();
            robComm->second = WEST;
            robComm->first = MOVE;
            newPushPosX--;
            RCList.push_back(*robComm);
            return (make_pair(newPushPosY, newPushPosX));     
        }

        if (pushToDoorAxis == HORIZONTAL && distanceFromOrigBoxToDoorY < 0 && distanceFromOrigBoxToDoorY < 0){
            pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
            robComm->second = NORTH;
            robComm->first = MOVE;
            newPushPosY--;
            RCList.push_back(*robComm);
            robComm = new pair<Moves, Direction>();
            robComm->second = EAST;
            robComm->first = MOVE;
            newPushPosX++;
            RCList.push_back(*robComm);
            return (make_pair(newPushPosY, newPushPosX));      
        }

        if (distanceFromOrigBoxToDoorY == 0 || distanceFromOrigBoxToDoorX == 0) {
            cout << "error establishing second push position " << endl;
            // exit(99);
        }
        cout << "AN ERROR OCCURRED...." << endl;
        return make_pair(-1u, -1u);
        // exit(99);
    }

   tuple<int, int, axis> determineStartingPushPositionAxis(RThread * RTinfo){
       // assume we will push vertically first, then horizontally

       // first need to figure out if need to push vertically at all

       // variable where we store if this is situation where don't need to push at all in 
       // vertical axis default is that you do need to push vertically
        axis axis = VERTICAL;
        // get current index of thread/robot
        int idx = RTinfo->idx_of_robot;
        int startPushTargY, startPushTargX;

        // to figure out what side of the box we need to push, it needs to be whatever
        // is the opposite side from the door.  So figure out what side the door is on compared
        // with the Box.  In other words if it is a positive diff. Box - door or a negative difference.
        // If box - door is positive, the door is closer to the top row, and vice versa.

        int yDiffBoxDoor = boxLoc[idx]->first - doorLoc[doorAssign[idx]]->first;        

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
            break;
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
                dirAsString = "N";
            break;
            case SOUTH:
                dirAsString = "S";
            break;
            case EAST:
                dirAsString = "E";
            break;
            case WEST:
                dirAsString = "W";
            break;
            case NOMOVEMENT:
                dirAsString = "";
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



    void recordMovesX(vector<pair<Moves, Direction>>& RCList, pair<int, int> startingPoint, pair<int, int> destination, Moves argmove, legStatus leg_status){

        int distanceFromRobToDestinationX = startingPoint.second - destination.second;
        if (distanceFromRobToDestinationX > 0){

            for (int i = 0; i < distanceFromRobToDestinationX; i++){
                    if (i == distanceFromRobToDestinationX - 1 && (leg_status == ON_SECOND_LEG  || leg_status == NO_SECOND_LEG)){
                        pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
                        robComm->second = WEST;
                        robComm->first = MOVE;
                        RCList.push_back(*robComm);
                        break;
                    }
                    pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
                    robComm->second = WEST;
                    robComm->first = argmove;
                    RCList.push_back(*robComm);
            }
        }
        if (distanceFromRobToDestinationX < 0) {
            for (int i = 0; i > distanceFromRobToDestinationX; i--){
                    if (i == distanceFromRobToDestinationX + 1 && (leg_status == ON_SECOND_LEG  || leg_status == NO_SECOND_LEG)){
                        pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
                        robComm->second = EAST;
                        robComm->first = MOVE;
                        RCList.push_back(*robComm);
                        break;
                    }
                    pair<Moves, Direction>* robComm= new pair<Moves, Direction>();
                    robComm->second = EAST;
                    robComm->first = argmove;
                    RCList.push_back(*robComm);
         
            }
        }
        if (distanceFromRobToDestinationX == 0){
        }
    }


    void recordMovesY(vector<pair<Moves, Direction>>& RCList, pair<int, int> startingPoint, pair<int, int> destination, 
    Moves argmove, legStatus leg_status){

        // if this is NOT no_Y_Diff_Case... horizontal movement

        int distanceFromRobToDestinationY = startingPoint.first - destination.first;

        if (distanceFromRobToDestinationY > 0){
            for (int i = 0; i < distanceFromRobToDestinationY; i++){
                    if (i == distanceFromRobToDestinationY - 1 && (leg_status == ON_SECOND_LEG  || leg_status == NO_SECOND_LEG)){
                        pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
                        robComm->second = NORTH;
                        robComm->first = MOVE;
                        RCList.push_back(*robComm);
                        break;
                    }
                    pair<Moves, Direction>* robComm= new pair<Moves, Direction>();
                    robComm->second = NORTH;
                    robComm->first = argmove;
                    RCList.push_back(*robComm);
            }
        }

        if (distanceFromRobToDestinationY < 0){
            for (int i = 0; i > distanceFromRobToDestinationY; i--){

                    if (i == distanceFromRobToDestinationY + 1 && (leg_status == ON_SECOND_LEG  || leg_status == NO_SECOND_LEG)){
                        pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
                        robComm->second = SOUTH;
                        robComm->first = MOVE;
                        RCList.push_back(*robComm);
                        break;
                    }                
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
