
//
//  main.cpp
//  Final Project CSC412
// John Parkhurst and David Shawver
//
//  Created by Jean-Yves Hervé on 2018-12-05, Rev. 2021-12-01
//	This is public domain code.  By all means appropriate it and change is to your
//	heart's content.


/**
 * @file main.cpp
 * @author David Shawver and John Parkhurst
 * @brief 
 * 
 * 
 * Notes:
 * 
 * The critical sections of this program are as follows:
 * 
 * 	Writing:
 *		(1) When writing to file
 * 		(2) When writing the commands to robot or box array, 
 * 			values are incremented (i.e. written)
 * 		(3) If we write to standard output we should also be using locks
 * 	
 *  Reading:
 * 		(1) When a thread checks if the next cell (the robot when moving or
 * 			the box when pushing) that will be visited is already occupied,
 * 			it reads the robotLoc and boxLoc arrays.
 *
 * 		(2) In gridDisplayPane() when the GUI reads the values of the array
 * 
 * 		For Reading number 1, we are going to try to implement reader-writers solution.
 * 		
 * 			
 * 
 * @version 0.1
 * @date 2021-12-18
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#include <string.h>			// for strerror()
#include <string>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <assert.h>
#include <algorithm>
#include <vector>
#include "Robot.h"
#include <random>
#include <unistd.h>
#include <fstream>

//
#include "guiChoice.h"
#if CSC412_FP_USE_GUI
	#include "gl_frontEnd.h"
#endif

using namespace std;

//==================================================================================
  //	Function prototypes
//==================================================================================
#if CSC412_FP_USE_GUI
	void displayGridPane(void);
	void displayStatePane(void);
#endif
void initializeApplication();
void robotRandomPlacement(uniform_int_distribution<int> rowDist, uniform_int_distribution<int> colDist,
 default_random_engine myEngine);
void doorRandomPlacement(uniform_int_distribution<int> rowDist, uniform_int_distribution<int> colDist,
 default_random_engine myEngine);
void boxRandomPlacement(default_random_engine myEngine);
void assignDoors(uniform_int_distribution<int> randDoorDist, default_random_engine myEngine);

template <typename T>
void printVector(T vec);

// Utility functions
void printObjectPlacements();
bool checkIfPairExists(pair<uint, uint> randPair, vector<pair<uint, uint>*> vec);
bool checkIfNumExistsInVec(uint unum, vector<uint> vec);

//==================================================================================
//	Application-level global variables
//==================================================================================

//	Don't touch
#if CSC412_FP_USE_GUI
	extern int	GRID_PANE, STATE_PANE;
	extern int	gMainWindow, gSubwindow[2];
	extern int GRID_PANE_WIDTH, GRID_PANE_HEIGHT;
	extern int STATE_PANE_WIDTH, STATE_PANE_HEIGHT;
#endif


//	Don't rename any of these variables
//-------------------------------------
//	The state grid and its dimensions (arguments to the program)
uint** grid;
uint numRows = -1;	//	height of the grid
uint numCols = -1;	//	width
uint numBoxes = -1;	//	also the number of robots
uint numDoors = -1;	//	The number of doors.
uint& numRobots = numBoxes;

int numLiveThreads = 0;		//	the number of live robot threads

//	robot sleep time between moves (in microseconds)
const int MIN_SLEEP_TIME = 1000;
int robotSleepTime = 500000;

//	An array of C-string where you can store things you want displayed
//	in the state pane to display (for debugging purposes?)
//	Dont change the dimensions as this may break the front end
const int MAX_NUM_MESSAGES = 8;
const int MAX_LENGTH_MESSAGE = 32;
char** message;


//-----------------------------
//	Global Vectors
//-----------------------------

vector<pair<uint, uint>*> boxLoc;
vector<uint> doorAssign;
vector<pair<uint, uint>*> doorLoc;

namespace Robot{
	vector<pair<uint, uint>*> robotLoc;
	pthread_mutex_t RThread::mutex;
	pthread_mutex_t RThread::file_mutex;
	extern Robot::RThread* RThread::RTinfo;
	extern vector<vector<pair<Moves, Direction>>> RThread::commandsListHolder;
	vector<pthread_mutex_t*> RThread::boxLocReadingMutexVec;
    vector<pthread_mutex_t*> RThread::boxLocWritingMutexVec;
    vector<pthread_mutex_t*> RThread::robotLocWritingMutexVec;
    vector<pthread_mutex_t*> RThread::robotLocReadingMutexVec;
};


//==================================================================================
//	These are the functions that tie the simulation with the rendering.
//	Some parts are "don't touch."  Other parts need your intervention
//	to make sure that access to critical section is properly synchronized
//==================================================================================

#if CSC412_FP_USE_GUI

void displayGridPane(void)
{
	//Do one move here...
	//	This is OpenGL/glut magic.  Don't touch
	//---------------------------------------------
	glutSetWindow(gSubwindow[GRID_PANE]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// move to top of the pane
	glTranslatef(0.f, GRID_PANE_WIDTH, 0.f);
	// flip the vertiucal axis pointing down, in regular "grid" orientation
	glScalef(1.f, -1.f, 1.f);


	for (uint i=0; i<numBoxes; i++)
	{	//	here I would test if the robot thread is still live
			pthread_mutex_lock(&Robot::RThread::mutex);
			drawRobotAndBox(i,Robot::robotLoc[i]->first, Robot::robotLoc[i]->second, 
			boxLoc[i]->first, boxLoc[i]->second, doorAssign[i]);
			pthread_mutex_unlock(&Robot::RThread::mutex);
	}

	for (uint i=0; i<numDoors; i++)

	{
		drawDoor(i, doorLoc[i]->first, doorLoc[i]->second);
	}

	//	This call does nothing important. It only draws lines
	//	There is nothing to synchronize here
	drawGrid();

	


	//	This is OpenGL/glut magic.  Don't touch
	glutSwapBuffers();
	
	glutSetWindow(gMainWindow);
}

void displayStatePane(void)
{
	//	This is OpenGL/glut magic.  Don't touch
	glutSetWindow(gSubwindow[STATE_PANE]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//	Here I hard-code a few messages that I want to see displayed
	//	in my state pane.  The number of live robot threads will
	//	always get displayed.  No need to pass a message about it.
	int numMessages = 3;
	sprintf(message[0], "We have %d doors", numDoors);
	sprintf(message[1], "I like cheese");
	sprintf(message[2], "System time is %ld", time(NULL));
	
	//---------------------------------------------------------
	//	This is the call that makes OpenGL render information
	//	about the state of the simulation.
	//
	//	You *must* synchronize this call.
	//
	//---------------------------------------------------------
	drawState(numMessages, message);
	
	
	//	This is OpenGL/glut magic.  Don't touch
	glutSwapBuffers();
	
	glutSetWindow(gMainWindow);
}

#endif // CSC412_FP_USE_GUI

//------------------------------------------------------------------------
//	You shouldn't have to touch this one.  Definitely if you don't
//	add the "producer" threads, and probably not even if you do.
//------------------------------------------------------------------------
void speedupRobots(void)
{
	//	decrease sleep time by 20%, but don't get too small
	int newSleepTime = (8 * robotSleepTime) / 10;
	
	if (newSleepTime > MIN_SLEEP_TIME)
	{
		robotSleepTime = newSleepTime;
	}
}

void slowdownRobots(void)
{
	//	increase sleep time by 20%
	robotSleepTime = (12 * robotSleepTime) / 10;
}
void inputCheck(int argc,char** argv){
	/** @param: length of argv string array array of the inputs
	 * @brief: Will check the inputs are valid inputs
	 * @return: nothing
	 */
	
	for(int x = 1; x < argc;x++ ){
		if(!isdigit(*argv[x])){
			exit(43);
		}
	}
	//If it survives the loop inputs are all integers
}


//------------------------------------------------------------------------
//	You shouldn't have to change anything in the main function besides
//	the initialization of numRows, numCos, numDoors, numBoxes.
//------------------------------------------------------------------------
int main(int argc, char** argv)
{
	//	We know that the arguments  of the program  are going
	//	to be the width (number of columns) and height (number of rows) of the
	//	grid, the number of boxes (and robots), and the number of doors.
	//	You are going to have to extract these.  For the time being,
	//	I hard code-some values
	//WARNING ROBOTS = BOXES or Seg fault
	//ERROR HANDLING
	inputCheck(argc,argv);
	numRows = atoi(argv[1]);
	numCols = atoi(argv[2]);
	numBoxes = atoi(argv[3]);
	numDoors = atoi(argv[4]);
	if(numDoors>3 || numDoors <1){
		exit(44);
	}


	// abort program if these values do not match
	assert (numBoxes == numRobots);


#if CSC412_FP_USE_GUI
	//	Even though we extracted the relevant information from the argument
	//	list, I still need to pass argc and argv to the front-end init
	//	function because that function passes them to glutInit, the required call
	//	to the initialization of the glut library.
	initializeFrontEnd(argc, argv, displayGridPane, displayStatePane);
#endif
	
	//	Now we can do application-level initialization
	initializeApplication();

#if CSC412_FP_USE_GUI
	//	Now we enter the main loop of the program and to a large extend
	//	"lose control" over its execution.  The callback functions that 
	//	we set up earlier will be called when the corresponding event
	//	occurs

	// signal to worker threads that GUI starting now

	pthread_mutex_unlock(&Robot::RThread::mutex);

	glutMainLoop();

	for (int i=0; i < numRobots; i++){

		pthread_join(Robot::RThread::RTinfo[i].TID, NULL);

	}

#endif
	
	//	Free allocated resource before leaving (not absolutely needed, but
	//	just nicer.  Also, if you crash there, you know something is wrong
	//	in your code.
	for (uint i=0; i< numRows; i++)
		delete []grid[i];
	delete []grid;
	for (int k=0; k<MAX_NUM_MESSAGES; k++)
		free(message[k]);
	free(message);

	
	//	This will probably never be executed (the exit point will be in one of the
	//	call back functions).
	return 0;
}


//==================================================================================
//
//	This is a part that you have to edit and add to.
//
//===================================	===============================================
void Robot::printBeginningPartOfOutputFile(){

	ofstream myfile;
	myfile.open("robotSimulOut.txt");

	myfile << "num of rows = " << numRows << "; num of cols = " << numCols << "; num of boxes = " 
	<< numBoxes << "; num of doors = " << numDoors << endl;

 	myfile << "\n";

	myfile << "Door locations:" << endl;
	for(uint i=0; i < doorLoc.size(); i++){
		myfile <<"\t"<< doorLoc[i]->first << ", "<<doorLoc[i]->second<< endl;
	}

	myfile << endl;
	myfile << "Box locations:" << endl;
	for(uint i=0; i < boxLoc.size(); i++){
		myfile <<"\t"<< boxLoc[i]->first << ", "<<boxLoc[i]->second<< " location of dest door = " << 
		doorLoc[doorAssign[i]]->first <<", " << doorLoc[doorAssign[i]]->second << endl;
	}

	myfile << endl;
	myfile << "Robot locations:" << endl;
	for(uint i=0; i < Robot::robotLoc.size(); i++){
		myfile <<"\t"<< Robot::robotLoc[i]->first << ", "<<Robot::robotLoc[i]->second<< endl;
	}

	myfile << "\n";

}

void Robot::RThread::initializeMutexes(){
	
	// an all purpose mutex used as a mutex gate when initializing gui 
	// and for reading data in single-threaded version
	if (pthread_mutex_init(&RThread::mutex, NULL) != 0) {                                    
		perror("mutex_lock");                                                       
		exit(1);                                                                    
	}    

	// a mutex to be used for opening and writing the robot commands to file
	if (pthread_mutex_init(&RThread::file_mutex, NULL) != 0) {                                    
		perror("mutex_lock");                                                       
		exit(1);                                                                    
	}   
	
	// declare and initialize two mutex arrays for each box loc
	// robot loc arrays ... one for reading one for writing

	for (int j = 0; j < boxLoc.size(); j++){
		pthread_mutex_t *mutexP = new pthread_mutex_t();
		if(pthread_mutex_init(mutexP, NULL) != 0){
			perror("mutex_lock");                                                       
			exit(1);   
		}
		boxLocReadingMutexVec.push_back(mutexP);
	}

	for (int j = 0; j < boxLoc.size(); j++){
		pthread_mutex_t *mutexP = new pthread_mutex_t();
		if(pthread_mutex_init(mutexP, NULL) != 0){
			perror("mutex_lock");                                                       
			exit(1);   
		}
		boxLocWritingMutexVec.push_back(mutexP);
	}

	for (int j = 0; j < robotLoc.size(); j++){
		pthread_mutex_t *mutexP = new pthread_mutex_t();
		if(pthread_mutex_init(mutexP, NULL) != 0){
			perror("mutex_lock");                                                       
			exit(1);   
		}
		robotLocReadingMutexVec.push_back(mutexP);
	}
		for (int j = 0; j < robotLoc.size(); j++){
		pthread_mutex_t *mutexP = new pthread_mutex_t();
		if(pthread_mutex_init(mutexP, NULL) != 0){
			perror("mutex_lock");                                                       
			exit(1);   
		}
		robotLocWritingMutexVec.push_back(mutexP);
	}
}

void Robot::placeRobots(){
	random_device myRandDev;
	default_random_engine myEngine(myRandDev());
	uniform_int_distribution<int> robotRowDist(0, numRows - 1);
	uniform_int_distribution<int> robotColDist(0, numCols - 1);

	robotRandomPlacement(robotRowDist, robotColDist, myEngine);
	doorRandomPlacement(robotRowDist, robotColDist, myEngine);
	boxRandomPlacement(myEngine);
	uniform_int_distribution<int> randDoorDist(0, doorLoc.size() - 1);
	assignDoors(randDoorDist, myEngine);
}

void initializeApplication(){

	{using namespace Robot;
		
	RThread::initializeMutexes();


	//printObjectPlacements();
	placeRobots();


	RThread::RTinfo = new RThread[numRobots];

	// the main thread's lock, which blocks the worker threads from proceeding
	// until unlock called after main gui loop entered.
	pthread_mutex_lock(&RThread::mutex);


	for (uint i =0; i < numRobots; i++){
		(RThread::RTinfo+i)->idx_of_robot = i;
		printBeginningPartOfOutputFile();
		int errCode = pthread_create(&RThread::RTinfo->TID, NULL, robotThreadFunc, RThread::RTinfo+i);
		if (errCode != 0){
			printf ("could not pthread_create thread %d. %d/%s\n",
			i, errCode, strerror(errCode));
			exit (EXIT_FAILURE);
		}
	}


	grid = new uint*[numRows];
	for (uint i=0; i<numRows; i++)
		grid[i] = new uint [numCols];
	
	message = (char**) malloc(MAX_NUM_MESSAGES*sizeof(char*));
	for (uint k=0; k<MAX_NUM_MESSAGES; k++)
		message[k] = (char*) malloc((MAX_LENGTH_MESSAGE+1)*sizeof(char));
	}

}



void boxRandomPlacement(default_random_engine myEngine){
	uniform_int_distribution<int> robotRowDist(1, numRows - 2);
	uniform_int_distribution<int> robotColDist(1, numCols - 2);
	
	for(uint i = 0; i < numBoxes; i++){
		while(true){
			uint boxRow = robotRowDist(myEngine);
			uint boxCol = robotColDist(myEngine);
			pair<uint, uint> proposedPair = make_pair(boxRow, boxCol);
			if(!checkIfPairExists(proposedPair, boxLoc) && !checkIfPairExists(proposedPair, Robot::robotLoc)){
				boxLoc.push_back(new pair<uint, uint>(boxRow, boxCol));
				break;
			}
		}
	}	
}


void robotRandomPlacement(uniform_int_distribution<int> robotRowDist, 
uniform_int_distribution<int> robotColDist, default_random_engine myEngine){
	for(uint i = 0; i < numRobots; i++){
		while(true){
			uint robotRow = robotRowDist(myEngine);
			uint robotCol = robotColDist(myEngine);
			pair<uint, uint> proposedPair = make_pair(robotRow, robotCol);
			if(!checkIfPairExists(proposedPair, Robot::robotLoc) && !checkIfPairExists(proposedPair, boxLoc)){
				Robot::robotLoc.push_back(new pair<uint, uint>(robotRow, robotCol));
				break;
			}
		}
	}
}

void doorRandomPlacement(uniform_int_distribution<int> robotRowDist, 
uniform_int_distribution<int> robotColDist, default_random_engine myEngine){
	for(uint i = 0; i < numDoors; i++){
		while(true){
			uint doorRow = robotRowDist(myEngine);
			uint doorCol = robotColDist(myEngine);
			pair<uint, uint> proposedPair = make_pair(doorRow, doorCol);
			if(!checkIfPairExists(proposedPair, Robot::robotLoc) && !checkIfPairExists(proposedPair, boxLoc)
				&& !checkIfPairExists(proposedPair, doorLoc)){
				doorLoc.push_back(new pair<uint, uint>(doorRow, doorCol));
				break;
			}
		}
	}
}

void printObjectPlacements(){
	cout << "Door locations:" << endl;
	for(uint i=0; i < doorLoc.size(); i++){
		cout <<"\t"<< doorLoc[i]->first << ", "<<doorLoc[i]->second<< endl;
	}
	cout << endl;
	cout << "Robot locations:" << endl;
	for(uint i=0; i < Robot::robotLoc.size(); i++){
		cout <<"\t"<< Robot::robotLoc[i]->first << ", "<<Robot::robotLoc[i]->second<< endl;
	}
	cout << endl;
	cout << "Box locations:" << endl;
	for(uint i=0; i < boxLoc.size(); i++){
		cout <<"\t"<< boxLoc[i]->first << ", "<<boxLoc[i]->second<< endl;
	}
	cout << endl;
}

bool checkIfPairExists(pair<uint, uint> randPair, vector<pair<uint, uint>*> vec){
	for(auto it = vec.begin(); it!=vec.end(); it++){ 
		if((*it)->first == randPair.first && (*it)->second == randPair.second){ 
			return true; 
		}
	}
	return false;
}

bool checkIfNumExistsInVec(uint num, vector<uint> vec){
	bool result = false;
    if( find(vec.begin(), vec.end(), num) != vec.end() )
    {
        result = true;
    }
    return result;
}



void assignDoors(uniform_int_distribution<int> randDoorDist, default_random_engine myEngine){

		for (uint i=0; i < Robot::robotLoc.size() && i < doorLoc.size(); i++){
			while (true){
			uint randomDoor = randDoorDist(myEngine);
			if (!checkIfNumExistsInVec(randomDoor, doorAssign)){
				doorAssign.push_back(randomDoor);
				break;
			}	
		}
	}

	for (uint i=doorLoc.size(); i < Robot::robotLoc.size(); i++){
		while(true){
		uint randomDoor = randDoorDist(myEngine);
			doorAssign.push_back(randomDoor);
			break;
		}
	}	
	cout << "assigned doors:\n";
	printVector<vector<uint> >(doorAssign);
}


template <typename T>
void printVector(T vec)
{
	for(uint i=0; i < vec.size(); i++){
		cout << vec[i] << " ";
	}
	cout << endl;
}

void printRobotsCommandsList(){

	cout << "Printing RCL List: \n"<< endl;
	cout << "\tMOVE:\tDirection:" << endl;

	// printing to file

		cout << "command list holder size = " << Robot::RThread::commandsListHolder.size() << endl;

        for (uint j = 0; j < Robot::RThread::commandsListHolder.size(); j++){
            cout << "List " << j << endl;
           for (uint i= 0; i < Robot::RThread::commandsListHolder[j].size(); i++) {
            cout << "'i' = " << i << endl;
            string moveString = convertMoveEnumToWord(Robot::RThread::commandsListHolder[j][i].first);
			string directionString = convertDirEnumToWord(Robot::RThread::commandsListHolder[j][i].second);
	
            cout <<"\t" << moveString << "\t" << directionString << endl;
                     fflush(stdout);
            }
        }

}
