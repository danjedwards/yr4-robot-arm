# file(GLOB SRC_FILES "*.c")

# message(WARNING ${SRC_FILES} )

idf_component_register(SRCS "test_main.c" "test_server.c" "../../main/server.c" 
                        INCLUDE_DIRS "." "../../main"
                        REQUIRES cmock)