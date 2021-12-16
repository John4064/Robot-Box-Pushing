using namespace std;
#include <vector>
#include <utility>
#include <iostream>

typedef unsigned int uint;

using namespace std;

namespace Robot{vector<pair<uint, uint>*> robotLoc;
vector<pair<uint, uint>*> boxLoc;
vector<uint> ;
vector<pair<uint, uint>*> doorLoc;

int numCols, numRows, numDoors, numBoxes;
int& numRobots = numBoxes;

void robotRandomPlacement(){
	for(int i = 0; i < numRobots; i++){
		int robotRow = random() % numRows;
		int robotCol = random() % numCols;
		robotLoc.push_back(new pair<uint, uint>(robotRow, robotCol));
	}
}
void doorRandomPlacement(){
	for(int i = 0; i < numDoors; i++){
		int doorRow = random() % numRows;
		int doorCol = random() % numCols;
		doorLoc.push_back(new pair<uint, uint>(doorRow, doorCol));
	}
}

void boxRandomPlacement(){
	for(int i = 0; i < numDoors; i++){
		int boxRow = random() % numRows;
		int boxCol = random() % numCols;
		boxLoc.push_back(new pair<uint, uint>(boxRow, boxCol));
	}
}

void printObjectPlacements(){
	cout << "Door locations:" << endl;
	for(int i=0; i < doorLoc.size(); i++){
		cout <<"\t"<< doorLoc[i]->first << ", "<<doorLoc[i]->second<< endl;
	}
	cout << endl;
	cout << "Robot locations:" << endl;
	for(int i=0; i < robotLoc.size(); i++){
		cout <<"\t"<< robotLoc[i]->first << ", "<<robotLoc[i]->second<< endl;
	}
	cout << endl;
	cout << "Box locations:" << endl;
	for(int i=0; i < boxLoc.size(); i++){
		cout <<"\t"<< boxLoc[i]->first << ", "<<boxLoc[i]->second<< endl;
	}
	cout << endl;
}



int main(int argc, char** argv){

	numRows = atoi(argv[1]);
	numCols = atoi(argv[2]);
	numBoxes = atoi(argv[3]);
	numDoors = atoi(argv[4]);

    robotRandomPlacement();
	doorRandomPlacement();
	boxRandomPlacement();
    printObjectPlacements();

    return 0;
}



