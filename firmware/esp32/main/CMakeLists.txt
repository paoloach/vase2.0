set(COMPONENT_SRCS "main.c"
        wifi.c
        httpServer.c
        taskSNTP.c
        Flash.cpp
        MCP3201.c
        Light.c
        SoilService.c
        Settings.c
        DataChunkFlash.cpp
        DataChunk.cpp
        DataService.cpp)
set(COMPONENT_ADD_INCLUDEDIRS "")

register_component()