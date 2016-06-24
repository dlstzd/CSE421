#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
void help();
void readNums();

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
      //printf("Number of frame is %s\n", argv[i+1]);
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
  printf("replacement policy is %s\n", repPol);



  
  return 0;
}

void readNums(int flag, char* filename){

  if(flag == 0){
    printf("No file specified, reading from STDIN Press ] to break...\n>");
    char ch;
    do{
      ch = getchar();
      printf("character is %c\n", ch);
    }while(getchar() != ']');
  }else{
    char buf[255];     
    FILE *fp;
    fp = fopen(filename, "r");
    fgets(buf, 255, (FILE*)fp);
    printf(buf);
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
