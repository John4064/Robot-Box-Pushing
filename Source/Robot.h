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
        static pthread_mutex_t mutex;
        static RThread* RTinfo;
        pthread_t TID;
        static vector<vector<pair<Moves, Direction>>> commandsListHolder;
        vector<pair<Moves, Direction>> thisRobotsMoves;
        uint idx_of_robot;  
        void genRobotsCommandsList(RThread* RTinfo);
        void printARobotsCommandList();
        void robotMakeMoves();
    } RThread;

    void * robotThreadFunc(void * arg);

    tuple<int, int, startPushAxis> determineStartingPushPositionAxis(RThread * RTinfo);


    vector<pair<Moves, Direction> > genCommGetBehindBox(RThread* RTinfo, tuple <int, int, startPushAxis>& startingPushPositionAxis);

    vector<pair<Moves, Direction> > genCommPushBoxtoDoor(RThread* RTinfo);



    vector<pair<Moves, Direction> > recordMovesPushToDoor(RThread* RTinfo, tuple <int, int ,startPushAxis> startingPushPositionAxis);
    
    string convertMoveEnumToWord(Moves move);
    string convertDirEnumToWord(Direction dir);

    void printRobotsCommandsList(RThread* RTInfo);
    void destroyRobotsCommandsList(RThread* RTinfo);
    vector<pair<Moves, Direction> > recordMovesToBehindBox(tuple <int, int, startPushAxis> startingPushPosition, RThread* RTinfo);
    
    void recordMovesX(vector<pair<Moves, Direction>>& RCList, pair<int, int> startingPoint, pair<int, int> destination, Moves argmove, bool pushCase);
    void recordMovesY(vector<pair<Moves, Direction>>& RCList, pair<int, int> startingPoint, pair<int, int> destination, Moves argmove, bool pushCase);
    void recordMovesToSecondPushPosition(vector<pair<Moves, Direction> >& RCList, tuple <int, int, startPushAxis> 
        startingPushPositionAxis, int idx);
    // bool collisionWithBoxAvoider(tuple <int, int, bool> targetStartingPushPositionAxis, int idx, NeedToGoAround& goAround);
    void makeRegMove(Direction dir, int idx);
    void makePushMove(Direction dir, int idx);

};

#endif