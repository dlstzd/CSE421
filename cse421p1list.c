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
	int SJF_ID;
    int minsize;
    
    
    if( ptr == NULL){
        printf("queue is empty! cannot find SJF");
        return 0;
    }
    SJF_ID = ptr->id; // sets shortest job id to head
    minsize = ptr->size; // sets minimun size to this
    ptr = ptr->next; // moves pointer
    if ( ptr->next == NULL){
        return ptr->id; //only job in queue
    }
	while (ptr != NULL) {
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
        *prev = tmp;
		
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
//removed one pointer to head in param
struct data pop_msg (struct data **head ){ //**head
    
    struct data request_message; // container
    struct data *tmp = *head;
    if (tmp == NULL){ return;}
   // int f = tmp.size;
    request_message.size = (*head)->size;
    strcpy(request_message.file_name, (*head)->file_name);
    strcpy(request_message.message, (*head)->message);
    printf("working so far");
    // copied all info into to be returned
    *head = tmp->next; // head is now head's next node
    free(tmp); // free memory space of old head.
    
    return request_message;
    
}

struct data get_message(int id, struct data **prev) {
	struct data *ptr = head;
	struct data *tmp = NULL;
	bool found = false;
    
    struct data request_message; // container for msg to be requested
    
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
        request_message.size = ptr->id;
        request_message.size = ptr->size;
        strcpy(request_message.file_name,ptr->file_name);
        strcpy(request_message.message,ptr->message);
		return request_message;
	}
	else {
	    strcpy(request_message.file_name, "error");
		return request_message;
	}
}
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

void print_list(void){
    struct data *ptr = head;
    while(ptr != NULL){
        //printf("HI");
        printf("%d, %d, %s, %s\n", ptr->id, ptr->size, ptr->file_name, ptr->message);
        ptr = ptr->next;
    }
    return;
}

int main(int argc, char** argv[]){
    struct data* hello;
    struct data* bye;
    struct data pop;
    int id = 1;
    int id2 = 3;
	int size = 5;
	int size2 = 6;
	time_t currTime;
    char file_name[IN_BUF] = "testerfilename";
    char file_name2[IN_BUF] = "second tester name";
	char message[BUF_LEN] = "PPPPPPPthis is a test message"; //is this large enough
    char message2[BUF_LEN] = "LJSFDONFDSOINFD";
    ///struct data * next;
    add_node(id, size, file_name, message);
    add_node(id2,size2,file_name2,message2);
    //printf("%d", hello->size);
    print_list();
    //pop = pop_msg((&head));
    //printf ( "%d \n", pop.id);
    return 0;
}