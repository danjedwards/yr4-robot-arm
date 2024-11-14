#ifndef NVS
#define NVS

#include <stdint.h>

#define MAX_PROGRAMS 5
#define MAX_WAYPOINTS 100
#define MAX_NAME_LEN 16

typedef struct
{
    // uint16_t x, y, z;
    uint16_t base_pos, shoulder_pos, elbow_pos, wrist1_pos, wrist2_pos, gripper_pos;
} waypoint;

extern waypoint current_program[MAX_WAYPOINTS];

void init_nvs();

char *get_program_name(uint8_t program_index);
void set_program_name(uint8_t program_index, const char *name);

void save_waypoint(uint8_t program_index, uint8_t waypoint_index, waypoint wp);
waypoint load_waypoint(uint8_t program_index, uint8_t waypoint_index);
void load_program(uint8_t program_index);

#endif // NVS