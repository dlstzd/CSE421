#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include <stdint.h>

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


struct node{

  int id; //number id assigned to node from input
  int freq; //number of times the number has occurred -- used for LFU
  int dist_to_next; //number of characters to go through in buffer until we find another occurence -- 200 is an arbitrary large number
  uint8_t ref;
  struct node *next;
};

struct node *head = NULL;
struct node *curr = NULL;

void print_list(void){
    struct node *ptr = head;
    //printf("\nThe list is:\n");
    while(ptr != NULL){
        //printf("%d, %d, \n", ptr->id, ptr->freq);
        ptr = ptr->next;
    }
    return;
}

struct node* create_list(){
  struct node *ptr = (struct node*)malloc(sizeof(struct node));
  if(ptr == NULL){
    //printf("Error creating node\n");
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
        //printf("id: %d   elem: %d\n", ptr->id, elem);
      found = true;
      break;
    }else{
      temp = ptr;
      ptr = ptr->next;
    }

  }
  if(found == true){ //increment count on node freq
    return true;
  }
  else{ //think about what to do here, replace element?
    return false;
  }

}

struct node* add_elem(){
  if(head == NULL){
    return (create_list());
  }
  struct node *ptr = (struct node*)malloc(sizeof(struct node));
  if(ptr == NULL){
    //printf("Error creating node\n");
    return NULL;
  }
  ptr->id = -1;
  ptr->freq = -1;
  ptr->next = NULL;

  curr->next = ptr;
  curr = ptr;

  return ptr;

}

void delete_list(){
  struct node *temp = (struct node*)malloc(sizeof(struct node));
  while(head != NULL){
    temp = head;
    head = temp->next;
    free(temp);
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

  printf("Number of frames is: %d\n", frames);
  printf("Replacement policy is %s\n", repPol);
  readNums(readflg, filename,frames,repPol);
  return 0;
}

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
     //printf("Page Queue is: %s",inBuf);

     //Call Page Replacement Algorithm on inBuf
     if(strcmp(repAlgorithm,"FIFO") == 0){
        start = start_time();
        //fprintf(stderr,"Attempting to call FIFO()\n");
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
        //fprintf(stderr,"Attempting to call LFU()\n");
        algPageReplacements = LFU(frames,inBuf);
        stop = stop_time();
        algTime = total_time(start,stop);
     }else if(strcmp(repAlgorithm,"LRU-STACK") == 0){
        start = start_time();
        //#########################
        // Make first pages stack
        //fprintf(stderr,"Making first pages into stack\n");
        //odd
        int limit = (frames * 2) - 2;
        int temp = (frames * 2) - 2;
        int i = 0;
        for(i;i<limit - 2;i++){
            char c1 = inBuf[i];
            char c2 = inBuf[temp];
            printf("c1 = %c\n",c1);
            printf("c2 = %c\n",c2);
            inBuf[i] = c2;
            inBuf[temp] = c1;
            temp = temp - 1;
        }
        //fprintf(stderr,"Page Queue is now :%s\n",inBuf);
        //##########################
        //fprintf(stderr,"Attempting to call LRU_STACK()\n");
        algPageReplacements = LRU_STACK(frames,buf);
        stop = stop_time();
        algTime = total_time(start,stop);
        //fprintf(stderr,"Attempting to call LRU_STACK()\n");
        algPageReplacements = LRU_STACK(frames,inBuf);
        stop = stop_time();
        algTime = total_time(start,stop);
     }else if(strcmp(repAlgorithm,"LRU-CLOCK") == 0){
        start = start_time();
        //fprintf(stderr,"Attempting to call LRU_CLOCK()\n");
        algPageReplacements = LRU_CLOCK(frames,inBuf);
        stop = stop_time();
        algTime = total_time(start,stop);
     }else if(strcmp(repAlgorithm,"LRU-REF8") == 0){
        start = start_time();
        //fprintf(stderr,"Attempting to call LRU_REF8()\n");
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

     //algorithm comparison

     algPenalty = (algPageReplacements - optimalPageReplacements);
     algPenalty = (algPenalty/optimalPageReplacements)*100;

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
     //printf("Reading from File\n");
     FILE *fp;
     fp = fopen(filename, "r");
     fgets(buf, 255, (FILE*)fp);
     //printf("Page Queue is : %s",buf);
     //Call Page Replacement Algorithm on buf

     int i = 0;
     while(i < frames){
       //will build our list of frames, add_elem sets default value for id and frequency to -1
       add_elem();
       i++;
     }
     i=0;
     //start off at the beginning(head) of the list
     curr=head;

     if(strcmp(repAlgorithm,"FIFO")== 0){
        start = start_time();
        //fprintf(stderr,"Attempting to call FIFO()\n");
        algPageReplacements = FIFO(buf);
        stop = stop_time();
        algTime = total_time(start,stop);
     }else if(strcmp(repAlgorithm,"LFU") == 0){
        start = start_time();
        //fprintf(stderr,"Attempting to call LFU()\n");
        algPageReplacements = LFU(frames,buf);
        stop = stop_time();
        algTime = total_time(start,stop);
     }else if(strcmp(repAlgorithm,"LRU-STACK")== 0){
        start = start_time();
        //#########################
        // Make first pages stack
        //fprintf(stderr,"Making first pages into stack\n");
        //odd
        int limit = (frames * 2) - 2;
        int temp = (frames * 2) - 2;
        int i = 0;
        for(i;i<limit - 2;i++){
            char c1 = buf[i];
            char c2 = buf[temp];
            //printf("c1 = %c\n",c1);
            //printf("c2 = %c\n",c2);
            buf[i] = c2;
            buf[temp] = c1;
            temp = temp - 1;
        }
        //(stderr,"Page Queue is now :%s\n",buf);
        //##########################
        //fprintf(stderr,"Attempting to call LRU_STACK()\n");
        algPageReplacements = LRU_STACK(frames,buf);
        stop = stop_time();
        algTime = total_time(start,stop);
        //fprintf(stderr,"Attempting to call LRU_STACK()\n");
        algPageReplacements = LRU_STACK(frames,buf);
        stop = stop_time();
        algTime = total_time(start,stop);
     }else if(strcmp(repAlgorithm,"LRU-CLOCK") == 0){
        start = start_time();
        //fprintf(stderr,"Attempting to call LRU_CLOCK()\n");
        algPageReplacements = LRU_CLOCK(frames,buf);
        stop = stop_time();
        algTime = total_time(start,stop);
     }else if(strcmp(repAlgorithm,"LRU-REF8") == 0){
        start = start_time();
        //fprintf(stderr,"Attempting to call LRU_REF8()\n");
        algPageReplacements = LRU_REF8(frames,buf);
        stop = stop_time();
        algTime = total_time(start,stop);
     }else{
        printf("Invalid Algorithm\n");
        exit(0);
     }
     //Run Optimal Algorithm
     //Clear list
     delete_list();
     while(i < frames){
       add_elem();
       i++;
     }
     curr=head;
     start = start_time();
     //fprintf(stderr,"Attempting to call OPTIMAL()\n");
     optimalPageReplacements = OPTIMAL(frames,buf);
     stop = stop_time();
     optimalTime = total_time(start,stop);

     algPenalty = (algPageReplacements - optimalPageReplacements);
     algPenalty = (algPenalty/optimalPageReplacements)*100;
     //printf("temp pen is: %.2f\n", algPenalty);

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
     printf("# page replacement penalty using %s: %.2f%%\n",repAlgorithm,algPenalty);
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

//TODO: is curr the right position to set
int OPTIMAL(int frames,char *buffer){
 //fprintf(stderr,"Entered OPTIMAL Alg\n");
 int pageReplacements = 0;

 char* futureBuf = buffer;
 int d=0;
 //print_list();
 for(d;d<strlen(buffer)-1;d++){
   if(buffer[d] == ' '){
     //printf("\n");
   }else{
     int val = atoi(&buffer[d]);

     struct node *ptr = (struct node*)malloc(sizeof(struct node));
     ptr = curr;
     if (ptr->id == -1){
       ptr->id = val;
       if(ptr->next == NULL){
	 curr = head;
       }
       else{ curr = ptr->next;}
     }
     if (find_elem(val)){
       //Do Nothing, do not need to count hits
       //printf("\nElement found\n");
     }else{
       //printf("OPT REPLACE CASE\nValue is : %d\n", val);
       //print_list();
       //need a copy of the buffer to go through until next occurrence of id
       int hold = ptr->id;

       int largestID = ptr->id;
       //printf("LARGEST IS: %d\n", largestID);
       int largest = 0;
       int t = d;
       for(t; t<strlen(futureBuf)-1; t++){
	 if(futureBuf[t] == ' '){

	 }else{
	   //ptr->freq = -1;
	   int bar = atoi(&futureBuf[t]);

	   if(bar == ptr->id){
	     break;
	   }else{
	     ptr->freq++;
	   }
	 }

       }
       largest = ptr->freq;
       //printf("LDR: %d\n", largest);

       if(ptr->next == NULL){
        ptr=head;
       }else{
	 ptr = ptr->next;
       }
       while(ptr->id != hold){
	 int i = d;
	 for(i; i<strlen(futureBuf)-1; i++){

	   if(futureBuf[i] == ' '){
	   }else{
	     int temp = atoi(&futureBuf[i]);
	     if(temp == ptr->id){
	       break;
	     }else{
	       ptr->freq++; //here freq refers to the distance to next occurence of id in futureBuf
	     }
	   }
	 }
	 if(ptr->freq > largest){
	   largest = ptr->freq;
	   largestID = ptr->id;
	 }
	 if(ptr->next == NULL){
	   ptr=head;
	   ptr->freq = -1;
	 }else{
	   ptr = ptr->next;
	   ptr->freq = -1;
	 }
       }
       //printf("ID of element with largest distance is: %d\n distance to next is: %d", largestID, largest);
       while(ptr->id != largestID){
	 if(ptr->next == NULL){
	   ptr=head;
	 }else{
	   ptr = ptr->next;
	 }
       }


       pageReplacements++;
       ptr->id = val;
       ptr->freq = -1;
       if(ptr->next == NULL){
           curr = head;
       }
       else{ curr = ptr->next; }
       print_list();
     }
   }
 }
 return pageReplacements;
}

int FIFO(char *buffer){
 //fprintf(stderr,"Entered FIFO Alg\n");
 int pageReplacements = 0;

 int d = 0;
 //printf("BUF: %s", buffer);
 for(d;d<strlen(buffer)-1;d++){
    //Set val to the next character in buffer , convert to integer.
   if(buffer[d] == ' '){
     //printf("\n");
   }
   else{
    int val = atoi(&buffer[d]);
    //printf("FIFO replace\n");

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
        //printf("Element found\n");
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
 return pageReplacements;
}


//Least Frequently Used Algorithm
int LFU(int frames,char *buffer){
 int pageReplacements = 0;
 //fprintf(stderr,"Entered LFU Alg\n");
 int d = 0;
 int temp=0;
 //printf("BUF: %s", buffer);

 for(d;d<strlen(buffer)-1;d++){
   //used to ignore space characters
   if(buffer[d] == ' '){
     //printf("Space detected\n");
     //d++;
   }
   //we have some number do the following
   else{
    int val = atoi(&buffer[d]);

    /*create a node struct which will be
     used to go through our already built list (this was done around line 246)
     Three options for every number
     1) There is no set value for our node we can just replace any id with -1
     2) The current element in the buffer is already in the list
     3) The current element in the buffer is not in the list
     */
    struct node *ptr = (struct node*)malloc(sizeof(struct node));
    ptr = curr;
    //Case 1: node is "empty", just replace the id and frequency
    if (ptr->id == -1){
        ptr->id = val;
	ptr->freq = 0;
	//if we are at the end of our list go back to the head
        if(ptr->next == NULL){
            curr = head;
        }
	//otherwise go to the next node in the list
        else{ curr = ptr ->next; }
    }
    //Case 2: our current buffer element is found in the list -- determined by find_elem() line 76
    if(find_elem(val)){
      /*copy the id of the current id this has to be done in order to keep track of where
       we are in the list, find_elem() just returns true if it finds the id but does not keep track
       of current position in our list
       */
      int temp = ptr->id;
      while(ptr->id != val){
	//same travel through the list as before
	if(ptr->next==NULL){
	  ptr = head;}
	else{
	ptr= ptr->next;
	}
      }
      //at this point we found the right id, increment its frequency
      ptr->freq++;

      /*NOTE: I'm not too sure what the use of this is, I ran this with and
       without the following code and the results seem to be the same
       */
      //FROM HERE
      //set the ptr to be the next

      while(ptr->id != temp){
	if(ptr->next==NULL){
	  ptr=head;
	}
	else{
	ptr = ptr->next;
	}
      }
        //printf("Element found\n");
	//TO HERE
    }
    //Case 3: current buffer element was not found in the list of frames
    else{
       //Increment Page Replacement Counter.
      //ntemp helps us loop through until we find our id again
      //smallestID will be the id to be replaced
      int ntemp = ptr->id;
      int smallest = ptr->freq;
      int smallestID = ptr->id;
      //printf("smallest ids : %d %d \n\n" , smallestID,smallest);
      //same travel through list as before
      if(ptr->next == NULL){
	ptr=head;
      }

      else{
	ptr = ptr->next;
      }
      //loop through the entire list and check for the smallest frequency of each id,
	while(ptr->id != ntemp){
	  if(ptr->freq < smallest){
	    smallest = ptr->freq;
	    smallestID = ptr->id;
	  }
	  //same travel through list
	  if(ptr->next == NULL){
	    ptr = head;
	  }else{
	    ptr = ptr->next;
	  }
	}
	//smallestID has now been determined go through list until we find the correct id,
      while(ptr->id != smallestID){
	if(ptr->next == NULL){
	  ptr = head;
	}else{
	  ptr= ptr->next;
	}
       }

      //replace the element move the pointer to the next position in the list
      pageReplacements++;
       ptr->id = val;
       ptr->freq = 1;
        if(ptr->next == NULL){
            curr = head;
        }
        else{ curr = ptr->next; }
    }
   }
 }
 //print_list();
 return pageReplacements;
}

int LRU_STACK(int frames,char *buffer){
 int pageReplacements = 0;
 //fprintf(stderr,"Entered LFU Alg\n");
 int d = 0;
 int temp=0;
 int page_buf[frames];
 //printf("BUF: %s", buffer);

 for(d;d<strlen(buffer)-1;d++){
   //used to ignore space characters
   if(buffer[d] == ' '){
     //printf("Space detected\n");
     //d++;
   }
   //we have some number do the following
   else{
    int val = atoi(&buffer[d]);

    /*create a node struct which will be
     used to go through our already built list (this was done around line 246)
     Three options for every number
     1) There is no set value for our node we can just replace any id with -1
     2) The current element in the buffer is already in the list
     3) The current element in the buffer is not in the list
     */
    struct node *ptr = (struct node*)malloc(sizeof(struct node));
    ptr = curr;
    //Case 1: node is "empty", just replace the id and frequency
    if (ptr->id == -1){
        ptr->id = val;
	ptr->freq = 0;
	//if we are at the end of our list go back to the head
        if(ptr->next == NULL){
            curr = head;
        }
	//otherwise go to the next node in the list
        else{ curr = ptr ->next; }
    }
    //Case 2: our current buffer element is found in the list -- determined by find_elem() line 76
    else if(find_elem(val)){
      /*copy the id of the current id this has to be done in order to keep track of where
       we are in the list, find_elem() just returns true if it finds the id but does not keep track
       of current position in our list
       */
      int temp = ptr->id;
      while(ptr->id!=val){
            if(ptr->next==NULL){
                ptr = head;
            }else{
                ptr = ptr->next;
            }
        }



    //Save value to skip over
        temp = ptr->id;
        //printf("Element found\n");
    //curr = head;
    //Iterate through list
    //Add id to array
    //If id = temp skip because it has already been added
        page_buf[0] = ptr->id;
    //set pointer to head of list
        //ptr = head;
        ptr = head;
        int j = 1;
        while(ptr->next != NULL){
            if(ptr->id == temp){
            ptr = ptr->next;
            }else{
            page_buf[j] = ptr->id;
            ptr = ptr->next;
            j++;
            }
        }
        page_buf[j] = ptr->id;

        int z = 0;
        //printf("page_buf\n");
        for(z;z<frames;z++){
            //printf("%i\n",page_buf[z]);
        }

        j=0;
        ptr = head;
        while(ptr->next != NULL){
        ptr->id = page_buf[j];
        j++;
        ptr = ptr->next;
        }
        ptr->id = page_buf[j];
        //print_list();
        //printf("Element found\n");
	//TO HERE
    }
    //Case 3: current buffer element was not found in the list of frames
    else{
       //Increment Page Replacement Counte
      int last = ptr->id;
      int j=1;
      while(ptr->next!=NULL){
        ptr = ptr->next;
      }
      last = ptr->id;
      //printf("last is %d\n", last);
      page_buf[0]=val;
      ptr = head;
      while(ptr->id != last){
            if(ptr->id == val){
            ptr = ptr->next;
            }else{
            page_buf[j] = ptr->id;
            ptr = ptr->next;
            j++;
            }
        }
        int z = 0;
        //printf("page_buf\n");
        for(z;z<frames;z++){
            //printf("%i\n",page_buf[z]);
        }

        j=0;
        ptr = head;
        while(ptr->next != NULL){
        ptr->id = page_buf[j];
        j++;
        ptr = ptr->next;
        }
        ptr->id = page_buf[j];
        //print_list();


      //replace the element move the pointer to the next position in the list
      pageReplacements++;

    }
   }
 }
 print_list();
 return pageReplacements;
}
int LRU_REF8(int frames,char *buffer){
 int pageReplacements = 0;
 //fprintf(stderr,"Entered REF8 Alg\n");
 int d = 0;
 int temp=0;
 //printf("BUF: %s", buffer);

 for(d;d<strlen(buffer)-1;d++){
   //used to ignore space characters
   if(buffer[d] == ' '){
    // printf("Space detected\n");
     //d++;
   }
   //we have some number do the following
   else{
    int val = atoi(&buffer[d]);

    struct node *ptr = (struct node*)malloc(sizeof(struct node));
    ptr = curr;
    //Case 1: node is "empty", just replace the id and frequency
    if (ptr->id == -1){
        ptr->id = val;
	ptr->ref = 00000000;
	//if we are at the end of our list go back to the head
        if(ptr->next == NULL){
            curr = head;
        }
	//otherwise go to the next node in the list
        else{ curr = ptr ->next; }
    }
    //Case 2: our current buffer element is found in the list -- determined by find_elem() line 76
    if(find_elem(val)){

      int temp = ptr->id;

      while(ptr->id != val){
	//same travel through the list as before
	if(ptr->next==NULL){
	  ptr = head;}
	else{
	ptr= ptr->next;
	}
      }

      ptr->ref = ptr->ref >> 1;
      //printf("REF bit %u\n", ptr->ref);
      ptr->ref = ptr->ref | 10000000;

      //printf("REF bit %u\n", ptr->ref);
      //print_list();

      while(ptr->id != temp){
	if(ptr->next==NULL){
	  ptr = head;
	}
	else{
	  ptr = ptr->next;
	}
      }
      if(ptr->next == NULL){
	ptr = head;
	ptr->ref = ptr->ref >> 1;
      } else{
	ptr = ptr->next;
	ptr->ref = ptr->ref >> 1;
      }
     while(ptr->id != temp){
	if(ptr->next == NULL){
	  ptr = head;
	  ptr->ref = ptr->ref >> 1;
	}else{
	  ptr = ptr->next;
	  ptr->ref = ptr->ref >> 1;
	}
      }

        //printf("Element found\n");
	//TO HERE
    }
    //Case 3: current buffer element was not found in the list of frames
    else if(!find_elem(val)){

      int ntemp = ptr->id;
     // printf("ntemp: %d\n", ntemp);
      uint8_t smallest = ptr->ref;
      int smallestID = ptr->id;
      //printf("smallest ids : %d %d \n\n" , smallestID,smallest);
      //same travel through list as before
      if(ptr->next == NULL){
	ptr=head;
      }

      else{
	ptr = ptr->next;
      }
      //loop through the entire list and check for the smallest frequency of each id,
	while(ptr->id != ntemp){
	  if(ptr->ref < smallest){
	    smallest = ptr->ref;
	    smallestID = ptr->id;
	  }
	  //same travel through list
	  if(ptr->next == NULL){
	    ptr = head;
	  }else{
	    ptr = ptr->next;
	  }
	}

	//smallestID has now been determined go through list until we find the correct id,
      while(ptr->id != smallestID){
	if(ptr->next == NULL){
	  ptr = head;
	}else{
	  ptr= ptr->next;
	}
       }
      //ptr = ptr->next;
      ptr->ref = ptr->ref >> 1;
      while(ptr->id != smallestID){
	if(ptr->next == NULL){
	  ptr = head;
	  ptr->ref = ptr->ref >> 1;
	}else{
	  ptr = ptr->next;
	  ptr->ref = ptr->ref >> 1;
	}
      }


      //print_list();

      //replace the element move the pointer to the next position in the list
      pageReplacements++;
       ptr->id = val;
       ptr->ref = 00000000;
       ptr->ref = ptr->ref | 10000000;
        if(ptr->next == NULL){
            curr = head;
        }
        else{ curr = ptr->next; }
    }
   }
 }
 return pageReplacements;
}
int LRU_CLOCK(int frames,char *buffer){
int pageReplacements = 0;
 //fprintf(stderr,"Entered LFU Alg\n");
 int d = 0;
 int temp=0;
 //printf("BUF: %s", buffer);

 for(d;d<strlen(buffer)-1;d++){
   //used to ignore space characters
   if(buffer[d] == ' '){
     //printf("Space detected\n");

   }
   //we have some number do the following
   else{
    int val = atoi(&buffer[d]);

    struct node *ptr = (struct node*)malloc(sizeof(struct node));
    ptr = curr;

    if(find_elem(val)){

      int temp = ptr->id;
      while(ptr->id != val){
	if(ptr->next==NULL){
	  ptr = head;}
	else{
	ptr= ptr->next;
	}
      }

      ptr->freq = 1;



      while(ptr->id != temp){
	if(ptr->next==NULL){
	  ptr=head;
	}
	else{
	ptr = ptr->next;
	}
      }
        //printf("Element found\n");
	//printf("pter is at : %d \n" , ptr->id);
	//print_list();
    }
    if (ptr->id == -1){
        ptr->id = val;
	ptr->freq = 0;
        if(ptr->next == NULL){
            curr = head;
        }
        else{ curr = ptr ->next; }
	//print_list();
    }
    else if( !find_elem(val)){
      	//printf("pter is at : %d \n" , ptr->id);
      //printf("repalcement case\n");
      int ntemp = ptr->id;

      if(ptr->freq == 0){
	ptr->id = val;
	pageReplacements++;
	if(ptr->next == NULL){
	  curr = head;
	}else{
	  curr = ptr->next;
	}
      }

      if(ptr->freq == 1){
	ptr->freq = 0;
	if(ptr->next == NULL){
	  ptr = head;
	}else{
	  ptr = ptr->next;
	}
	while(ptr->id != ntemp){
	  if(ptr->freq == 0){
	    ptr->id = val;
	    pageReplacements++;
	    if(ptr->next == NULL){
	      curr = head;
	      break;
	    }else {
	      curr = ptr->next;
	      break;
	    }
	  }else if(ptr->freq == 1){
	    ptr->freq = 0;
	    if(ptr->next == NULL){
	      ptr = head;
	    }else{
	      ptr = ptr->next;
	    }
	  }
	}
      }

    }
    //print_list();
   }
 }
 //print_list();
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
