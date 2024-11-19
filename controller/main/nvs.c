// #include "nvs_manager.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <string.h>
#include <stdio.h>

static const char *NAMESPACE = "robot_data";

void init_nvs()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    for (int i = 0; i < MAX_PROGRAMS; i++)
    {
        char *prog_name = get_program_name(i);
        if (prog_name != NULL)
        {
            printf("Found %s\n", prog_name);
        }
        else
        {
            char program_name[MAX_NAME_LEN];
            snprintf(program_name, MAX_NAME_LEN, "Program %d", i);
            set_program_name(i, program_name);
        }
    }
}

static void generate_key(char *key, const char *prefix, uint8_t index)
{
    snprintf(key, MAX_NAME_LEN, "%s_%d", prefix, index);
}

char *get_program_name(uint8_t program_index)
{
    static char name[MAX_NAME_LEN];
    if (program_index >= MAX_PROGRAMS)
    {
        printf("Invalid program index\n");
        return NULL;
    }

    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK)
    {
        printf("Error opening NVS handle\n");
        return NULL;
    }

    char key[MAX_NAME_LEN];
    generate_key(key, "prog_name", program_index);

    size_t required_size = MAX_NAME_LEN;
    err = nvs_get_str(nvs_handle, key, name, &required_size);
    if (err != ESP_OK)
    {
        printf("Error reading program name\n");
        nvs_close(nvs_handle);
        return NULL;
    }

    nvs_close(nvs_handle);
    return name;
}

void set_program_name(uint8_t program_index, const char *name)
{
    if (program_index >= MAX_PROGRAMS)
    {
        printf("Invalid program index\n");
        return;
    }

    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK)
    {
        printf("Error opening NVS handle\n");
        return;
    }

    char key[MAX_NAME_LEN];
    generate_key(key, "prog_name", program_index);

    err = nvs_set_str(nvs_handle, key, name);
    if (err != ESP_OK)
    {
        printf("Error setting program name\n");
    }

    nvs_commit(nvs_handle);
    nvs_close(nvs_handle);
}

void save_waypoint(uint8_t program_index, uint8_t waypoint_index, waypoint wp)
{
    if (program_index >= MAX_PROGRAMS || waypoint_index >= MAX_WAYPOINTS)
    {
        printf("Invalid program or waypoint index\n");
        return;
    }

    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK)
    {
        printf("Error opening NVS handle\n");
        return;
    }

    char key[MAX_NAME_LEN];
    snprintf(key, MAX_NAME_LEN, "wp_%d_%d", program_index, waypoint_index);

    err = nvs_set_blob(nvs_handle, key, &wp, sizeof(waypoint));
    if (err != ESP_OK)
    {
        printf("Error saving waypoint\n");
        printf("Error (%s) writing data to NVS!\n", esp_err_to_name(err));
    }

    nvs_commit(nvs_handle);
    nvs_close(nvs_handle);
}

waypoint load_waypoint(uint8_t program_index, uint8_t waypoint_index)
{
    waypoint wp = {0};

    if (program_index >= MAX_PROGRAMS || waypoint_index >= MAX_WAYPOINTS)
    {
        printf("Invalid program or waypoint index\n");
        return wp;
    }

    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK)
    {
        printf("Error opening NVS handle\n");
        return wp;
    }

    char key[MAX_NAME_LEN];
    snprintf(key, MAX_NAME_LEN, "wp_%d_%d", program_index, waypoint_index);

    size_t required_size = sizeof(waypoint);
    err = nvs_get_blob(nvs_handle, key, &wp, &required_size);
    if (err != ESP_OK)
    {
        printf("Waypoint not found! Writing default waypoint...\n");
        save_waypoint(program_index, waypoint_index, wp);
        return wp;
    }

    nvs_close(nvs_handle);
    return wp;
}

void load_program(uint8_t program_index, waypoint *program)
{
    char *name = get_program_name(program_index);
    printf("Loading %s\n", name);
    for (int i = 0; i < MAX_WAYPOINTS; i++)
    {
        program[i] = load_waypoint(program_index, i);
    }
}

void save_program(uint8_t program_index, waypoint *program)
{
    char *name = get_program_name(program_index);
    printf("Saving to %s\n", name);
    for (int i = 0; i < MAX_WAYPOINTS; i++)
    {
        save_waypoint(program_index, i, program[i]);
    }
}