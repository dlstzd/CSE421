/*
 * soc.c - program to open sockets to remote machines
 *
 * $Author: kensmith $
 * $Id: soc.c 6 2009-07-03 03:18:54Z kensmith $
 */

static char svnid[] = "$Id: soc.c 6 2009-07-03 03:18:54Z kensmith $";

#define	BUF_LEN	8192
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netdb.h>
#include	<netinet/in.h>
#include	<inttypes.h>

#include    <pthread.h>
#include    <semaphore.h>
#include    <fcntl.h>

char *progname;
char buf[BUF_LEN];

void help();
void usage();
int setup_client();
int setup_server();

int s, sock, ch, server, done, bytes, aflg;
int soctype = SOCK_STREAM;
char *host = NULL;

extern char *optarg;
extern int optind;

char *file = NULL;
char *port = NULL;
char *dir = NULL;
char *schedtime = NULL;
char *threadnum = NULL;
char *schedtype = NULL;
int debug;

int accept_pid;
int pid;



int
main(int argc,char *argv[])
{
    server = 1;
	fd_set ready;
	struct sockaddr_in msgfrom;
	int msgsize;
	union {
		uint32_t addr;
		char bytes[4];
	} fromaddr;

	/*
        Set Program Defaults before getting arguments

        Notes: Mitcht
        I changed the original program parameters to what our server needs.
        So far this is functioning correctly

        I removed the client code mainly because we just don't need it
        since testing we need to use the following

        telnet timberlake.cse.buffalo.edu Port

        This does the same thing as the client did.

        The program will run without any parameters or any number of them.
        Compile the same way that was shown on piaza.

	*/

	port = "8080";
	schedtime = "60";
	threadnum = "4";
	schedtype = "FCFS";
	debug = 0;

	if ((progname = rindex(argv[0], '/')) == NULL)
		progname = argv[0];
	else
		progname++;
	while ((ch = getopt(argc, argv, "dhl:p:r:t:n:s:")) != -1)
		switch(ch) {
			case 'd':
			    //Set Debug flag to 1, this will force program to only allow one client at a time
			    debug = 1;
				break;
			case 'h':
			    help();
				break;
			case 'l':
			    file = optarg;
				break;
			case 'p':
                port = optarg;
				break;
			case 'r':
			    dir = optarg;
                break;
            case 't':
                schedtime = optarg;
                break;
            case 'n':
			    threadnum = optarg;
			    break;
            case 's':
                schedtype = optarg;
                break;
			default:
				usage();
		}
	argc -= optind;
	if (argc != 0)
		usage();
	if(dir != NULL){
		chdir(dir);
	}

/*
 * Create socket on Server.

 Notes: Mitcht - Im not sure if we need multithreading here.
 This is where we create a socket on the server and actually setup the server.

 */

 //Setup server listening on port and waiting for a connection
 //Once Connection is obtained fork process and child process will handle request
 //Main process continues to listen





    s = socket(AF_INET,soctype,0);
    if(s < 0){
        perror("socket");
        exit(1);
    }else{
        sock = setup_server();
    }

/*
 * Set up select(2) on both socket and terminal, anything that comes
 * in on socket goes to terminal, anything that gets typed on terminal
 * goes out socket...


 Notes by Mitcht:

    I think we need to implement a form of multi-threading here. Currently , if you
    start the server, then telnet on one prompt and telnet on another , only the first
    one is able to send and rcv to the server. The other connects, but cannot do anything.

 */

	while (!done) {
		FD_ZERO(&ready);
		FD_SET(sock, &ready);
		FD_SET(fileno(stdin), &ready);


        //Logging
		if(!debug && file != NULL){
            //Print log to file
		}else if(debug && file !=NULL){
            //Print to screen and to file
		}else if(debug){
            //Only print to screen
		}else{
            //No Logging
		}

		if (select((sock + 1), &ready, 0, 0, 0) < 0) {
			perror("select");
			exit(1);
		}
		//Sends information from server to client
		if (FD_ISSET(fileno(stdin), &ready)) {
			if ((bytes = read(fileno(stdin), buf, BUF_LEN)) <= 0)
				done++;
			send(sock, buf, bytes, 0);

		}

        /*
            Receives message from client
            buf is the actual message
            BUF_LEN is length of message
        */
		msgsize = sizeof(msgfrom);
		if (FD_ISSET(sock, &ready)) {
			if ((bytes = recvfrom(sock, buf, BUF_LEN, 0, (struct sockaddr *)&msgfrom, &msgsize)) <= 0) {
				done++;
			} else if (aflg) {
				fromaddr.addr = ntohl(msgfrom.sin_addr.s_addr);
				fprintf(stderr, "%d.%d.%d.%d: ", 0xff & (unsigned int)fromaddr.bytes[0],
			    	0xff & (unsigned int)fromaddr.bytes[1],
			    	0xff & (unsigned int)fromaddr.bytes[2],
			    	0xff & (unsigned int)fromaddr.bytes[3]);
			}


			/*
                Determine if a GET or POST
                We then need to send the information back to client using the following
                buf would be the actual data we are sending back
                send(sock, buf, bytes, 0);
			*/
            char msgType = buf[0];
            switch(msgType){
                case'G':
                fprintf(stderr,"GET request \n");
                //parse_GET(buf);
                break;
                case'P':
                fprintf(stderr,"POST request \n");
                //parse_POST(buf);
                break;
            }

            //Displays output of what was typed on clients end
            write(fileno(stdout), buf, bytes);
		}
	}
	return(0);
}

/*
 * setup_client() - set up socket for the mode of soc running as a
 *		client connecting to a port on a remote machine.
 */

int
setup_client() {

	struct hostent *hp, *gethostbyname();
	struct sockaddr_in serv;
	struct servent *se;

/*
 * Look up name of remote machine, getting its address.
 */
	if ((hp = gethostbyname(host)) == NULL) {
		fprintf(stderr, "%s: %s unknown host\n", progname, host);
		exit(1);
	}
/*
 * Set up the information needed for the socket to be bound to a socket on
 * a remote host.  Needs address family to use, the address of the remote
 * host (obtained above), and the port on the remote host to connect to.
 */
	serv.sin_family = AF_INET;
	memcpy(&serv.sin_addr, hp->h_addr, hp->h_length);
	if (isdigit(*port))
		serv.sin_port = htons(atoi(port));
	else {
		if ((se = getservbyname(port, (char *)NULL)) < (struct servent *) 0) {
			perror(port);
			exit(1);
		}
		serv.sin_port = se->s_port;
	}
/*
 * Try to connect the sockets...
 */
	if (connect(s, (struct sockaddr *) &serv, sizeof(serv)) < 0) {
		perror("connect");
		exit(1);
	} else
		fprintf(stderr, "Connected...\n");
	return(s);
}

/*
 * setup_server() - set up socket for mode of soc running as a server.
 */

int
setup_server() {
	struct sockaddr_in serv, remote;
	struct servent *se;
	int newsock, len;

	len = sizeof(remote);
	memset((void *)&serv, 0, sizeof(serv));
	serv.sin_family = AF_INET;
	if (port == NULL)
		serv.sin_port = htons(0);
	else if (isdigit(*port))
		serv.sin_port = htons(atoi(port));
	else {
		if ((se = getservbyname(port, (char *)NULL)) < (struct servent *) 0) {
			perror(port);
			exit(1);
		}
		serv.sin_port = se->s_port;
	}
	if (bind(s, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
		perror("bind");
		exit(1);
	}
	if (getsockname(s, (struct sockaddr *) &remote, &len) < 0) {
		perror("getsockname");
		exit(1);
	}
	fprintf(stderr, "Port number is %d\n", ntohs(remote.sin_port));

	//Maybe Start forking here
	listen(s, 1);
	newsock = s;
	if (soctype == SOCK_STREAM) {
		fprintf(stderr, "Entering accept() waiting for connection.\n");
		fprintf(stderr, "Waiting For Connection... \n");
		//Get PID of first process
		accept_pid = getpid();
		//Accept First connections
        newsock = accept(s, (struct sockaddr *) &remote, &len);
        fprintf(stderr, "New Client Connected \n");
        //Fork Child process
        fork();
        pid = getpid();

        /*  Force first process to stay in loop and watch for new connections
            When new connection comes fork again
            Original process should stay in loop to continue to accept and fork
            Because the process have their own address space the pid value should equal the accept_pid value
            When updating the pid value in the loop, the new child process will exit but the original should stay
        */
        if(debug == 0){
            while(pid == accept_pid){
                //This value should not change if this is functioning correctly
                fprintf(stderr,"Looping Accepting PID = %i \n",pid);
                fprintf(stderr,"This value should not change if functioning properly \n");
                newsock = accept(s, (struct sockaddr *) &remote, &len);
                fprintf(stderr, "New Client Connected \n");
                fork();
                pid = getpid();
            }
        }else{
            //Only allow one connection to communicate
        }
	}
	return(newsock);
}

/*
 * usage - print usage string and exit
 */
void
usage()
{
	fprintf(stderr, "usage: %s -d -l filename -p port -r dir -t time -n threadnum -s sched \n",progname);
	fprintf(stderr, "%s -h for help\n",progname);
	exit(1);
}
void
help(){
    fprintf(stderr, "Usage information\n");
	fprintf(stderr, "-d               :Enter Debugging Mode. Will not Daemonize. One connection at a time. Enables logging to stdout.\n");
	fprintf(stderr, "-h               :Prints Usage Summary with all options and exit\n");
	fprintf(stderr, "-l filename      :Log all requests to the given file. \n");
	fprintf(stderr, "-p               :Listen on the given port. If not provided, myhttpd will listen on port 8080\n");
	fprintf(stderr, "-r dir           :Set the root directory for the http server to dir\n");
	fprintf(stderr, "-t time          :Set the queuing time to time seconds. The default will be 60 seconds\n");
	fprintf(stderr, "-n threadnumber  :Set number of threads waiting ready in thread pool. Default is 4");
	fprintf(stderr, "-s sched         :Set the scheduling policy. Either FCFS or SJF. Default is FCFS\n");
    exit(1);
}
