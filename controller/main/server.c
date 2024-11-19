#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "server.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

static const char *TAG = "wifi_ap";
static message_callback_t message_callback = NULL; // Callback for received messages

void register_message_callback(message_callback_t callback)
{
    message_callback = callback;
}

void on_message_received(const message msg, message *response)
{
    ESP_LOGI(TAG, "Callback received message");
}

void buffer_to_message(message *msg, const char *buf, int len)
{
    msg->rw = (buf[0] >> 6) & 1;
    msg->state = (buf[0] >> 3) & 7;
    msg->command = buf[0] & 7;
    msg->data_len = len - 1;

    if (msg->data_len > 0)
    {
        memcpy(msg->data, buf + 1, msg->data_len);
    }
}

void message_to_buffer(message msg, char *buf)
{
    buf[0] = (msg.err << 7) | ((msg.rw & 1) << 6) | ((msg.state & 7) << 3) | (msg.command & 7);
    memcpy(buf + 1, msg.data, msg.data_len);
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG, "Device connected, MAC: " MACSTR, MAC2STR(event->mac));
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
        ESP_LOGI(TAG, "Device disconnected, MAC: " MACSTR, MAC2STR(event->mac));
    }
}

void handle_client_task(void *pvParameters)
{
    int client_socket = (int)pvParameters;
    char buffer[BUFFER_SIZE];
    int len;

    while ((len = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) > 0)
    {
        buffer[len] = '\0';

        ESP_LOGI(TAG, "Received message: %s", buffer);

        message request, response = {0};
        buffer_to_message(&request, buffer, len);

        if (message_callback)
        {
            message_callback(request, &response);
        }

        message_to_buffer(response, buffer);

        send(client_socket, buffer, response.data_len + 1, 0);
    }

    ESP_LOGI(TAG, "Client disconnected");
    close(client_socket);
    vTaskDelete(NULL);
}

void tcp_server_task(void *pvParameters)
{
    int listen_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Create a TCP socket
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket < 0)
    {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    // Bind the socket to the port
    if (bind(listen_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        close(listen_socket);
        vTaskDelete(NULL);
        return;
    }

    // Listen for incoming connections
    if (listen(listen_socket, 1) < 0)
    {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        close(listen_socket);
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "TCP server listening on port %d", SERVER_PORT);

    while (1)
    {
        // Accept a client connection
        client_socket = accept(listen_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket < 0)
        {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            break;
        }

        ESP_LOGI(TAG, "Client connected");

        // Create a new task to handle the client connection
        xTaskCreate(handle_client_task, "handle_client_task", 4096, (void *)client_socket, 5, NULL);
    }

    // Clean up
    close(listen_socket);
    vTaskDelete(NULL);
}

void server_init()
{
    // Initialize TCP/IP and Wi-Fi
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = AP_SSID,
            .ssid_len = strlen(AP_SSID),
            .password = AP_PASSWORD,
            .max_connection = MAX_CLIENTS,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
        },
    };

    if (strlen(AP_PASSWORD) == 0)
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Wi-Fi AP started. SSID:%s password:%s", "ESP32_AP", "12345678");

    // Start the TCP server task
    xTaskCreate(tcp_server_task, "tcp_server_task", 4096, NULL, 5, NULL);

    // Defualt callback for receiving data
    register_message_callback(on_message_received);
}