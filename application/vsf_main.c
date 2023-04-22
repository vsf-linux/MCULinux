#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_DISP_CLASS_INHERIT__
#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>

#ifndef VSF_APP_ENTRY
#   define VSF_APP_ENTRY        VSF_USER_ENTRY
#endif

extern int lbb_main(int argc, char *argv[]);
static int __busybox_export(int argc, char *argv[])
{
    char path[PATH_MAX], *app_pos;
    strcpy(path, VSF_LINUX_CFG_BIN_PATH "/");
    app_pos = &path[strlen(path)];

    argv++;
    argc--;
    while (argc-- > 0) {
        strcpy(app_pos, *argv++);
        vsf_linux_fs_bind_executable(path, lbb_main);
    }
    return 0;
}

WEAK(vsf_board_init)
void vsf_board_init(void) {}

int vsf_linux_create_fhs(void)
{
    // 0. devfs, busybox, etc
    vsf_linux_vfs_init();

    // 1. hardware driver related demo
    vsf_board_init();

    // 2. fs

    // 3. install executables
    extern int dynloader_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/load", dynloader_main);

#if defined(__WIN__) || defined(__LINUX__)
    int __vsf_linux_spawn(pid_t *pid, vsf_linux_main_entry_t entry,
                const posix_spawn_file_actions_t *actions,
                const posix_spawnattr_t *attr,
                char * const argv[], char * const env[], void *priv, int priv_size);

    // embedded busybox will not run, so setup basic directory structures here
    // temperary path to make mount command available
    putenv("PATH=/bin");
    pid_t pid;
    mkdir("/usr", 0);
    const char *mount_usr_argv[] = {
        "mount", "-t", VSF_LINUX_HOSTFS_TYPE, "./usr", "/usr", NULL,
    };
    posix_spawnp(&pid, "mount", NULL, NULL, (char * const *)mount_usr_argv, NULL);
    waitpid(pid, NULL, 0);

    mkdir("/etc", 0);
    const char *mount_etc_argv[] = {
        "mount", "-t", VSF_LINUX_HOSTFS_TYPE, "./etc", "/etc", NULL,
    };
    posix_spawnp(&pid, "mount", NULL, NULL, (char * const *)mount_etc_argv, NULL);
    waitpid(pid, NULL, 0);

    mkdir("/home", 0);
    const char *mount_home_argv[] = {
        "mount", "-t", VSF_LINUX_HOSTFS_TYPE, "./home", "/home", NULL,
    };
    posix_spawnp(&pid, "mount", NULL, NULL, (char * const *)mount_home_argv, NULL);
    waitpid(pid, NULL, 0);
#else
    const char *inittab_content = ":1:askfirst:/bin/sh\n";
    mkdir("/etc", 0);
    int fd = open("/etc/inittab", O_CREAT);
    if (fd >= 0) {
        write(fd, inittab_content, strlen(inittab_content));
        close(fd);
    }
#endif

    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/busybox_export", __busybox_export);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/busybox", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/init", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/sh", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/ls", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/cat", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/rm", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/mkdir", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/chmod", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/touch", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/uname", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/vi", lbb_main);

    return 0;
}

// TODO: SDL require that main need argc and argv
int VSF_APP_ENTRY(int argc, char *argv[])
{
    VSF_STREAM_INIT(&VSF_DEBUG_STREAM_RX);
    VSF_STREAM_INIT(&VSF_DEBUG_STREAM_TX);

#if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
    vsf_stdio_init();
#endif

    vsf_trace_info("start linux..." VSF_TRACE_CFG_LINEEND);
    vsf_linux_stdio_stream_t stream = {
        .in     = (vsf_stream_t *)&VSF_DEBUG_STREAM_RX,
        .out    = (vsf_stream_t *)&VSF_DEBUG_STREAM_TX,
        .err    = (vsf_stream_t *)&VSF_DEBUG_STREAM_TX,
    };
    vsf_linux_init(&stream);
    return 0;
}

/* EOF */