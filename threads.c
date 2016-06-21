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
            
            
            
            
            i
            
            
            
            
        }
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
    }
}