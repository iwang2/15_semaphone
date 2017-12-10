#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define KEY 23318

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
      }
      else {
	fd = open("story.txt", O_TRUNC | O_CREAT, 0644);
	semctl(sd, 0, SETVAL, 1);	
	smd = shmget(KEY, sizeof(int), IPC_CREAT);
	close(fd);
      }
    }

    // VIEWING STORY
    if (!strcmp(argv[i], "-v")) {

      struct stat st;
      stat("story.txt", &st);
      int size = (int)st.st_size;
      printf("size of story.txt: %d\n", size);

      char * buff = malloc(size);
      fd = open("story.txt", O_RDONLY | O_TRUNC);
      int rr = read(fd, buff, size);      
      if(rr == -1){
	printf("error reading story.txt: %s\n", strerror(errno));
      }else{
	printf("%s\n", buff);
      }
      free(buff);
      close(fd);
    }

    // REMOVAL
    if (!strcmp(argv[i], "-r")) {

      //removing semaphore
      sd = semget(KEY, 1, 0);
      sv = semctl(sd, 0, IPC_RMID);
      if (sv == -1) {
	printf("error removing semaphore: %s\n\n", strerror(errno));
      } else {
	printf("semaphore removed: %d\n\n", sd);
      }

      //remove shared memory
      smd = shmget(KEY, sizeof(int), 0);
      int smr = shmctl(smd, IPC_RMID, 0);
      if(smr == -1){
	printf("error removing shared memory: %s\n", strerror(errno));
      } else {
	printf("shared memory removed\n");
      }
      
      //read file into buff
      struct stat st;
      stat("story.txt", &st);
      int size = (int)st.st_size;
      
      fd = open("story.txt", O_RDONLY);
      char * buff = malloc(size);
      read(fd, buff, size);
      close(fd);
      remove("story.txt");

      printf("\nFINAL STORY:\n%s\n", buff);
      free(buff);
    }
  }
}
