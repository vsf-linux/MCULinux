vsf_add_sources(
    vsf_main.c
)
vsf_add_include_directories(
    .
)

if(APP_USE_LINUX_BUSYBOX_DEMO)
    add_subdirectory(3rd-party/busybox)
endif()
