#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <pthread.h>
#include <sys/time.h>

int **shudu;
int **ans;
int num_of_thread;
int curr_shudu;
int num_of_shu;
pthread_mutex_t lock;
pthread_mutex_t lock1;

using namespace std;

 void* work(void* argv){

  int temp = curr_shudu;
  while(1){
    pthread_mutex_lock(&lock1);
    if( curr_shudu<1000)
    curr_shudu++;
    printf("tenp: %d   curr: %d  \n" , temp,curr_shudu);
    pthread_mutex_unlock(&lock1);
    if(curr_shudu>=1000) break;
     
  }
 
  

}

int main(int argc, char* argv[]){
  curr_shudu = 0;
  num_of_thread = 4;
  pthread_mutex_init(&lock, NULL);
  pthread_mutex_init(&lock1, NULL);
  pthread_mutex_lock(&lock1); 
  pthread_mutex_lock(&lock); 
  pthread_t pid[num_of_thread];
  for(int i=0;i<num_of_thread;i++){
    pthread_create(pid + i , NULL, work,NULL);
  }
  pthread_mutex_unlock(&lock1); 
  pthread_mutex_unlock(&lock);  


  for(int i=0;i<num_of_thread;i++){
    pthread_join(pid[i], NULL);
  }


  printf("the end of main!\n");
}


// sudoku: main.cc neighbor.cc sudoku_basic.cc sudoku_min_arity.cc sudoku_min_arity_cache.cc sudoku_dancing_links.cc
// 	g++ -O2 -lpthread -o $@ $^

