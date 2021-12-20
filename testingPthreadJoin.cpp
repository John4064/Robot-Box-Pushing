using namespace std;
#include <vector>
#include <utility>
#include <iostream>
#include <pthread.h>

typedef unsigned int uint;

using namespace std;

pthread_t threadJoin;
pthread_t threadFunc;
	

void* threadFunct(void *){


	cout << "thread func is doing stuff" << endl;


	return NULL;
}

void* threadJoinFunc(void *){
	
	pthread_join(threadFunc, NULL);

	cout << "thread func successfully joined" << endl;

	return NULL;
}




int main(int argc, char** argv){


	pthread_create(&threadFunc, NULL, &threadFunct, NULL);

	pthread_create(&threadJoin, NULL, &threadJoinFunc, NULL);

	pthread_join(threadJoin, NULL);

	cout << "thread join successfully joined" << endl;

    return 0;
}



