#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

void help();
void readNums();

int OPTIMAL();
int FIFO();
int LFU();
int LRU_STACK();
int LRU_CLOCK();
int LRU_REF8();

struct timeval start_time();
struct timeval stop_time();
struct timeval total_time(struct timeval,struct timeval);
void print_time_ms();




#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netdb.h>
#include 	<time.h>
#include	<netinet/in.h>
#include	<inttypes.h>
#include    <pthread.h>
#include    <semaphore.h>
#include    <fcntl.h>
#include    <stdbool.h>

struct node{

  int id; //number id assigned to node from input
  int freq; //number of times the number has occurred -- used for LFU
  int dist_to_next; //number of characters to go through in buffer until we find another occurence -- 200 is an arbitrary large number
  struct node *next;
};

struct node *head = NULL;
struct node *curr = NULL;

void print_list(void){
    struct node *ptr = head;
    printf("\nThe list is:\n");
    while(ptr != NULL){
        printf("%d, %d, \n", ptr->id, ptr->freq);
        ptr = ptr->next;
    }
    return;
}

struct node* create_list(){
  struct node *ptr = (struct node*)malloc(sizeof(struct node));
  if(ptr == NULL){
    printf("Error creating node\n");
    return NULL;
  }
  ptr->id = -1;
  ptr->freq = -1;
  ptr->next = NULL;
  head = curr = ptr;
  return ptr;

}

bool find_elem(int elem){ // possibly add frame size limiter here
  struct node *ptr = head;
  struct node *temp = NULL;
  bool found = false;

  while(ptr != NULL){
    if(ptr->id == elem){
        printf("id: %d   elem: %d\n", ptr->id, elem);
      found = true;
      break;
    }else{
      temp = ptr;
      ptr = ptr->next;
    }

  }
  if(found == true){ //increment count on node freq
    printf("Target Found\n");
    return true;
  }
  else{ //think about what to do here, replace element?
    printf("false \n");
    return false;
  }

}

struct node* add_elem(){
  if(head == NULL){
    return (create_list());
  }
  struct node *ptr = (struct node*)malloc(sizeof(struct node));
  if(ptr == NULL){
    printf("Error creating node\n");
    return NULL;
  }
  ptr->id = -1;
  ptr->freq = -1;
  ptr->next = NULL;

  curr->next = ptr;
  curr = ptr;

  return ptr;

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

  printf("Number of frames is: %d\n", frames);
  printf("Replacement policy is %s\n", repPol);
  readNums(readflg, filename,frames,repPol);
  return 0;
}
//needs work for stdin
void readNums(int flag, char* filename,int frames,char* repAlgorithm){
  //Buffer used only for file reading page queue
  char buf[255];

  int algPageReplacements = 0;
  int optimalPageReplacements = 0;
  struct timeval start,stop;
  struct timeval algTime;
  struct timeval optimalTime;

  double algPenalty = 0.0;
  double percentFaster = 0.0;

  //fprintf(stderr,"repAlgorithm = %s\n",repAlgorithm);

  if(flag == 0){ //stdin
     printf("Reading from stdin, numbers only! Hit enter when done\n");
     //Buffer used only for user input page queue.
     char *inBuf;

     //Get user input for Pager Queue
     size_t len = 0;
     ssize_t read;
     read = getline(&inBuf,&len,stdin);
     printf("Page Queue is: %s",inBuf);

     //Call Page Replacement Algorithm on inBuf
     if(strcmp(repAlgorithm,"FIFO") == 0){
        start = start_time();
        fprintf(stderr,"Attempting to call FIFO()\n");
	 int i = 0;
	 while(i < frames){
	   add_elem();
	   i++;
	 }
	curr=head;
        algPageReplacements = FIFO(inBuf);
        stop = stop_time();
        algTime = total_time(start,stop);
     }else if(strcmp(repAlgorithm,"LFU") == 0){
        start = start_time();
        fprintf(stderr,"Attempting to call LFU()\n");
        algPageReplacements = LFU(frames,inBuf);
        stop = stop_time();
        algTime = total_time(start,stop);
     }else if(strcmp(repAlgorithm,"LRU-STACK") == 0){
        start = start_time();
        fprintf(stderr,"Attempting to call LRU_STACK()\n");
        algPageReplacements = LRU_STACK(frames,inBuf);
        stop = stop_time();
        algTime = total_time(start,stop);
     }else if(strcmp(repAlgorithm,"LRU-CLOCK") == 0){
        start = start_time();
        fprintf(stderr,"Attempting to call LRU_CLOCK()\n");
        algPageReplacements = LRU_CLOCK(frames,inBuf);
        stop = stop_time();
        algTime = total_time(start,stop);
     }else if(strcmp(repAlgorithm,"LRU-REF8") == 0){
        start = start_time();
        fprintf(stderr,"Attempting to call LRU_REF8()\n");
        algPageReplacements = LRU_REF8(frames,inBuf);
        stop = stop_time();
        algTime = total_time(start,stop);
     }else{
        printf("Invalid Algorithm\n");
        exit(0);
     }
     //Run Optimal Algorithm
     start = start_time();
     optimalPageReplacements = OPTIMAL(frames,inBuf);
     stop = stop_time();
     optimalTime = total_time(start,stop);

     //Compare times and page replacements between optimal and algorithm.
     /* Example of output.
        # of page replacements with LFU : 118
        # of page replacements with Optimal : 85
        % page replacement penalty using LFU : 38.8%

        Total time to run LFU algorithm : 1214 msec
        Total time to run Optimal algorithm : 1348 msec
        LFU is 9.9% faster than Optimal algorithm.
     */
     printf("\n");
     printf("# of page replacements with %s : %i\n",repAlgorithm,algPageReplacements);
     printf("# of page replacements with Optimal: %i\n",optimalPageReplacements);
     printf("# page replacement penalty using %s: %f\n",repAlgorithm,algPenalty);
     printf("\n");
     printf("\n");
     printf("Total time to run %s algorithm: ",repAlgorithm);
     print_time_ms(algTime);

     printf("Total time to run Optimal algorithm: ");
     print_time_ms(optimalTime);
     if(algTime.tv_sec < optimalTime.tv_sec){
        printf("%s is %f faster than Optimal algorithm\n",repAlgorithm,percentFaster);
     }else if(algTime.tv_sec > optimalTime.tv_sec){
        printf("Optimal is %f faster than %s algorithm\n",percentFaster,repAlgorithm);
     }else if(algTime.tv_sec == optimalTime.tv_sec){
        printf("%s and Optimal performed at the same speed.\n",repAlgorithm);
     }

    }else{
     //read from file
     printf("Reading from File\n");
     FILE *fp;
     fp = fopen(filename, "r");
     fgets(buf, 255, (FILE*)fp);
     printf("Page Queue is : %s",buf);
     //Call Page Replacement Algorithm on buf

     int i = 0;
     while(i < frames){
       add_elem();
       i++;
     }
     curr=head;

     if(strcmp(repAlgorithm,"FIFO")== 0){
        start = start_time();
        fprintf(stderr,"Attempting to call FIFO()\n");

        algPageReplacements = FIFO(buf);
	printf("IIIIII : %d", algPageReplacements);
        stop = stop_time();
        algTime = total_time(start,stop);
     }else if(strcmp(repAlgorithm,"LFU") == 0){
        start = start_time();
        fprintf(stderr,"Attempting to call LFU()\n");
        algPageReplacements = LFU(frames,buf);
        stop = stop_time();
        algTime = total_time(start,stop);
     }else if(strcmp(repAlgorithm,"LRU-STACK")== 0){
        start = start_time();
        fprintf(stderr,"Attempting to call LRU_STACK()\n");
        algPageReplacements = LRU_STACK(frames,buf);
        stop = stop_time();
        algTime = total_time(start,stop);
     }else if(strcmp(repAlgorithm,"LRU-CLOCK") == 0){
        start = start_time();
        fprintf(stderr,"Attempting to call LRU_CLOCK()\n");
        algPageReplacements = LRU_CLOCK(frames,buf);
        stop = stop_time();
        algTime = total_time(start,stop);
     }else if(strcmp(repAlgorithm,"LRU-REF8") == 0){
        start = start_time();
        fprintf(stderr,"Attempting to call LRU_REF8()\n");
        algPageReplacements = LRU_REF8(frames,buf);
        stop = stop_time();
        algTime = total_time(start,stop);
     }else{
        printf("Invalid Algorithm\n");
        exit(0);
     }
     //Run Optimal Algorithm
     start = start_time();
     fprintf(stderr,"Attempting to call OPTIMAL()\n");
     optimalPageReplacements = OPTIMAL(frames,buf);
     stop = stop_time();
     optimalTime = total_time(start,stop);

     //Compare times and page replacements between optimal and algorithm.
     /* Example of output.
        # of page replacements with LFU : 118
        # of page replacements with Optimal : 85
        % page replacement penalty using LFU : 38.8%

        Total time to run LFU algorithm : 1214 msec
        Total time to run Optimal algorithm : 1348 msec
        LFU is 9.9% faster than Optimal algorithm.
     */
     printf("\n");
     printf("# of page replacements with %s : %i\n",repAlgorithm,algPageReplacements);
     printf("# of page replacements with Optimal: %i\n",optimalPageReplacements);
     printf("# page replacement penalty using %s: %f\n",repAlgorithm,algPenalty);
     printf("\n");
     printf("Total time to run %s algorithm: ",repAlgorithm);
     print_time_ms(algTime);

     printf("Total time to run Optimal algorithm: ");
     print_time_ms(optimalTime);
     if(algTime.tv_sec < optimalTime.tv_sec){
        printf("%s is %f faster than Optimal algorithm\n",repAlgorithm,percentFaster);
     }else if(algTime.tv_sec > optimalTime.tv_sec){
        printf("Optimal is %f faster than %s algorithm\n",percentFaster,repAlgorithm);
     }else if(algTime.tv_sec == optimalTime.tv_sec){
        printf("%s and Optimal performed at the same speed.\n",repAlgorithm);
     }

  }

 //Print results from optimal and algorithm
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

//Page Replacement Algorithms
int OPTIMAL(int frames,char *buffer){
 fprintf(stderr,"Entered OPTIMAL Alg\n");
 int pageReplacements = 0;

 return pageReplacements;
}
int FIFO(char *buffer){
 fprintf(stderr,"Entered FIFO Alg\n");
 int pageReplacements = 0;
 //Setup Frame


 //Begin Page Replacement
 //Start at beginning of Buffer
 //The first pages should simply be inserted into frame
 //Until it reaches frame size.
 int d = 0;
 printf("BUF: %s", buffer);
 for(d;d<strlen(buffer)-1;d++){
    //Set val to the next character in buffer , convert to integer.
   if(buffer[d] == ' '){
     printf("Space detected\n");
     //d++;
   }
   else{
     printf("HI\n");
    int val = atoi(&buffer[d]);
    //printf("VALUE IS: %d\n\n\n", val);

    struct node *ptr = (struct node*)malloc(sizeof(struct node));
    ptr = curr;
    if (ptr->id == -1){
        ptr->id = val;
        if(ptr->next == NULL){
            curr = head;
        }
        else{ curr = ptr ->next; }
        }
    if (find_elem(val)){
        //Do Nothing, do not need to count hits
        printf("Element found\n");
    }
    else{
       //Increment Page Replacement Counter.
       pageReplacements++;
       ptr->id = val;
        if(ptr->next == NULL){
            curr = head;
        }
        else{ curr = ptr ->next; }
    }
   }
 }
 print_list();
 return pageReplacements;
}

int LFU(int frames,char *buffer){
  int pageReplacements = 0;
 fprintf(stderr,"Entered LFU Alg\n");
 int d = 0;
 int temp=0;
 printf("BUF: %s", buffer);
 for(d;d<strlen(buffer)-1;d++){
    //Set val to the next character in buffer , convert to integer.
   if(buffer[d] == ' '){
     printf("Space detected\n");
     //d++;
   }
   else{
     printf("HI\n");
    int val = atoi(&buffer[d]);
    //printf("VALUE IS: %d\n\n\n", val);

    struct node *ptr = (struct node*)malloc(sizeof(struct node));
    ptr = curr;
    if (ptr->id == -1){
        ptr->id = val;
	ptr->freq = 0;
        if(ptr->next == NULL){
            curr = head;
        }
        else{ curr = ptr ->next; }
    }
    if(find_elem(val)){
        //Do Nothing, do not need to count hits
      int temp = ptr->id;
      while(ptr->id != val){
	if(ptr->next==NULL){
	  ptr = head;}
	else{
	ptr= ptr->next;
	}
      }
      ptr->freq++;
      while(ptr->id != temp){
	if(ptr->next==NULL){
	  ptr=head;
	}
	else{
	ptr = ptr->next;
	}
      }
        printf("Element found\n");
	//ptr->freq++;
    }
    else{
       //Increment Page Replacement Counter.




      pageReplacements++;
       ptr->id = val;
       ptr->freq = 1;
        if(ptr->next == NULL){
            curr = head;
        }
        else{ curr = ptr ->next; }
    }
   }
 }
 print_list();
 return pageReplacements;
}
int LRU_STACK(int frames,char *buffer){
 fprintf(stderr,"Entered LRU_STACK Alg\n");
 int pageReplacements = 0;

 return pageReplacements;
}
int LRU_REF8(int frames,char *buffer){
 fprintf(stderr,"Entered LRU_REF8 Alg\n");
 int pageReplacements = 0;

 return pageReplacements;
}
int LRU_CLOCK(int frames,char *buffer){
 fprintf(stderr,"Entered LRU_CLOCK Alg\n");
 int pageReplacements = 0;

 return pageReplacements;
}

//Runtime algorithm
struct timeval start_time(){

struct timeval start;
gettimeofday(&start,NULL);
return start;


}
struct timeval stop_time(){

struct timeval stop;
gettimeofday(&stop,NULL);
return stop;

}

struct timeval total_time(struct timeval start,struct timeval stop){

struct timeval total;
total.tv_sec = (long double)(stop.tv_sec - start.tv_sec) * 1000.0;
total.tv_usec = (long double)(stop.tv_usec - start.tv_usec) /1000.0;
total.tv_sec = (long double)total.tv_sec + total.tv_usec;

return total;
}
void
print_time_ms(struct timeval t){

printf("%ldms\n",t.tv_sec);

}
