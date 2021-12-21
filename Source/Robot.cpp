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
extern uint numLiveThreads;
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

    /**
     * @brief A function that 
     * 
     * @param arg 
     * @return void* 
     */
    void* robotThreadFunc(void * arg){
        
        // for each thread this number goes up until the thread completes
        numLiveThreads++;

        // block until the GUI sets up.
        pthread_mutex_lock(&RThread::mutex);
        // unlock after GUI sets up... lock no longer needed
        pthread_mutex_unlock(&RThread::mutex);
        RThread* RTinfo = (RThread*) arg;
        RTinfo->genRobotsCommandsList(RTinfo);
        RTinfo->robotMakeMovesnPrint();
        RTinfo->freeThreadMemory();

        return (NULL);
    }

    /**
     * @brief A function that frees the dynamic memory used by the Thread
     * 
     */
    void RThread::freeThreadMemory(){

        for (int i=0; i < copy_of_robot_moves.size();i++){
            delete(copy_of_robot_moves[i]);
        }
    }

    void RThread::fprintRobotMove(Moves move, Direction direction){
        pthread_mutex_lock(&file_mutex);
        ofstream myfile;
        myfile.open("robotSimulOut.txt", ios_base::app);
        string moveString = convertMoveEnumToWord(move);    
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
            string moveString = convertMoveEnumToWord( copy_of_robot_moves[i]->first);    
            string directionString = convertDirEnumToWord(copy_of_robot_moves[i]->second);
            myfile << "\t" << moveString << "\t" << directionString << endl;
            myfile << "\n";
        }
        myfile.close();
        pthread_mutex_unlock(&file_mutex);
    }

    // Check if the cell you want to move 
    bool RThread::checkLocAlreadyExists(pair<uint, uint> locMovingTo, bool isReader){
        
        // test if we can even access this mutex location

        // switch this to true if find a conflict in occupancy of the cell moving to
        bool exists = false;
        // read/check all the boxes

        // now read/check all the robots
    for(int i=0; i<robotLoc.size(); i++){    
        if (isReader == true){
            pthread_mutex_lock(robotLocProtectReaderCountMutexVec[i]);
            robotLocReaderCountVec[i]++;
			
            if (robotLocReaderCountVec[i] == 1){
                pthread_mutex_lock(robotLocWritingMutexVec[i]);
            }
            pthread_mutex_unlock(robotLocProtectReaderCountMutexVec[i]);
        }

        if ((locMovingTo.first == robotLoc[i]->first && locMovingTo.second == robotLoc[i]->second
            || locMovingTo.first == boxLoc[i]->first && locMovingTo.second == boxLoc[i]->second) && i != idx_of_robot){
                exists = true;
        }
        if (isReader == true){
            pthread_mutex_lock(robotLocProtectReaderCountMutexVec[i]);
            robotLocReaderCountVec[i]--;
            
            if (robotLocReaderCountVec[i] == 0){
                pthread_mutex_unlock(robotLocWritingMutexVec[i]);
            }
            pthread_mutex_unlock(robotLocProtectReaderCountMutexVec[i]);
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
    void RThread::robotMakeMovesnPrint(){

        while(!(thisRobotsMoves->empty()) && this->stillAlive == true){
   
            pair<Moves, Direction>* command = thisRobotsMoves->front();
     
            if (command->first == END){
                fprintRobotMove(command->first, command->second);
                this->stillAlive = false;
                thisRobotsMoves->clear();
                break;
            }

            if (thisRobotsMoves->size() == 2){
                pthread_mutex_unlock((*gridMutexVector[boxLoc[idx_of_robot]->first])[boxLoc[idx_of_robot]->second]);
                boxLoc[idx_of_robot]->first = -1;
                boxLoc[idx_of_robot]->second = -1;
            }

            bool couldMakeMove = false;
            pair<uint, uint> newLocBox;
            pair<uint, uint> newLocRobot = determineLocCommBringsUsToMOVE(command->second);

            int oldLocRobotY = robotLoc[idx_of_robot]->first;
            int oldLocRobotX = robotLoc[idx_of_robot]->second;

            if(command->first == PUSH){
                newLocBox = determineLocCommBringsUsToPUSH(command->second);
            }

            if (command->first == MOVE){
                while (checkLocAlreadyExists(newLocRobot, true)){};
                pthread_mutex_lock((*gridMutexVector[newLocRobot.first])[newLocRobot.second]);
            }
            if (command->first == PUSH){
                while (checkLocAlreadyExists(newLocBox, true)){};
                pthread_mutex_lock((*gridMutexVector[newLocBox.first])[newLocBox.second]);
            }

            pthread_mutex_lock(robotLocWritingMutexVec[idx_of_robot]);

            if (command->first == MOVE){
                if(!checkLocAlreadyExists(newLocRobot, false)){
                    makeRegMove(command->second, idx_of_robot);
                    couldMakeMove = true;
                    fprintRobotMove(command->first, command->second);
                }
            }
            if (command->first == PUSH){
                if(!checkLocAlreadyExists(newLocBox, false)){
                    makePushMove(command->second, idx_of_robot);
                    couldMakeMove = true;
                    fprintRobotMove(command->first, command->second);
                }
            }
            pthread_mutex_unlock(robotLocWritingMutexVec[idx_of_robot]);
            pthread_mutex_unlock((*gridMutexVector[oldLocRobotY])[oldLocRobotX]);

            if(!thisRobotsMoves->empty() && couldMakeMove == true){
                // pair<Moves, Direction> *movePointer = &thisRobotsMoves[0];
                thisRobotsMoves->erase(thisRobotsMoves->begin());
                // delete movePointer;
            }
            usleep(robotSleepTime);
        }

        if (thisRobotsMoves->empty()){
            // pthread_mutex_unlock((*gridMutexVector[boxLoc[idx_of_robot]->first])[boxLoc[idx_of_robot]->second]);
            delete(thisRobotsMoves);
            pthread_mutex_unlock((*gridMutexVector[robotLoc[idx_of_robot]->first])[robotLoc[idx_of_robot]->second]);
            robotLoc[idx_of_robot]->first = -1;
            robotLoc[idx_of_robot]->second = -1;
        }
    }

/**
 * @brief The main function that generates the list of commands that are executed by each robot
 * 
 * @param RTinfo 
 */
 void RThread::genRobotsCommandsList(RThread* RTinfo){

     // A tuple that holds both the coordinates for the starting position from which the robot begins
     // "pushing" and the third element is the axis down which the robot pushes
    tuple <int, int, axis> startingPushPositionAxis;

    // This function returns the moves that are required to get to the push position
    thisRobotsMoves = genCommGetBehindBox(RTinfo, startingPushPositionAxis);

    // This function returns rest of the moves
    vector<pair<Moves, Direction>*>* movesToDoor = recordMovesPushToDoor(RTinfo, startingPushPositionAxis);

    // the moves are pushed back into a member variable
    for (uint i = 0; i < movesToDoor->size(); i++){
        thisRobotsMoves->push_back((*movesToDoor)[i]);
    }

    // we tag on the last command here
    pair<Moves, Direction>* lastCommand = new pair<Moves, Direction>();
    lastCommand->first = END;
    lastCommand->second = NOMOVEMENT;

    thisRobotsMoves->push_back(lastCommand);

    // we make a copy of the moves array just in case we need it since we are 
    // going to 
    for (uint i=0; i < thisRobotsMoves->size(); i++){
        pair<Moves, Direction>* pairP = new pair<Moves, Direction>();
        pairP->first = (*thisRobotsMoves)[i]->first;
        pairP->second = (*thisRobotsMoves)[i]->second;
        copy_of_robot_moves.push_back(pairP);
    }
}

    vector<pair<Moves, Direction>*>* recordMovesPushToDoor(RThread* RTinfo, tuple <int, int, axis> startingPushPositionAxis){

        // the default is that we are on the first leg of the push trip
        legStatus leg_status = legStatus::ON_MOVE_OR_FIRST_LEG_PUSH;

        vector<pair<Moves, Direction>*>* movesPushtoDoor = new vector<pair<Moves, Direction>*>();

        int idx = RTinfo->idx_of_robot;
        
        // pair<uint,uint>* origStartingPoint = robotLoc[idx];
        pair<uint, uint>* destination = doorLoc[doorAssign[idx]];
        pair<int, int> pushStartingPoint = make_pair(get<0> (startingPushPositionAxis), get<1> (startingPushPositionAxis));

        if (pushStartingPoint.first - destination->first == 0 || pushStartingPoint.second - destination->second == 0){
            leg_status = legStatus::NO_SECOND_LEG;
        }

        if ((get<2> (startingPushPositionAxis)) == HORIZONTAL){
            recordMovesX(movesPushtoDoor, pushStartingPoint, *destination, PUSH, leg_status); 
            if (leg_status == NO_SECOND_LEG){
                return movesPushtoDoor;
             }       
            if (!movesPushtoDoor->empty() && leg_status != NO_SECOND_LEG){
                movesPushtoDoor->pop_back();
            }
            // movesPushtoDoor->pop_back();
            pair<uint, uint> startingPushPosAfterRotation = recordRotationToSecondPushPos(movesPushtoDoor, startingPushPositionAxis, idx);
            leg_status = legStatus::ON_SECOND_LEG;
            recordMovesY(movesPushtoDoor,startingPushPosAfterRotation, *destination, PUSH, leg_status);
        }

        if ((get<2>(startingPushPositionAxis)) == VERTICAL){
            recordMovesY(movesPushtoDoor, pushStartingPoint, *destination, PUSH, leg_status);
            if (leg_status == NO_SECOND_LEG){
                return movesPushtoDoor;
            }
            if (!movesPushtoDoor->empty()&& leg_status != NO_SECOND_LEG){
                movesPushtoDoor->pop_back();
            }
            // movesPushtoDoor->pop_back();
            pair<uint, uint> startingPushPosAfterRotation =  recordRotationToSecondPushPos(movesPushtoDoor, startingPushPositionAxis, idx);
            leg_status = legStatus::ON_SECOND_LEG;
            recordMovesX(movesPushtoDoor, startingPushPosAfterRotation, *destination, PUSH, leg_status);
        }

        return movesPushtoDoor;
    }

    vector<pair<Moves, Direction>*>* RThread::genCommGetBehindBox(RThread* RTinfo, tuple <int, int, axis>& startingPushPositionAxis){
        // determine the starting push position 
        startingPushPositionAxis = determineStartingPushPositionAxis(RTinfo);
        return recordMovesToBehindBox(startingPushPositionAxis, RTinfo);
    }


// true = y first, false = x first
 bool RThread::recordYfirst(tuple <int, int, axis> startingPushPositionAxis){

    int pushDestY = get<0>(startingPushPositionAxis);
    int pushDestX = get<1>(startingPushPositionAxis);
    int robtY = robotLoc[idx_of_robot]->first;
    int robtX = robotLoc[idx_of_robot]->second;
    int boxLocY = boxLoc[idx_of_robot]->first;
    int boxLocX = boxLoc[idx_of_robot]->second;


    int dogLegDest1Y = pushDestY;
    int dogLegDest1X = robtX;
    int dogLegDest2Y = robtY;
    int dogLegDest2X = pushDestX;

    // case where 
    if (((dogLegDest1X > robtX && dogLegDest1X < pushDestX ||
        dogLegDest1X > pushDestX && dogLegDest1X < robtX) && dogLegDest1Y == pushDestY)
        || 
        ((dogLegDest1Y > robtY && dogLegDest1Y < pushDestY ||
        dogLegDest1Y > pushDestY && dogLegDest1Y < robtY) && dogLegDest1X == pushDestX)) {
            return false;
        }

    if (((dogLegDest2X > robtX && dogLegDest2X < pushDestX ||
        dogLegDest2X > pushDestX && dogLegDest2X < robtX) && dogLegDest2Y == pushDestY)
        || 
        ((dogLegDest2Y > robtY && dogLegDest2Y < pushDestY ||
        dogLegDest2Y > pushDestY && dogLegDest2Y < robtY) && dogLegDest2X == pushDestX)) {
        }
        {
            return true;  
        }



    cout << "something went wrong..." << endl;
    return false;
 }

   vector<pair<Moves, Direction>*>* RThread::recordMovesToBehindBox(tuple <int, int, axis> startingPushPositionAxis, RThread* RTinfo){
        int idx = idx_of_robot;
        legStatus leg_status = ON_MOVE_OR_FIRST_LEG_PUSH;

        // may have to change this later if this doesn't work
        vector<pair<Moves, Direction>*>* vec = new vector<pair<Moves, Direction>*>();
        bool recordYFirst =  recordYfirst(startingPushPositionAxis);
        if (recordYFirst){
            recordMovesY(vec, make_pair(robotLoc[idx]->first, robotLoc[idx]->second),  
            make_pair(get<0>(startingPushPositionAxis), get<1>(startingPushPositionAxis)), MOVE, leg_status);
            recordMovesX(vec, make_pair(robotLoc[idx]->first, robotLoc[idx]->second), make_pair(get<0>(startingPushPositionAxis), get<1>(startingPushPositionAxis)), MOVE, leg_status);
        // bool verticalShouldBeFirst = collisionWithBoxAvoider(startingPushPositionAxis, idx, goAround);
        }
    if (!recordYFirst){
         recordMovesX(vec, make_pair(robotLoc[idx]->first, robotLoc[idx]->second), make_pair(get<0>(startingPushPositionAxis), get<1>(startingPushPositionAxis)), MOVE, leg_status);
            recordMovesY(vec, make_pair(robotLoc[idx]->first, robotLoc[idx]->second),  
            make_pair(get<0>(startingPushPositionAxis), get<1>(startingPushPositionAxis)), MOVE, leg_status);
        // bool verticalShouldBeFirst = collisionWithBoxAvoider(startingPushPositionAxis, idx, goAround);
        }

        return vec;
    }


    pair <uint, uint> recordRotationToSecondPushPos(vector<pair<Moves, Direction>*>* RCList, tuple <int, int, axis> 
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
            RCList->push_back(robComm);
            robComm = new pair<Moves, Direction>();
            robComm->second = NORTH;
            robComm->first = MOVE;
            newPushPosY--;
            RCList->push_back(robComm);
            return (make_pair(newPushPosY, newPushPosX));    
        }
        if (pushToDoorAxis == VERTICAL && distanceFromOrigBoxToDoorY > 0 && distanceFromOrigBoxToDoorX < 0){
            pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
            robComm->second = WEST;
            robComm->first = MOVE;
            newPushPosX--;
            RCList->push_back(robComm);
            robComm = new pair<Moves, Direction>();
            robComm->second = NORTH;
            robComm->first = MOVE;
            newPushPosY--;
            RCList->push_back(robComm);
           return (make_pair(newPushPosY, newPushPosX));    
        }

        
        if (pushToDoorAxis == VERTICAL && distanceFromOrigBoxToDoorY < 0 && distanceFromOrigBoxToDoorX > 0){
            pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
            robComm->second = EAST;
            robComm->first = MOVE;
            newPushPosX++;
            RCList->push_back(robComm);
            robComm = new pair<Moves, Direction>();
            robComm->second = SOUTH;
            robComm->first = MOVE;
            newPushPosY++;
            RCList->push_back(robComm);
            return (make_pair(newPushPosY, newPushPosX));      
        }
        if (pushToDoorAxis == VERTICAL && distanceFromOrigBoxToDoorY < 0 && distanceFromOrigBoxToDoorX < 0){
            pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
            robComm->second = WEST;
            robComm->first = MOVE;
            newPushPosX--;
            RCList->push_back(robComm);
            robComm = new pair<Moves, Direction>();
            robComm->second = SOUTH;
            robComm->first = MOVE;
            newPushPosY++;
            RCList->push_back(robComm);
            return (make_pair(newPushPosY, newPushPosX));    
        }


        if (pushToDoorAxis == HORIZONTAL && distanceFromOrigBoxToDoorY > 0 && distanceFromOrigBoxToDoorX > 0){
            pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
            robComm->second = SOUTH;
            robComm->first = MOVE;
            newPushPosY++;
            RCList->push_back(robComm);
            robComm = new pair<Moves, Direction>();
            robComm->second = WEST;
            robComm->first = MOVE;
            newPushPosX--;
            RCList->push_back(robComm);
            return (make_pair(newPushPosY, newPushPosX));    
        }
        if (pushToDoorAxis == HORIZONTAL && distanceFromOrigBoxToDoorY > 0 && distanceFromOrigBoxToDoorX < 0){
            pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
            robComm->second = SOUTH;
            robComm->first = MOVE;
            newPushPosY++;
            RCList->push_back(robComm);
            robComm = new pair<Moves, Direction>();
            robComm->second = EAST;
            robComm->first = MOVE;
            newPushPosX++;
            RCList->push_back(robComm);
            return (make_pair(newPushPosY, newPushPosX));       
        }

        
        if (pushToDoorAxis == HORIZONTAL && distanceFromOrigBoxToDoorY < 0 && distanceFromOrigBoxToDoorX > 0){
            pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
            robComm->second = NORTH;
            robComm->first = MOVE;
            newPushPosY--;
            RCList->push_back(robComm);
            robComm = new pair<Moves, Direction>();
            robComm->second = WEST;
            robComm->first = MOVE;
            newPushPosX--;
            RCList->push_back(robComm);
            return (make_pair(newPushPosY, newPushPosX));     
        }

        if (pushToDoorAxis == HORIZONTAL && distanceFromOrigBoxToDoorY < 0 && distanceFromOrigBoxToDoorY < 0){
            pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
            robComm->second = NORTH;
            robComm->first = MOVE;
            newPushPosY--;
            RCList->push_back(robComm);
            robComm = new pair<Moves, Direction>();
            robComm->second = EAST;
            robComm->first = MOVE;
            newPushPosX++;
            RCList->push_back(robComm);
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

   tuple<int, int, axis> RThread::determineStartingPushPositionAxis(RThread * RTinfo){

       axis preferredFirstAxis;

       if (boxLoc[idx_of_robot]->first == robotLoc[idx_of_robot]->first){
           preferredFirstAxis = HORIZONTAL;
       }
       if (boxLoc[idx_of_robot]->second == robotLoc[idx_of_robot]->second){
           preferredFirstAxis = VERTICAL;
       }
    
        if (boxLoc[idx_of_robot]->first == doorLoc[doorAssign[idx_of_robot]]->first){
           preferredFirstAxis = HORIZONTAL;
       }
       if (boxLoc[idx_of_robot]->second == doorLoc[doorAssign[idx_of_robot]]->second){
           preferredFirstAxis = VERTICAL;
       }

        int startPushTargY, startPushTargX;

        // to figure out what side of the box we need to push, it needs to be whatever
        // is the opposite side from the door.  So figure out what side the door is on compared
        // with the Box.  In other words if it is a positive diff. Box - door or a negative difference.
        // If box - door is positive, the door is closer to the top row, and vice versa.

        int yDiffBoxDoor = boxLoc[idx_of_robot]->first - doorLoc[doorAssign[idx_of_robot]]->first; 
        int xDiffBoxDoor = boxLoc[idx_of_robot]->second - doorLoc[doorAssign[idx_of_robot]]->second;

        if (preferredFirstAxis == VERTICAL){
        //if door closer to top, we want to push the box from one greater row number than the box
            if (yDiffBoxDoor > 0){
                startPushTargY = boxLoc[idx_of_robot]->first + 1;
            }
            // if door closer to top, we want to push the box from one lesser row number than the box
            if (yDiffBoxDoor < 0){
                startPushTargY = boxLoc[idx_of_robot]->first -1;
            }
            // if door is the same, there is no distance to travel
            if (yDiffBoxDoor == 0){
                startPushTargY = boxLoc[idx_of_robot]->first;
            }
            startPushTargX = boxLoc[idx_of_robot]->second;
        }

        if (preferredFirstAxis == HORIZONTAL){
        //if door closer to top, we want to push the box from one greater row number than the box
            if (xDiffBoxDoor > 0){
                startPushTargX = boxLoc[idx_of_robot]->second + 1;
            }
            // if door closer to top, we want to push the box from one lesser row number than the box
            if (xDiffBoxDoor < 0){
                startPushTargX = boxLoc[idx_of_robot]->second -1;
            }
            // if door is the same, there is no distance to travel
            if (xDiffBoxDoor == 0){
                startPushTargX = boxLoc[idx_of_robot]->second;
            }
            startPushTargY = boxLoc[idx_of_robot]->first;
        }

        // record start values and return tuple
        return make_tuple(startPushTargY, startPushTargX, preferredFirstAxis);
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



    void recordMovesX(vector<pair<Moves, Direction>*>* RCList, pair<int, int> startingPoint, pair<int, int> destination, Moves argmove, legStatus leg_status){

        int distanceFromRobToDestinationX = startingPoint.second - destination.second;
        if (distanceFromRobToDestinationX > 0){
            for (int i = 0; i < distanceFromRobToDestinationX; i++){
                    if (i == distanceFromRobToDestinationX - 1 && (leg_status == ON_SECOND_LEG  || leg_status == NO_SECOND_LEG)){
                        pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
                        robComm->second = WEST;
                        robComm->first = MOVE;
                        RCList->push_back(robComm);
                        break;
                    }
                    pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
                    robComm->second = WEST;
                    robComm->first = argmove;
                    RCList->push_back(robComm);
            }
        }
        if (distanceFromRobToDestinationX < 0) {
            for (int i = 0; i > distanceFromRobToDestinationX; i--){
                    if (i == distanceFromRobToDestinationX + 1 && (leg_status == ON_SECOND_LEG  || leg_status == NO_SECOND_LEG)){
                        pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
                        robComm->second = EAST;
                        robComm->first = MOVE;
                        RCList->push_back(robComm);
                        break;
                    }
                    pair<Moves, Direction>* robComm= new pair<Moves, Direction>();
                    robComm->second = EAST;
                    robComm->first = argmove;
                    RCList->push_back(robComm);
         
            }
        }
        if (distanceFromRobToDestinationX == 0){
        }
    }


    void recordMovesY(vector<pair<Moves, Direction>*>* RCList, pair<int, int> startingPoint, pair<int, int> destination, 
    Moves argmove, legStatus leg_status){

        // if this is NOT no_Y_Diff_Case... horizontal movement

        int distanceFromRobToDestinationY = startingPoint.first - destination.first;

        if (distanceFromRobToDestinationY > 0){
            for (int i = 0; i < distanceFromRobToDestinationY; i++){
                    if (i == distanceFromRobToDestinationY - 1 && (leg_status == ON_SECOND_LEG  || leg_status == NO_SECOND_LEG)){
                        pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
                        robComm->second = NORTH;
                        robComm->first = MOVE;
                        RCList->push_back(robComm);
                        break;
                    }
                    pair<Moves, Direction>* robComm= new pair<Moves, Direction>();
                    robComm->second = NORTH;
                    robComm->first = argmove;
                    RCList->push_back(robComm);
            }
        }

        if (distanceFromRobToDestinationY < 0){
            for (int i = 0; i > distanceFromRobToDestinationY; i--){

                    if (i == distanceFromRobToDestinationY + 1 && (leg_status == ON_SECOND_LEG  || leg_status == NO_SECOND_LEG)){
                        pair<Moves, Direction>* robComm = new pair<Moves, Direction>();
                        robComm->second = SOUTH;
                        robComm->first = MOVE;
                        RCList->push_back(robComm);
                        break;
                    }                
                    pair<Moves, Direction>* robComm= new pair<Moves, Direction>();
                    robComm->second = SOUTH;
                    robComm->first = argmove;
                    RCList->push_back(robComm);
            }
        }

        if (distanceFromRobToDestinationY == 0){
            cout << "no distance to travel Y axis" << endl;
        }
    }
};
