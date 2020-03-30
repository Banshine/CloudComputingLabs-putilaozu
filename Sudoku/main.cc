#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <sys/time.h>

#include "sudoku.h"

char **shudu;

int64_t now()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000 + tv.tv_usec;
}

int main(int argc, char* argv[])
{
  init_neighbors();

  FILE* fp = fopen(argv[1], "r");  //argv[1] 为输入文件
  char puzzle[128];
  int total_solved = 0;
  int total = 0;
  bool (*solve)(int) = solve_sudoku_dancing_links;

  shudu = (char**)malloc(1005 * sizeof(128));
  int count0 = 0;
  //while (fgets(puzzle, sizeof puzzle, fp) != NULL){

 // }
 

  int64_t start = now();
  while (fgets(puzzle, sizeof puzzle, fp) != NULL) {

    if (strlen(puzzle) >= N) {
      ++total;
      char str[128];
      strcpy(str,puzzle);
      //*(shudu + count0) = str;
      count0++;
      //printf("%d \n" , total);
      input(puzzle);
      init_cache();

      if (solve(0)) {
        ++total_solved;
        if (!solved())
          assert(0);
      }
      else {
        printf("No: %s", puzzle);
      }
    }
  }
  int64_t end = now();
  double sec = (end-start)/1000000.0;
  printf("%f sec %f ms each %d\n", sec, 1000*sec/total, total_solved);

  printf("%d \n" , total);
  printf("%d \n" , count0);

  // for(int i=0;i<count0 ; i++)
  // printf("%s\n", shudu[i]);
  return 0;
}

