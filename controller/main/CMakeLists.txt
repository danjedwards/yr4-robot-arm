file(GLOB SRC_FILES "*.c")

idf_component_register(
    SRCS ${SRC_FILES}
    INCLUDE_DIRS "."
    PRIV_REQUIRES spi_flash
    REQUIRES "esp_wifi" "nvs_flash" "driver"
)
