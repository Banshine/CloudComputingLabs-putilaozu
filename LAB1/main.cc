#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <pthread.h>
#include <sys/time.h>

#include "sudoku.h"
using namespace std;

int **shudu;
int **ans;
int num_of_thread =4;
int curr_shudu;
int num_of_shu;
int64_t time_start;
int64_t time_end ;
bool (*solve)(int* ) = solve_sudoku_dancing_links;
pthread_mutex_t lock;
pthread_mutex_t lock1;
int64_t now()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000 + tv.tv_usec;
}

void* work(void * unuse);

int main(int argc, char* argv[])
{
  init_neighbors();

  FILE* fp = fopen(argv[1], "r");  //argv[1] 为输入文件
  char puzzle[128];
  int total_solved = 0;
  int total = 0;
 

  vector<char>shu;
  vector<vector<char> >shus;

  while (fgets(puzzle, sizeof puzzle, fp) != NULL){
      for(int i=0;i<81;i++)
        shu.push_back(puzzle[i]);
      shus.push_back(shu);
      shu.clear();
  }
  num_of_shu = shus.size();
  shudu = (int **)malloc(sizeof(int *) * num_of_shu);
  ans = (int **)malloc(sizeof(int *) * num_of_shu);
  for(int i=0;i<num_of_shu;i++){
     shudu[i] = (int *)malloc(sizeof(int) * (81));
     ans[i] = (int *)malloc(sizeof(int) * (81));
     for(int j=0;j<81;j++)
          shudu[i][j] = shus[i][j]-48;
  }
 //处理输入
  // for(int i=0;i<num_of_shu;i++){//查看输入
  //   for(int j=0;j<81;j++)
  //   printf("%d",shudu[i][j] );
  //   printf("\n");
  // }
  printf("num_of_shu:%d\n",num_of_shu);
  printf("input succeed！\n");

  curr_shudu=0;

  pthread_mutex_init(&lock, NULL);
  pthread_mutex_init(&lock1, NULL);
  
  pthread_t pid[num_of_thread];

  time_start = now();  
  pthread_mutex_lock(&lock); 
  pthread_mutex_lock(&lock1); 
  for(int i=0;i<num_of_thread;i++){
    int ret = pthread_create(pid + i, NULL, work, NULL);
      if (ret != 0)
      printf("create pthread error!" );   
  }
  printf("create pthread succeed!\n");
  pthread_mutex_unlock(&lock); 
  pthread_mutex_unlock(&lock1); 
 
  for(int i=0;i<num_of_thread;i++){
    pthread_join(pid[i],  NULL);
  }

  pthread_mutex_lock(&lock1); 
  time_end = now();
  double sec = (time_end-time_start)/1000000.0;
  printf("%f sec %f ms each %d\n", sec, 1000*sec/total, total_solved);

  printf("total:%d \n" , total);

  return 0;
}
void* work(void* unuse) {
  int temp;
   
   while (true) {
    //加锁访问任务队列
    printf( "create thread %d\n",((unsigned)pthread_self() % 100) );
    pthread_mutex_lock(&lock);

    if (curr_shudu >= num_of_shu) {
      printf("-------------\n");
      pthread_mutex_unlock(&lock1);  // 确保所有的线程都被创建
      break; 
    }

    temp=curr_shudu;
    curr_shudu++;
    printf("curr_shudu:%d\n",curr_shudu);
    pthread_mutex_unlock(&lock);
    int *tmp = new int[81];  //注意没有释放 内存
    for (int i = 0; i < 81; i++) tmp[i] = shudu[temp][i];
    if (solve(tmp)) {
      for (int i = 0; i < 81; i++) ans[temp][i] = tmp[i];
    } else {
      printf("no ans\n") ;
    }

    // 执行完计算任务，自动回去！
  }
  pthread_mutex_unlock(&lock);
}
