file(GLOB SRC_FILES "*.c")

idf_component_register(SRCS ${SRC_FILES}
                    PRIV_REQUIRES spi_flash
                    INCLUDE_DIRS "")
