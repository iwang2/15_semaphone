#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define KEY ftok("control.c", 1)

int main (int argc, char * argv[]){

  int
    fd /*file descriptor*/,
    sd /*semaphore descriptor*/,
    i,
    sv /*semaphore value*/,
    smd /*shared memory descriptor*/;
  
  for(i = 0; argv[i]; i++){

    // CREATION
    if (!strcmp(argv[i], "-c")) {

      sd = semget(KEY, 1, IPC_CREAT | IPC_EXCL | 0600);
      if (sd == -1) {
	printf("semaphore already exists\n");
      } else {
	smd = shmget(KEY, 1024, IPC_CREAT);
	fd = open("story.txt", O_TRUNC | O_CREAT, 0644);
      }
      close(fd);
    }

    // VIEWING STORY
    if (!strcmp(argv[i], "-v")) {
      fd = open("story.txt", O_RDONLY);
      char * buff;

      struct stat st;
      stat("story.txt", &st);
      int size = (int)st.st_size;

      read(fd, buff, size);
      printf("%s\n", buff);

      close(fd);
    }

    if (!strcmp(argv[i], "-r")) {
      
      //read file into buff
      fd = open("story.txt", O_RDONLY);
      char * buff;
      
      struct stat st;
      stat("story.txt", &st);
      int size = (int)st.st_size;
      
      read(fd, buff, size);
      close(fd);
      remove("story.txt");

      //removing semaphore
      sd = semget(KEY, 1, 0);
      sv = semctl(sd, 0, IPC_RMID);
      if (sv == -1) {
	printf("error removing semaphore: %s\n\n", strerror(errno));
      } else {
	printf("semaphore removed: %d\n\n", sd);
      }

      //remove shared memory
      smd = shmget(KEY, 1024, 0);
      int smr = shmctl(smd, IPC_RMID, 0);
      if(smr == -1){
	printf("error removing shared memory: %s\n", strerror(errno));
      } else {
	printf("shared memory removed\n");
      }

      printf("\nFINAL STORY:\n%s\n", buff);
    }
  }
}
