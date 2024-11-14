#ifndef SERVER
#define SERVER

#include <stdbool.h>

#define AP_SSID "Robot Arm"
#define AP_PASSWORD "\0" // No password
#define MAX_CLIENTS 2
#define SERVER_PORT 8080
#define BUFFER_SIZE 255

// Error
#define MSG_OKAY 0
#define MSG_ERROR 1

// R/W
#define READ 0
#define WRITE 1

// Mode
#define IDLE 0
#define PROGRAM 1

// Command
#define WAYPOINT

typedef struct
{
    char err, rw, mode, command;
    char data[BUFFER_SIZE - 1];
    int data_len;
} message;

typedef void (*message_callback_t)(const message request, message *response);

void server_init();
void register_message_callback(message_callback_t callback);
void tcp_server_task(void *pvParameters);

#endif // SERVER