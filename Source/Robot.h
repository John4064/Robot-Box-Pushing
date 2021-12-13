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

    typedef enum Direction {
								NORTH = 0,
								WEST = 1,
								EAST = 3,
								SOUTH = 4,
								NUM_TRAVEL_DIRECTIONS = 4
    } Direction;


    typedef enum Moves {
                                PUSH,
                                MOVE, 
                                END 
    } Moves;

    typedef pair<Moves, Direction> RobotCommand;

    typedef vector<RobotCommand> RobotCommandsList;

    typedef struct RThread {

        pthread_t TID;        

        vector<RobotCommandsList> CommandsforAllRobots;

    } RThread;

    void printRobotsCommandsList(RThread* RTInfo);
    

};

#endif