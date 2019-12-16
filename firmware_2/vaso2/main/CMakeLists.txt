set(COMPONENT_SRCS "hello_world_main.c"
        wifi.c
        httpServer.c
        taskSNTP.c
        Flash.c
        MCP3201.c)
set(COMPONENT_ADD_INCLUDEDIRS "")

register_component()