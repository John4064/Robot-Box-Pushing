//
//  main.cpp
//  Final Project CSC412
// John Parkhurst and David Shawver
//
//  Created by Jean-Yves Hervé on 2018-12-05, Rev. 2021-12-01
//	This is public domain code.  By all means appropriate it and change is to your
//	heart's content.


/* Tasks:

	John:
		-- Add in error handling e.g., number of doors should not exceed 3.


*/
#include <string>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <assert.h>
#include <algorithm>
#include <vector>
#include "Robot.h"
#include <unistd.h>

//
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
void initializeApplication(void);

void robotRandomPlacement();
void doorRandomPlacement();
void boxRandomPlacement();
void assignDoors();

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
int robotSleepTime = 100000;

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
	vector<RobotCommandsList*> RobotCLs;
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

	for (uint i = 0; i < numRobots; i++){

		{using namespace Robot;

			cout << "made it here .... yay1" << endl;
			if(!RobotCLs[i]->empty()){
			RobotCommand* command = RobotCLs[i]->back();
			cout << "made it here .... yay2" << endl;
			Moves move = command->move;
			cout << "made it here .... yay3" << endl;
			Direction dir = command->direction;
			cout << "made it here .... yay4" << endl;
			if (move == MOVE){
				makeRegMove(dir, i);
			}
			if (move == PUSH){
				makePushMove(dir, i);
			}

				RobotCLs[i]->pop_back();
			}
			cout << "made it here .... yay7" << endl;
		}
		usleep(500000);
	}

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
		drawRobotAndBox(i,Robot::robotLoc[i]->first, Robot::robotLoc[i]->second, boxLoc[i]->first, boxLoc[i]->second, doorAssign[i]);
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



//------------------------------------------------------------------------
//	You shouldn't have to change anything in the main function besides
//	the initialization of numRows, numCos, numDoors, numBoxes.
//------------------------------------------------------------------------
int main(int argc, char** argv)
{
	numRows = atoi(argv[1]);
	numCols = atoi(argv[2]);
	numBoxes = atoi(argv[3]);
	numDoors = atoi(argv[4]);

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
	glutMainLoop();
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
//==================================================================================


void initializeApplication(void)
{
	//	seed the pseudo-random generator
	srand((unsigned int) time(NULL));

		robotRandomPlacement();
		doorRandomPlacement();
		boxRandomPlacement();
		printObjectPlacements();
		assignDoors();

		Robot::RThread* rtInfo= new Robot::RThread();

		for (int i =0; i < numRobots; i++){
			(rtInfo+i)->index = i;
			Robot::robotThreadFunc(rtInfo+i);
		}

	// How we represent directions and orientations with respect to these objects also seems very
	// important.

	grid = new uint*[numRows];
	for (uint i=0; i<numRows; i++)
		grid[i] = new uint [numCols];
	
	message = (char**) malloc(MAX_NUM_MESSAGES*sizeof(char*));
	for (uint k=0; k<MAX_NUM_MESSAGES; k++)
		message[k] = (char*) malloc((MAX_LENGTH_MESSAGE+1)*sizeof(char));
		
	//---------------------------------------------------------------
	//	All the code below to be replaced/removed
	//	I initialize the grid's pixels to have something to look at
	//---------------------------------------------------------------
	
	//	normally, here I would initialize the location of my doors, boxes,
	//	and robots, and create threads (not necessarily in that order).
	//	For the handout I have nothing to do.

cout << "made it all the way down here ... "<< endl;
}
	// a random location for each door;•  an initial position of each box on the grid 
		//(be careful that boxes should not be placed on theedges of the grid, that two boxes 
		// should not occupy the same position, and that a box shouldnot be created at the same
		//  location as a door),•  an initial position for each on the grid (be careful that a 
		// robot should not occupy the sameposition as a door, a box, or as another robot);•  
		// randomly assign a door as destination for a robot-box pair.

void boxRandomPlacement(){
	for(uint i = 0; i < numBoxes; i++){
		while(true){
			uint boxRow = (random() % (numRows - 2))+1;
			uint boxCol = (random() % (numCols - 2))+1;
			pair<uint, uint> proposedPair = make_pair(boxRow, boxCol);
			if(!checkIfPairExists(proposedPair, boxLoc)){
				boxLoc.push_back(new pair<uint, uint>(boxRow, boxCol));
				break;
			}
		}
	}	
}


void robotRandomPlacement(){
	for(uint i = 0; i < numRobots; i++){
		while(true){
			uint robotRow = random() % numRows;
			uint robotCol = random() % numCols;
			pair<uint, uint> proposedPair = make_pair(robotRow, robotCol);
			if(!checkIfPairExists(proposedPair, Robot::robotLoc) && !checkIfPairExists(proposedPair, boxLoc)){
				Robot::robotLoc.push_back(new pair<uint, uint>(robotRow, robotCol));
				break;
			}
		}
	}
}

void doorRandomPlacement(){
	for(uint i = 0; i < numDoors; i++){
		while(true){
			uint doorRow = random() % numRows;
			uint doorCol = random() % numCols;
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


void assignDoors(){
	for (uint i=0; i < Robot::robotLoc.size() && i < doorLoc.size(); i++){
		bool numWorked = false;
		while (numWorked == false){
		uint randomDoor = random() % doorLoc.size();
			if (!checkIfNumExistsInVec(randomDoor, doorAssign)){
				doorAssign.push_back(randomDoor);
				break;
			}	
		}
	}

	for (uint i=doorLoc.size(); i < Robot::robotLoc.size(); i++){
		uint randomDoor = random() % doorLoc.size();
			doorAssign.push_back(randomDoor);
			break;
	}	
	cout << "assigned doors:\n";
	printVector<vector<uint>>(doorAssign);
}


template <typename T>
void printVector(T vec)
{
	for(uint i=0; i < vec.size(); i++){
		cout << vec[i] << " ";
	}
	cout << endl;
}

