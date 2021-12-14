#include <pthread.h>
#include <cstdlib>
#include <vector>
#include <utility>
#include <string>
#include <queue>

#ifndef ROBOT_H
#define ROBOT_H

using namespace std;

typedef unsigned int uint;

namespace Robot{

    typedef struct RThread {
        pthread_t TID;
        uint index;  
    } RThread;

     void* robotThreadFunc(void * arg);


    typedef enum Direction {
								NORTH = 0,
								WEST = 1,
								EAST = 3,
								SOUTH = 4,
								NUM_TRAVEL_DIRECTIONS = 4,
                                NOMOVEMENT = -1
    } Direction;


    typedef enum Moves {
                                PUSH,
                                MOVE, 
                                END 
    } Moves;

    typedef struct RobotCommand{
        Moves move;
        Direction direction;
    } RobotCommand;

    tuple<int, int, bool> determineStartingPushPositionAxis(RThread * RTinfo);

    typedef vector<RobotCommand*> RobotCommandsList;
    
    extern vector<RobotCommandsList> RobotsCL;

    RobotCommandsList* genCommGetBehindBox(RThread* RTinfo);

    RobotCommandsList* genCommPushBoxtoDoor(RThread* RTinfo);

    RobotCommandsList* genRobotsCommandsList(RThread* RTinfo);

    void fprintRobotsCommandsList(RThread* RTInfo);
    void destroyRobotsCommandsList(RThread* RTinfo);
    RobotCommandsList* recordMovesToBehindBox(tuple <int, int, bool> targetStartingPushPosition, RThread* RTinfo);
    
    void printRobotsCommandsList(RobotCommandsList* RCL);
    
};

#endif