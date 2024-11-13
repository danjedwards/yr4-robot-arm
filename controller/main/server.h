#ifndef SERVER
#define SERVER

#define AP_SSID "Robot Arm"
#define AP_PASSWORD "\0" // No password
#define MAX_CLIENTS 2
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

// Callback type for receiving data
typedef void (*message_callback_t)(const char *data, int len);

// Function prototypes
void server_init();
void register_message_callback(message_callback_t callback);
void tcp_server_task(void *pvParameters);

#endif // SERVER