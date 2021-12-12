using namespace std;
#include <vector>
#include <utility>
#include <iostream>

typedef unsigned int uint;

using namespace std;


template <typename T>
void printVector(T vec)
{
	for(int i=0; i < vec.size(); i++){
		cout << vec[i] << " ";
	}
	cout << endl;
}


int main(int argc, char** argv){

	vector<uint> vec;
	vec.push_back(4);
	vec.push_back(5);
	vec.push_back(6);
	printVector<vector<uint>>(vec);

    return 0;
}



