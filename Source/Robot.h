#include <pthread.h>
#include <cstdlib>
#include <vector>
#include <utility>
#include <string>
#include <deque>

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

    typedef enum NeedToGoAround{

        NO,
        YES_X,
        YES_Y

    } NeedToGoAround;

    tuple<int, int, bool> determineStartingPushPositionAxis(RThread * RTinfo);

    typedef deque<RobotCommand*> RobotCommandsList;
    
    extern vector<RobotCommandsList*> RobotCLs;

    RobotCommandsList* genCommGetBehindBox(RThread* RTinfo);

    RobotCommandsList* genCommPushBoxtoDoor(RThread* RTinfo);

    RobotCommandsList* genRobotsCommandsList(RThread* RTinfo);

    RobotCommandsList* recordMovesFirstLegPush(RThread*RTinfo, tuple<int, int, bool> startingPushPosition);
    RobotCommandsList* recordMovesFirstLeg(RThread* RTinfo, tuple <int, int ,bool> startingPushPositionAxis);



    }

    void fprintRobotsCommandsList(RThread* RTInfo);
    void destroyRobotsCommandsList(RThread* RTinfo);
    RobotCommandsList* recordMovesToBehindBox(tuple <int, int, bool> startingPushPosition, RThread* RTinfo);
    
    void printRobotsCommandsList(RobotCommandsList* RCL);
    void recordMovesX(RobotCommandsList* RCList, tuple <int, int, bool> startingPushPositionAxis, int idx);
    void recordMovesY(RobotCommandsList* RCList, tuple <int, int, bool> startingPushPositionAxis, int idx);
    // bool collisionWithBoxAvoider(tuple <int, int, bool> targetStartingPushPositionAxis, int idx, NeedToGoAround& goAround);
    void makeRegMove(Direction dir, int idx);
    void makePushMove(Direction dir, int idx);
    
};

#endif