#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define KEY ftok("control.c", 1)

int main (int argc, char * argv[]){
  
  int sd, i, sv;
  
  for(i = 0; argv[i]; i++){
    
    if (!strcmp(argv[i], "-c")) {
      int fd = open("story.txt", O_TRUC | O_CREAT, 0644);
      
      printf("creating semaphore...\n");
      sd = semget(KEY, 1, IPC_CREAT | IPC_EXCL | 0600);
      if (sd == -1) {
	printf("semaphore already exists\n\n");
      } else {
	printf("semaphore created: %d\n", sd);
	semctl(sd, 0, SETVAL, fd);
	//printf("value set: %d\n\n", value);
      }
      close(fd);
    }

    if (!strcmp(argv[i], "-v")) {
      int fd = open("story.txt", O_RDONLY);
      char * s;

      struct stat st;
      stat("story.txt", &st);
      int size = (int)st.st_size;

      read(fd, s, size);
      printf("%s\n", s);

      close(fd);
    }

    if (!strcmp(argv[i], "-r")) {
      printf("removing semaphore...\n");
      sd = semget(KEY, 1, 0);
      sv = semctl(sd, 0, IPC_RMID);

      if (sv == -1) {
	printf("error removing semaphore: %s\n\n", strerror(errno));
      } else {
	printf("semaphore removed: %d\n\n", sd);
      }
    }
  }
    
}
