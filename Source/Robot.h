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


    typedef struct RThread {
        static vector<vector<pair<Moves, Direction>>*> commandsListHolder;
        pthread_t TID;
        uint index;  
    } RThread;

     void* robotThreadFunc(void * arg);



    typedef enum NeedToGoAround{

        NO,
        YES_X,
        YES_Y

    } NeedToGoAround;

    tuple<int, int, bool> determineStartingPushPositionAxis(RThread * RTinfo);


    vector<pair<Moves, Direction>> genCommGetBehindBox(RThread* RTinfo, tuple <int, int, bool>& startingPushPositionAxis);

    vector<pair<Moves, Direction>> genCommPushBoxtoDoor(RThread* RTinfo);

    vector<pair<Moves, Direction>> genRobotsCommandsList(RThread* RTinfo);

    vector<pair<Moves, Direction>> recordMovesFirstLeg(RThread* RTinfo, tuple <int, int ,bool> startingPushPositionAxis);

    string convertMoveEnumToWord(Moves move);
    string convertDirEnumToWord(Direction dir);

    void fprintRobotsCommandsList(RThread* RTInfo);
    void destroyRobotsCommandsList(RThread* RTinfo);
    vector<pair<Moves, Direction>> recordMovesToBehindBox(tuple <int, int, bool> startingPushPosition, RThread* RTinfo);
    
    void recordMovesX(vector<pair<Moves, Direction>>& RCList, pair<int, int> startinPoint, pair<int, int> destination, Moves move);
    void recordMovesY(vector<pair<Moves, Direction>>& RCList,  pair<int, int> startinPoint, pair<int, int> destination, Moves move);
    // bool collisionWithBoxAvoider(tuple <int, int, bool> targetStartingPushPositionAxis, int idx, NeedToGoAround& goAround);
    void makeRegMove(Direction dir, int idx);
    void makePushMove(Direction dir, int idx);
    
};

#endif