#include "include/socketServer.h"
#include "include/socketClient.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

int main(){
  char buff[20];
  fgets(buff, 20, stdin);
  int i = 0;

  char *p = strtok (buff, " ");
  char *data[3];

  while (p)
    {
      data[i++] = p;
      p = strtok (NULL, " ");
    }

  for (i = 0; i < 3; i++)
    {
      printf("data[%i] = %s\n", i+1, data[i]);
    }
  
  

  return 0;

  
}
