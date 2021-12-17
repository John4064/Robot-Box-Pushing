#include <pthread.h>
#include <stdio.h>    // For printf()
#include <stdlib.h>   // For exit()
#include <string.h>   // For strlen()
#include <unistd.h>   // For sleep() if needed
#include <utility>
#include <vector>


typedef unsigned int uint;

int count;
// std::vector <std::pair<uint, uint>*> boxLoc1 = std::vector <std::pair<uint, uint>*>();
// std::vector <std::pair<uint, uint>*>&boxLoc =*boxLoc1;

std::vector <std::pair<uint, uint>*> boxLoc;
// std::pair<uint, uint>* pair0 = new std::pair<uint, uint>();
// std::pair<uint, uint> &pair1 = *pair0;
// std::pair<uint, uint>* pair3 = new std::pair<uint, uint>();
// std::pair<uint, uint> &pair2 = *pair3;
 std::pair<uint, uint> pair1;
std::pair<uint, uint> pair2;
static void
*code_for_thread1(void *arg)
{
    for (int i = 0; i < boxLoc.size(); i++){
        printf("Initial values of vector in thread 1\\
        are %d ,%d, %d \n", i, boxLoc[i]->first, boxLoc[i]->second); 
    }

    for (int i = 0; i < boxLoc.size(); i++){
        boxLoc[i]->first = boxLoc[i]->first + i + 1;
        boxLoc[i]->second = boxLoc[i]->second + i + 1;
        printf("values after modification  of vector in thread 1\\
        are %d ,%d, %d \n", i, boxLoc[i]->first, boxLoc[i]->second); 
    }


    printf("count in main thread is initially %d\n", count);

//   printf("count in thread 1 is initially %d\n", count);
//   count = 20;
//   printf("count in thread 1 is finally   %d\n", count);
  return NULL;
}



int
main(void)
{
  pthread_t thread1;
  void      *thread1_result;

//   count = 10;
    pair1.first = 10;
    pair1.second = 11;
    pair2.first = 13;
    pair2.second = 14;
  boxLoc.push_back(&pair1);
  boxLoc.push_back(&pair2);
//   printf("count in main thread is initially %d\n", count);
for (int i = 0; i < boxLoc.size(); i++){
    printf("Initial values of vector are %d :   %d, %d \n", i, boxLoc[i]->first, boxLoc[i]->second); 
}
    // printf("count in main thread is initially %d\n", count);
    pthread_create(&thread1, NULL, code_for_thread1, NULL);
    pthread_join(thread1, &thread1_result);
  for (int i = 0; i < boxLoc.size(); i++){
    printf(" values of vector after joining in main are %d ,%d, %d \n", i, boxLoc[i]->first, boxLoc[i]->second); 
}
    printf("count in main thread is finally   %d\n", count);
    exit(0);
}