#include <stdio.h>
#include<stdlib.h>

int main(){
    char **p;
    p = (char**)malloc(500 * sizeof(int *));
    char s[128];
    int i=0;
     int c= sizeof(p);
      printf("size : %d", c  );
      scanf("%s", s);
    // while(scanf("%s", s)){
    //      printf("size : %d", c  );
    //     p[i] = s;
    // }
   
    printf("size : %d", c  );
}