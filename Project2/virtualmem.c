#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

void help();
void readNums();

//linked list structure

struct node{

  int id; //actual number assigned to node from input
  int freq; //number of times the number has occurred used for LFU
  struct node *next;
};

struct node *head = NULL;
struct node *curr = NULL;

struct node* create_list(int val){
  struct node *ptr = (struct node*)malloc(sizeof(struct node));
  if(ptr == NULL){
    printf("Error creating node\n");
    return NULL;
  }
  ptr->id = val;
  ptr->next = NULL;
  head = curr = ptr;
  return ptr;

}

struct node* add_elem(int val){
  if(head == NULL){
    return (create_list(val));
  }
  struct node *ptr = (struct node*)malloc(sizeof(struct node));
  if(ptr == NULL){
    printf("Error creating node\n");
    return NULL;
  }
  ptr->id = val;
  ptr->next = NULL;

  curr->next = ptr;
  curr = ptr;

  return ptr;

}

struct node* find_elem(int elem){ // possibly add frame size limiter here
  struct node *ptr = head;
  struct node *temp = NULL;
  bool found = false;

  while(ptr != NULL){
    if(ptr->id == elem){
      found = true;
      break;
    }else{
      temp = ptr;
      ptr = ptr->next;
    }

  }
  if(found == true){ //increment count on node freq
    printf("Target Found\n");
    ptr->freq += 1;
  }
  else{ //think about what to do here, replace element?
    return NULL;
  }

}
int main(int argc, char* argv[]){
  char* repPol = "FIFO";
  int frames = 5;
  int i;
  int readflg = 0;
  char* filename;
  for(i=1; i < argc; i++){
    if(strcmp(argv[i],"-h") == 0){
      help();
    }else if(strcmp(argv[i], "-f") == 0){
      frames = atoi(argv[i+1]);
      i++;
    }else if(strcmp(argv[i], "-r") == 0){
      repPol = argv[i+1];
      i++;
    }else if(strcmp(argv[i], "-i") == 0){
      readflg = 1;
      filename = argv[i+1];
    }

  }
  readNums(readflg, filename);
  printf("Number of frames is: %d\n", frames);
  printf("Replacement policy is %s\n", repPol);




  return 0;
}
//needs work for stdin
void readNums(int flag, char* filename){
  char buf[255];
  if(flag == 0){ //stdin
     printf("Reading from stdin, numbers only! Hit enter when done\n");
     char *inBuf;
     size_t len = 0;
     ssize_t read;
     read = getline(&inBuf,&len,stdin);
     //Call Page Replacement Algorithm on inBuf

    }else{ //read from file
     FILE *fp;
     fp = fopen(filename, "r");
     fgets(buf, 255, (FILE*)fp);
     printf(buf);
     //Call Page Replacement Algorithm on buf
  }
}

void help(){
  printf("prog [-h][-f available frames][-r replacement-policy][-i input-file]\n\n");
  printf("-h                    :Print a usage summary with all options and exit\n\n");
  printf("-f available frames   :Set the number of frames, by default set to 5\n\n");
  printf("-r replacement-policy :Set the page replacement policy. Options follow:\n\n");
    printf("\t\tFIFO(First-in-first-out)\n");
    printf("\t\tLFU(Least-frequenty-used)\n");
    printf("\t\tLRU-STACK(Least-recently-used stack implmentation)\n");
    printf("\t\tLRU-CLOCK(Least-recently-used clock implementation)\n");
    printf("\t\tLRU-REF8(Least-recently-used Reference-bit Implementation, using 8 reference bits)\n");
    printf("\t\tDefault is FIFO\n\n");
  printf("-i input-file         :Read the page reference sequence from a specified file.\n\t\t\tIf not given, the sequence will be read from STDIN\n\n");
  exit(1);
}
