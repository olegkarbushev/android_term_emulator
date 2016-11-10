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


// Global vars
//#####################################
pthread_t receiver_thread;

int result;
int client_socket;
int count = 1;
int err;
struct sockaddr_un addr;
socklen_t len;

char stdin_buffer[2048];

char *server_addr;
int is_connected = 0;
// Global vars END
//#####################################

void* receive_function(void *param) {
    int receiver = *((int *)param);
    char *data;
    
    data = (char *) malloc(2048);
    
    LOGD("Receiver thread has started, socket: %d", receiver);
    
    while(1){
        count = read(receiver, data, sizeof(data) );
        if(count <= 0){ 
            printf("Fialed to read data\r\nServer socket has been closed or error occured\r\n");
            LOGD("Fialed to read data\nServer socket has been closed or error occured");
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

    server_addr = (char *) malloc(250);

    if(server_addr == NULL){
        printf("Failed to allocate memory!\r\n");
        free(server_addr);
        return 1;
    }

    if (argc == 2) {
        if(!strcmp(argv[0], "--help")){
            printf("Usage: %s \"socket addres\" \r\n", argv[0]);
        } else {
            strncpy(server_addr, argv[1], strlen(argv[1]));
        }
    } else {
        strncpy(server_addr, LOCAL_SERVER_ADDR, strlen(LOCAL_SERVER_ADDR));
    }

    LOGD("\"%s\" will be used as socket addr", server_addr);
    printf("\"%s\" will be used as socket addr\r\n", server_addr);

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

        printf("%s: Cannot open client_socket: %s (%d)\n",
            __FUNCTION__, strerror(err), err);

        errno = err;
        free(server_addr);
        return 1;
    }

    LOGD("Connecting to server side");
    if (connect(client_socket, (struct sockaddr *) &addr, len) < 0) {
        err = errno;
        LOGD("%s: connect() failed: %s (%d)\n",
            __FUNCTION__, strerror(err), err);

        printf("%s: connect() failed: %s (%d)\n",
            __FUNCTION__, strerror(err), err);
        
        close(client_socket);
        errno = err;
        free(server_addr);
        return 1;
    }

    LOGD("Connected to server side");
    printf("Connected to server side\r\n");

    is_connected = 1;

    // creating receiver thread
    pthread_create(&receiver_thread, NULL, receive_function, &client_socket);

    printf("Enter commands, hit \"<Ctrl> + C\" to exit\r\n");

    // sending loop
    while(is_connected){
        if(fgets(stdin_buffer, sizeof(stdin_buffer), stdin) != NULL) {
            result = write(client_socket, stdin_buffer, sizeof(stdin_buffer));
            if(result < 0){
                printf("Fialed to send data\r\nServer socket has been closed or error occured\r\n");
                LOGD("Fialed to send data\nServer socket has been closed or error occured");
                break;
            }
            LOGD("Sent [%d]: %s", result, stdin_buffer);
            memset(stdin_buffer, '\0' ,sizeof(stdin_buffer));
        } else {
            LOGD("Failed to read from stdin");
            exit(0);
        }
    }

    LOGD("Finished Term Emulator");

    //Join the thread with the main thread
    pthread_join(receiver_thread, NULL);

    free(server_addr);

    return 0;
}
