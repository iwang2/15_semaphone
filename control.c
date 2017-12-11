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
	return -1;
      }
      else {
	// FIGURE OUT UNION THING (keeps returning an error)
	semctl(sd, 0, SETVAL, 1);	
	smd = shmget(KEY, sizeof(int), IPC_CREAT | 0644);
	if (smd == -1) {
	  printf("error creating shared memory: %s\n", strerror(errno));
	  return -1;
	}
	else {
	  //create story.txt
	  fd = open("story.txt", O_TRUNC | O_CREAT, 0644);
	  close(fd);
	  printf("Success! Semaphore has been created!\n");
	  return 1;
	}
      }
    }

    // VIEWING STORY
    else if (!strcmp(argv[i], "-v")) {
      sd = semget(KEY, 1, 0600);
      if(sd == -1){
	printf("Semaphore has not been created yet!\n");
	return -1;
      }
      else{
	struct stat st;
	stat("story.txt", &st);
	int size = (int)st.st_size;
      
	fd = open("story.txt", O_RDONLY);
	char * buff = malloc(size);
	read(fd, buff, size);
	close(fd);

	printf("Here's the story:\n%s\n", buff);
	free(buff);
	return 1;
      }
    }

    // REMOVAL
    else if (!strcmp(argv[i], "-r")) {

      //removing semaphore
      sd = semget(KEY, 1, 0);
      sv = semctl(sd, 0, IPC_RMID);
      if (sv == -1) {
	printf("error removing semaphore: %s\n\n", strerror(errno));
	return -1;
      } else {
	printf("semaphore removed: %d\n\n", sd);
      }

      //remove shared memory
      smd = shmget(KEY, sizeof(int), 0);
      int smr = shmctl(smd, IPC_RMID, 0);
      if(smr == -1){
	printf("error removing shared memory: %s\n", strerror(errno));
	return -1;
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
      return 1;
    }
  }
  printf("Not a valid command! Please try:\n\t-c: Create a semaphore\n\t-v: View the value of the semaphore\n\t-r: Remove the semaphore\n");
  return -1;
}
