#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/un.h>
#include <android/log.h>
#include <pthread.h>
#include <signal.h>

// Defines
//#####################################
#define LOCAL_SERVER_ADDR "com.soft.penguin.localServerSock"

#define LOG_TAG "Term Emulator!!!"

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,LOG_TAG,__VA_ARGS__)
#define LOGS(...) __android_log_print(ANDROID_LOG_SILENT,LOG_TAG,__VA_ARGS__)
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE,LOG_TAG,__VA_ARGS__)

// Function prototypes
//#####################################
void INThandler(int);

// Global vars
//#####################################
pthread_t receiver_thread;

int result;
int client_socket;
int count = 1;
int err;
struct sockaddr_un addr;
socklen_t len;

char buffer[] = "Hello from native";
char entered_data[2048];

char *server_addr;
int is_connected = 0;
// Global vars END
//#####################################


void  INThandler(int sig) {
    char  c;

    signal(sig, SIG_IGN);
    printf("OUCH, did you hit Ctrl-C?\n"
            "Do you really want to quit? [y/n] ");
    c = getchar();
    if (c == 'y' || c == 'Y') {
        free(server_addr);
        close(client_socket);
        exit(0);
    } else {
        signal(SIGINT, INThandler);
    }
    getchar(); // Get new line character
}

void* receive_function(void *param) {
    int receiver = *((int *)param);
    int count;
    char *data, *dbuf;
    
    data = (char *) malloc(2048);
    
    LOGD("Receiver thread has started, socket: %d", receiver);
    
    while(1){
        count = read(receiver, data, sizeof(data) );
        if(count <= 0){ 
            printf("Server socket has been closed or error occured\r\n");
            LOGD("Server socket has been closed or error occured");
            break;
        } else {
            LOGD("> %s", data);
            printf("> %s\r\n", data);
            //fflush(stdout);
        }
    }
    
    LOGD("Receiver thread has finised");
    free(data);
    
    is_connected = 0;

    return NULL;
}

// Main
//#####################################
int main(int argc, char** argv) {
    LOGD("Term emulator started up");
    
    signal(SIGINT, INThandler);

    server_addr = (char *) malloc(250);
    
    if(server_addr == NULL){
        printf("Failed to allocate memory!\r\n");
        free(server_addr);
        return 1;
    }

    if (argc < 2) {
        // Tell the user how to run the program
        printf("Usage: %s \"Socket addres\" \r\n", argv[0]);
        printf("Default address wil be used: %s\r\n", LOCAL_SERVER_ADDR);
        strncpy(server_addr, LOCAL_SERVER_ADDR, strlen(LOCAL_SERVER_ADDR));
        //server_addr[strlen(LOCAL_SERVER_ADDR)] = '\0';
    } else {
        printf("Term emulator :) \r\n");
        sscanf(argv[1], "%s", server_addr);
    }
    
    fflush(stdout);
    addr.sun_family = AF_LOCAL;
    /* use abstract namespace for client_socket path */
    addr.sun_path[0] = '\0';
    strcpy(&addr.sun_path[1], server_addr);
    len = offsetof(struct sockaddr_un, sun_path) + 1 + strlen(&addr.sun_path[1]);
    
    //create client_socket
    LOGD("Creating cleint side client_socket");
    client_socket = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (client_socket < 0) {
        err = errno;
        LOGD("%s: Cannot open client_socket: %s (%d)\n",
            __FUNCTION__, strerror(err), err);
        errno = err;
        free(server_addr);
        return 1;
    }
        
    LOGD("Connecting to Java LocalSocketServer");
    if (connect(client_socket, (struct sockaddr *) &addr, len) < 0) {
        err = errno;
        LOGD("%s: connect() failed: %s (%d)\n",
            __FUNCTION__, strerror(err), err);
        close(client_socket);
        errno = err;
        free(server_addr);
        return 1;
    }

    LOGD("Managed to connect to Java LocalSocketServer");
    
    is_connected = 1;
    
    // creating receiver thread
    pthread_create(&receiver_thread, NULL, receive_function, &client_socket);
    
    printf("Enter commands, hit \"<Ctrl> + C\" to exit\r\n");
    
    // sending loop
    while(is_connected){
        if(fgets(entered_data, sizeof(entered_data), stdin) != NULL) {
            result = write(client_socket, entered_data, sizeof(entered_data));
            LOGD("Sent [%d]: %s", result, entered_data);
            memset(entered_data, '\0' ,sizeof(entered_data));
        } else {
            LOGD("Failed to read from stdin");
            exit(0);
        }
    }

    LOGD("Finished Term emulator");

    //Join the thread with the main thread
    pthread_join(receiver_thread, NULL);
    
    free(server_addr);
    
    return 0;
}
