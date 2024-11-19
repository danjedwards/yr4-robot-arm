#ifndef SERVER
#define SERVER

#include <stdbool.h>

// General Config
#define AP_SSID "Robot Arm"
#define AP_PASSWORD "\0"
#define MAX_CLIENTS 1
#define SERVER_PORT 8080
#define BUFFER_SIZE 255

// Message Defines
#define MSG_ERROR 0
#define MSG_OKAY 1
// RW
#define MSG_READ 0
#define MSG_WRITE 1
// States
#define MSG_IDLE 0
#define MSG_PROGRAM 1
#define MSG_RUNNING 2
// Commands
#define MSG_WAYPOINT_COUNT 0
#define MSG_WAYPOINT_IDX 1
#define MSG_WAYPOINT_CUR 2
#define MSG_PROGRAM_COUNT 3
#define MSG_PROGRAM_NAME 4
#define MSG_PROGRAM_RW 5
#define MSG_RUN 6

typedef struct
{
    char err, rw, state, command;
    char data[BUFFER_SIZE - 1];
    int data_len;
} message;

typedef void (*message_callback_t)(const message request, message *response);

void server_init();
void register_message_callback(message_callback_t callback);
void tcp_server_task(void *pvParameters);

#endif // SERVER