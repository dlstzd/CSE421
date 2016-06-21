#define IN_BUF 2048
#define	BUF_LEN	8192
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
#include <stdbool.h>


struct data {
	/*
	Other bits of data to include here?
	time, ipaddress ... get from server or set up
	*/
  int id;
  int size;
  time_t currTime;
  char file_name[IN_BUF];
  char message[BUF_LEN]; //is this large enough
  struct data * next;
};


struct data *head = NULL;
struct data *curr = NULL;

//struct data **queue = NULL;

struct data* get_head(){
    return head;
}
struct data* get_tail(){
    return curr;
}
void print_list(void){
    struct data *ptr = head;
    while(ptr != NULL){
        printf("%d, %d, %s, %s\n", ptr->id, ptr->size, ptr->file_name, ptr->message);
        ptr = ptr->next;
    }
    return;
}

struct data* create_list(int id,int size, char* file_name, char* message){
    printf("\n Creating new list with head node message %s \n", message);
    
    struct data *ptr = (struct data*)malloc(sizeof(struct data));
    
    if (NULL == ptr){
        printf("\n List creation failed");
    }
    char file[IN_BUF];
    char msg[IN_BUF];
    strcpy(file,file_name);
    strcpy(msg,message);
    // sets node data struct to these
    ptr->id= id;
    ptr->size =  size;
    strcpy(ptr->file_name, file);
    strcpy(ptr->message, msg);
    ptr->next = NULL;
    
    head = curr = ptr;
    //queue = &head;
    
    return ptr;
    
    
}

int find_shortest_job(struct data **prev){
    
    struct data *ptr = head;
    struct data *tmp = NULL;
    struct data *holder = head;
    int SJF_ID;
    int minsize;
    char* buf;
    
    if( ptr == NULL){
        printf("queue is empty! cannot find SJF");
        return 0;
    }
    SJF_ID = ptr->id; // sets shortest job id to head
    minsize = ptr->size; // sets minimun size to this
    //printf("Printlist function follows\n");

    ptr = ptr->next; // moves pointer
    if ( ptr->next == NULL){
        return ptr->id; //only job in queue
    }
    while (ptr != NULL) {
      //printf("IN SJF: minsize is: %d\n", minsize);
 
      if (ptr->size < minsize ) { 
	minsize = ptr->size;
        SJF_ID =  ptr->id;
        tmp = ptr;
        ptr = ptr->next;
      }
      else {

	tmp = ptr;
	ptr = ptr->next;
      }
    }

    if (prev) {
      //*prev = tmp;
      *prev = holder;
      //print_list();		
      return  SJF_ID;
    }
    else {
      return 0;
    }

}

struct data* add_node(int id,int size, char* file_name, char* message) {
	if (head == NULL) {
		return(create_list(id,size,file_name,message));
	}
    printf("\n adding new msgnode with msg [%s] \n ", message);
	struct data *ptr = (struct data*)malloc(sizeof(struct data));
	if (ptr == NULL) {
		printf("MsgNode build failure");
		return NULL;
	}
	
    char file[IN_BUF];
    char msg[IN_BUF];
    strcpy(file,file_name);
    strcpy(msg,message);
    // sets node data struct to these
    ptr->id = id;
    ptr->size =  size;
    strcpy(ptr->file_name, file);
    strcpy(ptr->message, msg);
    
	ptr->next = NULL;
    // assume we only add to end of queue
	curr->next = ptr;
	curr = ptr;
	return ptr;
}

struct data pop_msg (struct data **head ){ //**head
    
    struct data request_message; // container
    //struct data *tmp = *head;
    struct data *tmp = NULL;
    //if (tmp == NULL){ return;}
    if(*head == NULL){return;}
    tmp = (*head)->next;
    request_message.size = (*head)->size;
    request_message.id = (*head)->id;
    strcpy(request_message.file_name, (*head)->file_name);
    strcpy(request_message.message, (*head)->message);

    // copied all info into to be returned
    free(*head);
    *head = tmp;
    /*
    *head = tmp->next; // head is now head's next node
    free(tmp); // free memory space of old head.
    */
    printf("IN POP: id is: %d\n", request_message.id);
    return request_message;
    
}

struct data get_message(int id, struct data** head){ //**prev) {
  //struct data *ptr = head;
  //struct data *tmp = NULL;
  struct data *prev, *curr;
  bool found = false;
  //printf("IN GET: id is %d\n", ptr->id);
  struct data request_message; // container for msg to be requested
  if(*head == NULL){
    strcpy(request_message.file_name, "ERROR");
  }
  if((*head)->id == id){
    request_message =  pop_msg(head);
  }
  prev = curr = (*head)->next;
  while(curr){
    if(curr->id == id){
      prev->next = curr->next;
      request_message.id = curr->id;
      request_message.size = curr->size;
      strcpy(request_message.file_name, curr->file_name);
      strcpy(request_message.message, curr->message);
      printf("IN GET: message id is: %d, size is %d\n", request_message.id, request_message.size);
      free(curr);
      return request_message;
      
    }
    prev = curr;
    curr = curr->next;
  }
	
	//error seems to be related to the current element of tmp
	/*
	while (ptr != NULL) {
	  if (ptr->id == id) {
	    found = true;
	    break;
	  }
	  else {
	    tmp = ptr;
	    ptr = ptr->next;
	  }
	}
	if (found == true) {   //found the message
	  if (prev) {
	    *prev = tmp;
	  }
	  printf("IN GET: ptr id is: %d, tmp id is %d\n", ptr->id, tmp->id);
	  request_message.id = ptr->id;
	  request_message.size = ptr->size;
	  strcpy(request_message.file_name,ptr->file_name);
	  strcpy(request_message.message,ptr->message);
	  //printf("IN GET MESSAGE: message id is: %d, size is:%d\n", request_message.id, request_message.size);
	  return request_message;
	}
	else {
	  strcpy(request_message.file_name, "error");
	  return request_message;
	}
	*/
}
/*
int delete_from_list(int val) {
	struct data *prev = NULL;
	struct data *del = NULL;

	*del = get_message(val, &prev);
	if (del == NULL) {
		return -1;
	}
	else {
		if (prev != NULL) {
			prev->next = del->next;
		}
		if (del == curr) {
			curr = prev;
		}
		else if (del == head) {
			head = del->next;
		}
	}
	free(del);
	del = NULL;

	return 0;
}
*/
/*
void delete_from_list(int val){
  struct data *prev;
  struct data *curr;
  struct data holder;
  if(*head == NULL){
    printf("No available node")
  }
  if((*head)->id == val){
    //pop front
  }
  prev = curr = (*head)->next;
  while(curr){
    if(curr->id == val){
      prev->next = curr->next;
      free(curr);
      return val;
    }
  }
}
*/
int main(int argc, char** argv[]){
    
    struct data* hello;
    struct data* bye;
    struct data pop;
    int min;
    //char buf[BUF_LEN];

    ///struct data * next;
    add_node(1, 360, "filename1", "this is a message");
    add_node(2, 120, "Filename2", "this IS ANOTHER MESSAGE");
    add_node(3, 44, "FILENAME3", "PPPPPP ONE MORE MESSAGE");
    add_node(4, 22, "Filename4", "AAAAAAAAAAAAA");
    add_node(5, 330, "Filename5", "BSNSKSJKO");
    print_list();
    min = find_shortest_job(&head);
    printf("shortest job id is: %d\n", min);
 
    pop = get_message(min, &head);
    //printf("Shortest job is %d, The message is: %s\n", pop.id, pop.message);
    //pop = pop_msg(&head);
    print_list();
    
  
			 
    return 0;
}
