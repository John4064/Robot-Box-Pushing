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

   
    /**
     * @brief An enum to define directions as used in the program
     * 
     */
    typedef enum Direction {
								NORTH = 0,
								WEST = 1,
								EAST = 3,
								SOUTH = 4,
                                NOMOVEMENT = -1
    } Direction;

    /**
     * @brief An enum to define MOVES as used in the program
     * 
     */
    typedef enum Moves {
                                PUSH,
                                MOVE, 
                                END 
    } Moves;

    /**
     * @brief An enum to define the concept of AXIS as used in the program.
     *        Axes are used as parameters in the robot path planning functions 
     */
    typedef enum axis{
                                VERTICAL,
                                HORIZONTAL
    } axis;

    /**
     * @brief An enum to define the concept of LEG STATUS in the program.
     *        A robot's total path is divided into the first half of the trip where
     *        it MOVES and the second half where it PUSHES the box.  THe PUSH half is further
     *        divided into two halves, the part in which the robot is pushing to the first "dog leg"
     *        or right angle waypoint, and the second half where it is simply pushing in a straight line
     *        to the door.  The variable ON_MOVE_OR_FIRST_LEG_PUSH describes just that, and ON_SECOND_LEG
     *        describes the final push to the door.  Sometimes, A robot will already be in the push position 
     *        to go straight to the door and will have NO_SECOND_LEG.
     */
    typedef enum legStatus{
                                ON_MOVE_OR_FIRST_LEG_PUSH,
                                ON_SECOND_LEG,
                                NO_SECOND_LEG
    } legStatus;


    /**
     * @brief A single structure that holds all of the important information used by the threads
     * 
     */
    typedef struct RThread {
        
        /** @brief a 2d vector that holds the commands for all robots */
        static vector<vector<pair<Moves, Direction>*>*> commandsListHolder;

        /** @brief a variable that tells if the thread has not yet been joined */
        bool stillAlive;
        /** @brief a mutex used in GUI initialization process */
        static pthread_mutex_t mutex; 
        /** @brief the mutex used for writing to the file */
        static pthread_mutex_t file_mutex;
        /** @brief the mutex used for writing to the robotLoc and boxLoc position vectors */
        static vector<pthread_mutex_t*> robotLocWritingMutexVec;
        /** @brief a mutex used for protecting the reader count vectors for the reader writers solution pattern */
        static vector<pthread_mutex_t*> robotLocProtectReaderCountMutexVec;
        /** @brief a vector of mutexes that locks each position in the display grid when a robot wants to move there */
        static vector<vector<pthread_mutex_t*>*> gridMutexVector;
        /** @brief a vector of unsigned ints that counts how many readers there currently are.  while there are more than one
         * the mutex is locked against being written on
         */
        static vector<uint> robotLocReaderCountVec;
        /** @brief a pointer to the main thread structs array.. this is generally not useed by the threads themselves, but we
         * just put it here...
         */
        static RThread* RTinfo;
        /** @brief obviously just the thread ID data member */
        pthread_t TID;
        vector<pair<Moves, Direction>*>* thisRobotsMoves;
        /** @brief this a copy of the robot moves vector that can be used to save each robot's planned command list if you wanted to 
         * print it after the robot has made some of the moves.  We don't use it in the multithreaded versions.
          */
        vector<pair<Moves, Direction>*> copy_of_robot_moves;
        /** @brief each robot has an index number which is used in the threads to write to and pull info out of the global arrays*/
        uint idx_of_robot;  
        /** @brief the top-level function for the robot path planning*/
        void genRobotsCommandsList(RThread* RTinfo);
        /** @brief this is not used in the mutlithreaded versions, but can be used to print the robot's planned list*/
        void printARobotsCommandList();
        /** @brief a function used for the making moves logic.  it tells where any MOVE move will bring us to after we make it*/
        pair<uint, uint> determineLocCommBringsUsToPUSH(Direction command);
        /** @brief a function used for the making moves logic.  it tells where any PUSH move will bring us to after we make it*/
        pair<uint, uint> determineLocCommBringsUsToMOVE(Direction command);
        /** @brief this function is used in the making moves logic.  it tells whether a potential move already exists in the global 
                    position arrays*/
        bool checkLocAlreadyExists(pair<uint, uint> locMovingTo, bool isReader);
         /** @brief this function is used in robot path planning.  it determines the an appropriate original starting push position 
          * for the robot and also records the "axis" down which the robot will begin the PUSH portion of its path*/
        tuple<int, int, axis> determineStartingPushPositionAxis(RThread * RTinfo);
        /** @brief this thread-safe function is used in the multithreaded version to print/record each move by each robot as it is done to file.*/
        void fprintRobotMove(Moves move, Direction direction);
        /** @brief top-level routine that runs each multithreaded robot through its making moves process and printing routines.*/
        void robotMakeMovesnPrint();
        /** @brief path-planning function uses determineStartingPushPositionAxis to generate list of moves to get behind box in PUSH position.*/
        vector<pair<Moves, Direction>* >* genCommGetBehindBox(RThread* RTinfo, tuple <int, int, axis>& startingPushPositionAxis);
        /** @brief function that determines which initial path to (dog-leg and) initial push position (Y-first or X-first) should be used to prevent a 
         * collision with the box .*/
        bool recordYfirst(tuple <int, int, axis> startingPushPositionAxis);
        /** @brief function that determines the correct moves to get behind the box .*/
        vector<pair<Moves, Direction>*>* recordMovesToBehindBox(tuple <int, int, axis> startingPushPositionAxis, RThread* RTinfo);
        void freeThreadMemory();
    } RThread;

    /**  @brief a thread function that directs the single thread that collects all the other threads in a JOIN operation after the threads are complete. */
    void* joinThreads(void*);
     /** @brief a function that allocates and calls "init" on the mutexes used by the program. */
    void initializeMutexes();
    /**  @brief the function that is run by each robot thread. */
    void * robotThreadFunc(void * arg);
    /** @brief  a function that prints the required information for printing to file not unique to each thread */
    void printBeginningPartOfOutputFile();
    /**  @brief a function that determines random locations for robots and adds them to the robot location vector */
    void placeRobots();


    /** @brief  a path-planning function that generates a list of push commmands to get from the box to the door */
    vector<pair<Moves, Direction> > genCommPushBoxtoDoor(RThread* RTinfo);
    /** @brief  a lower-level path-planning function that generates a list of push commmands to get from the box to the door */ 
    vector<pair<Moves, Direction>* >* recordMovesPushToDoor(RThread* RTinfo, tuple <int, int ,axis> startingPushPositionAxis);
    
    /** @brief  used in printing and std output of command lists */
    string convertMoveEnumToWord(Moves move);
    /** @brief  used in printing and std output of command lists */
    string convertDirEnumToWord(Direction dir);

    /** @brief  used in printing the robots commands copy of planned commands */
    void printRobotsCommandsList(RThread* RTInfo);
    
    /** @brief low level path-planning functions that plan a path from a starting point to a finish point modulated by a couple of variables .*/
    void recordMovesX(vector<pair<Moves, Direction>*>* RCList, pair<int, int> startingPoint, pair<int, int> destination, Moves argmove, legStatus leg_status);
    void recordMovesY(vector<pair<Moves, Direction>*>* RCList, pair<int, int> startingPoint, pair<int, int> destination, Moves argmove, legStatus leg_status);
   /** @brief function to plan the rotation of robot about its initial push position.*/
    pair<uint, uint> recordRotationToSecondPushPos(vector<pair<Moves, Direction> *>* RCList, tuple <int, int, axis> 
        startingPushPositionAxis, int idx);
    /** @brief utility to compute new coordinates when certain MOVES are made.*/
    void makeRegMove(Direction dir, int idx);
    /** @brief utility to compute new coordinates when certain PUSHES are made.*/
    void makePushMove(Direction dir, int idx);

};

#endif