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

    typedef enum axis{
                                VERTICAL,
                                HORIZONTAL
    } axis;

    typedef enum legStatus{
                                ON_MOVE_OR_FIRST_LEG_PUSH,
                                ON_SECOND_LEG,
                                NO_SECOND_LEG
    } legStatus;



    typedef struct RThread {
        static pthread_mutex_t mutex; 
        static pthread_mutex_t file_mutex;
        static vector<pthread_mutex_t*> robotLocWritingMutexVec;
        static vector<pthread_mutex_t*> robotLocProtectReaderCountMutexVec;
        static vector<vector<pthread_mutex_t*>*> gridMutexVector;
        static vector<uint> robotLocReaderCountVec;
        static RThread* RTinfo;
        pthread_t TID;
        static vector<vector<pair<Moves, Direction>>> commandsListHolder;
        vector<pair<Moves, Direction>> thisRobotsMoves;
        vector<pair<Moves, Direction>> copy_of_robot_moves;
        uint idx_of_robot;  
        void genRobotsCommandsList(RThread* RTinfo);
        void printARobotsCommandList();
        pair<uint, uint> determineLocCommBringsUsToPUSH(Direction command);
        pair<uint, uint> determineLocCommBringsUsToMOVE(Direction command);
        bool checkLocAlreadyExists(pair<uint, uint> locMovingTo, bool isReader);
        void fprintRobotMove(Moves move, Direction direction);
        void robotMakeMoves();

    } RThread;
    void initializeMutexes();
    void * robotThreadFunc(void * arg);

    tuple<int, int, axis> determineStartingPushPositionAxis(RThread * RTinfo);
    void printBeginningPartOfOutputFile();

    void placeRobots();

    vector<pair<Moves, Direction> > genCommGetBehindBox(RThread* RTinfo, tuple <int, int, axis>& startingPushPositionAxis);

    vector<pair<Moves, Direction> > genCommPushBoxtoDoor(RThread* RTinfo);

    vector<pair<Moves, Direction> > recordMovesPushToDoor(RThread* RTinfo, tuple <int, int ,axis> startingPushPositionAxis);
    
    string convertMoveEnumToWord(Moves move);
    string convertDirEnumToWord(Direction dir);

    void printRobotsCommandsList(RThread* RTInfo);
    void destroyRobotsCommandsList(RThread* RTinfo);
    vector<pair<Moves, Direction> > recordMovesToBehindBox(tuple <int, int, axis> startingPushPosition, RThread* RTinfo);
    
    void recordMovesX(vector<pair<Moves, Direction>>& RCList, pair<int, int> startingPoint, pair<int, int> destination, Moves argmove, legStatus leg_status);
    void recordMovesY(vector<pair<Moves, Direction>>& RCList, pair<int, int> startingPoint, pair<int, int> destination, Moves argmove, legStatus leg_status);
    pair<uint, uint> recordRotationToSecondPushPos(vector<pair<Moves, Direction> >& RCList, tuple <int, int, axis> 
        startingPushPositionAxis, int idx);
    // bool collisionWithBoxAvoider(tuple <int, int, bool> targetStartingPushPositionAxis, int idx, NeedToGoAround& goAround);
    void makeRegMove(Direction dir, int idx);
    void makePushMove(Direction dir, int idx);

};

#endif