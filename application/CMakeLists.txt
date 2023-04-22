target_sources(${CMAKE_PROJECT_NAME} PUBLIC
    vsf_main.c
)

if(APP_USE_LINUX_BUSYBOX_DEMO)
    add_subdirectory(3rd-party/busybox)
endif()
