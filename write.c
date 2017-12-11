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

/*
attempt to access resources using the semaphore
- once in, display the last line added to the file (shared memory contains its size)
- prompt the user for the next line
- once a new line is read, write it to the file, update the shared memory, and release the semaphore
*/

int main () {
  int
    fd, //file descriptor
    sd, //semaphore descriptor
    smd; //shared memory descriptor

  // SEMAPHORE STUFF
  sd = semget(KEY, 1, 0);
  if(sd == -1){
    printf("error getting semaphore: %s\n", strerror(errno));
    exit(0);
  }
  struct sembuf sb = {0, -1, SEM_UNDO};
  semop(sd, &sb, 1);

  // GETTING LAST LINE
  smd = shmget(KEY, sizeof(int), 0);
  if(smd == -1){
    printf("error getting shared memory: %s\n", strerror(errno));
    exit(0);
  }
  int * size = shmat(smd, 0, 0);
  if(*size == -1){
    printf("error attaching shared memory to variable: %s\n", strerror(errno));
    exit(0);
  }
  else if(*size == 0){
    printf("You're starting with a clean slate!\n");
  }
  else {
    fd = open("story.txt", O_RDONLY);
    int pos = lseek(fd, *size * -1, SEEK_END);
    char * buff = malloc(*size);
    read(fd, buff, *size);
    printf("previous line of the story:\n%s\n", buff);
    close(fd);
    free(buff);
  }
  printf("Type the next line:\n");
  char input[sizeof(char) * 2000];
  fgets(input, sizeof(input), stdin);
  fd = open("story.txt", O_WRONLY | O_APPEND);

  *size = strlen(input);
  write(fd, input, *size);
  close(fd);
  shmdt(size);
  
  sb.sem_op = 1;
  semop(sd, &sb, 1);
}
