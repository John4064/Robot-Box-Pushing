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

    typedef enum startPushAxis{
                                VERTICAL,
                                HORIZONTAL
    } startPushAxis;


    typedef struct RThread {
<<<<<<< HEAD
        static vector<vector<pair<Moves, Direction>>> commandsListHolder;
=======
        static vector<vector<pair<Moves, Direction> >*> commandsListHolder;
>>>>>>> e042d7a8981031efc4ce2e8dbea29fa6ae536ca1
        pthread_t TID;
        uint index;  
    } RThread;

<<<<<<< HEAD
    void * robotThreadFunc(void * arg);
=======
    vector<pair<Moves, Direction> >  robotThreadFunc(void * arg);
>>>>>>> e042d7a8981031efc4ce2e8dbea29fa6ae536ca1



    // typedef enum NeedToGoAround{

    //     NO,
    //     YES_X,
    //     YES_Y

    // } NeedToGoAround;

    tuple<int, int, startPushAxis> determineStartingPushPositionAxis(RThread * RTinfo);


    vector<pair<Moves, Direction> > genCommGetBehindBox(RThread* RTinfo, tuple <int, int, startPushAxis>& startingPushPositionAxis);

    vector<pair<Moves, Direction> > genCommPushBoxtoDoor(RThread* RTinfo);

<<<<<<< HEAD
    void genRobotsCommandsList(RThread* RTinfo);
=======
    vector<pair<Moves, Direction> > genRobotsCommandsList(RThread* RTinfo);
>>>>>>> e042d7a8981031efc4ce2e8dbea29fa6ae536ca1

    vector<pair<Moves, Direction> > recordMovesPushToDoor(RThread* RTinfo, tuple <int, int ,startPushAxis> startingPushPositionAxis);



    string convertMoveEnumToWord(Moves move);
    string convertDirEnumToWord(Direction dir);

    void fprintRobotsCommandsList(RThread* RTInfo);
    void destroyRobotsCommandsList(RThread* RTinfo);
    vector<pair<Moves, Direction> > recordMovesToBehindBox(tuple <int, int, startPushAxis> startingPushPosition, RThread* RTinfo);
    
    void recordMovesX(vector<pair<Moves, Direction> >& RCList, pair<int, int> startinPoint, pair<int, int> destination, Moves move);
    void recordMovesY(vector<pair<Moves, Direction> >& RCList,  pair<int, int> startinPoint, pair<int, int> destination, Moves move);
    void recordMovesToSecondPushPosition(vector<pair<Moves, Direction> >& RCList, tuple <int, int, startPushAxis> 
        startingPushPositionAxis, int idx);
    // bool collisionWithBoxAvoider(tuple <int, int, bool> targetStartingPushPositionAxis, int idx, NeedToGoAround& goAround);
    void makeRegMove(Direction dir, int idx);
    void makePushMove(Direction dir, int idx);
    
};

#endif