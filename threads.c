void *scheduler(int sched_flag){
    struct data msg_to_be_schedueled;
    int flag = sched_flag;  // we need sched flag to know which algo to use, defualt FIFO is easy, just pop()! default = 0 =  FIFO
  //  int thread_avaiable ; // this should keep track of
    //threads avaible to work, we dont want to send work if no thread is open

    //move pthread lock above if statements
    while(1){
        if(flag==0){
			pthread_mutex_lock(&schedMutex);
            if(front ==  NULL){ // we have empty queue
                printf("Scheduleing for FIFO failed, queue is empty!");
            }else {
            // some mutex to lock popping the queue
            pthread_mutex_lock(&queLock);
            msg_to_be_schedueled =pop_msg(*head);
            // we aqcuired msg to be scheduled
            pthread_mutex_unlock(&queLock);
            // unlock mutex , we've popped queue
            // signal worker thread to do work on this message
            pthread_cond_signal(&worker);
            sleep(100); // make sure that one worker only gets one job
            printf("after FIFO , new queue is now \n");
            print_list();
            pthread_mutex_unlock(&schedMutex);

                 }

            }
        else(flag == 1){  //SJF
			pthread_mutex_lock(&schedMutex);
            struct data shortest;
            int sjf_id;

            if (front == NULL){
                print("Scheduleing for SJF failed, queue is empty!");
            }
            else{
                //some mutex
             pthread_mutex_lock(&queLock);
             sjf_id = find_shortest_job(&head); // find shortest job by job size
             shortest = get_message(sfj_id,&head);      // set shortest to this job
             pthread_mutex_unlock(&queLok);//unlock queue lock
             pthread_cond_signal(&worker); // signal avaible worker



             sleep(100);
            }
			pthread_mutex_unlock(&schedMutex);


        }
    }
}

void *listener(int s){
    int newsock;
    struct sockaddr_in serv, remote;
    len = sizeof(remote);
    listen(s, 1);
    while(1){


        if (soctype == SOCK_STREAM) {
            fprintf(stderr, "Entering accept() waiting for connection.\n");
            fprintf(stderr, "Waiting For Connection... \n");
            //Accept First connections


            newsock = accept(s, (struct sockaddr *) &remote, &len);
            if(newsock < 0){
                printf("error connecting to client");
            }
            else{
            fprintf(stderr, "New Client Connected \n");
            }

            char timebuf[128];
            time_t rawtime;
            struct tm * timeinfo;
            time ( &rawtime );
            timeinfo = localtime ( &rawtime );
            strftime(tmpbuf,sizeof(tmpbuf),"%d/%b/%Y : %H:%M:%S %z",timeinfo);
            printf("the time is %s ", tmpbuf);




        }
    }
}


void *worker(void){

    char *buffer[BUF_LEN];
    struct data *msg;

    /*
    mutex lock
    wait for signal from scheduler
    this msg = msg_to_be_schedueled
    unlock

    ready msg ...
    get time again
    do post or head ...
    send back info ...
    if debug -> log
    if not no log
    keep running.
    */

    pthread_mutex_lock(&worker_lock);
    pthread_cond_wait(&worker);
    //Need to set msg to the current job, not sure how?
    //msg =
    pthread_mutex_unlock(&worker_lock);
    buffer = msg.message;
    //do GET or HEAD
    char msgType = buffer[0];
    switch(msgType){
        case'G':
        {
            GET_response();
            //Logging/Debug
            /*
            %a : The remote IP address.
            %t : The time the request was received by the queuing thread (in GMT).
            %t : The time the request was assigned to an execution thread by the scheduler (in GMT).
            %r : The (quoted) first line of the request.
            %>s : The status of the request.
            %b : Size of the response in bytes. i.e, "Content-Length".

            Example:
            127.0.0.1 - [19/Sep/2011:13:55:36 -0600] [19/Sep/2011:13:58:21 -0600]
            "GET /index.html HTTP/1.0" 200 326
            */
            char log_buffer[2048];
            sprintf(log_buffer,"%s - [%i/%s/%i:%i:%i:%i %i] [%i/%s/%i:%i:%i:%i %i] \"%s\" 200 %i",
                NOTSET,zero,NOTSET,zero,zero,zero,zero,zero,zero,NOTSET,zero,zero,zero,zero,zero,NOTSET,zero);
            writeLog(log_buffer);
            writeDebug(log_buffer);

        }
        break;
        case'H':
        {
            HEAD_response();
            /*
            %a : The remote IP address.
            %t : The time the request was received by the queuing thread (in GMT).
            %t : The time the request was assigned to an execution thread by the scheduler (in GMT).
            %r : The (quoted) first line of the request.
            %>s : The status of the request.
            %b : Size of the response in bytes. i.e, "Content-Length".

            Example:
            127.0.0.1 - [19/Sep/2011:13:55:36 -0600] [19/Sep/2011:13:58:21 -0600]
            "GET /index.html HTTP/1.0" 200 326
            */


            //WriteLog and WriteDebug will do the proper implementation based
            //on the flags being set when the program is run
            //This is already implemented in the methods.
            char *log_buffer;
            sprintf(log_buffer,"%s - [%i/%s/%i:%i:%i:%i %i] [%i/%s/%i:%i:%i:%i %i] \"%s\" 200 %i",
                    NOTSET,zero,NOTSET,zero,zero,zero,zero,zero,zero,NOTSET,zero,zero,zero,zero,zero,NOTSET,zero);
            writeLog(log_buffer);
            writeDebug(log_buffer);
        }
        break;
        default:
        {
            write(sock,invalid_request,sizeof(invalid_request)-1);
        }
        }
}



int main(int argc, char** argv[]){
// set up socket here

sock = new sock ... etc

// get arg flags
for (0 to get_thread_num_from_arg){
    pthread_create ( worker threads );
}


pthread_create ( listener*) ... // listener arg would be SOCK
sleepfor some time
pthread_create ( scheuduler ) ... // let it run and schedule
// at this time we inserted msg's into queue
// waited for some msgs'some
// scheduled and signaling avaiable worker threads .


pthread_join (listener)
pthread_join ( scheduler)
pthread_join ( worker)
kill socket
display socket













    return 0;

}
