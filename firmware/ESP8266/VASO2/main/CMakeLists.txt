set(COMPONENT_SRCS "main.c"
        wifi.c
        httpServer.c
        taskSNTP.c
        Flash.c
        MCP3201.c
        Light.c
        SoilService.c
        Settings.c)
set(COMPONENT_ADD_INCLUDEDIRS "")

register_component()