#define __VSF_LINUX_FS_CLASS_INHERIT__
#define __VSF_FS_CLASS_INHERIT__
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

    // 1. hardware driver
    vsf_linux_fs_bind_pipe("/dev/ptyp0", "/dev/ttyp0", true);

    // 2. fs
    mkdir("/tmp", 0);

    // 3. install executables
    extern int mount_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/mount", mount_main);
    extern int dynloader_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/load", dynloader_main);

#ifdef VSF_LINUX_HOSTFS_TYPE
    int __vsf_linux_spawn(pid_t *pid, vsf_linux_main_entry_t entry,
                const posix_spawn_file_actions_t *actions,
                const posix_spawnattr_t *attr,
                char * const argv[], char * const env[], void *priv, int priv_size);

    // embedded busybox will not run, so setup basic directory structures here
    // temperary path to make mount command available
    putenv("PATH=/bin");
    pid_t pid;

#   ifdef MCULINUX_SRC_PATH
    mkdirs("/src/mculinux", 0);
    const char *mount_mculinux_src_argv[] = {
        "mount", "-t", VSF_LINUX_HOSTFS_TYPE, MCULINUX_SRC_PATH, "/src/mculinux", NULL,
    };
    if (!posix_spawnp(&pid, "mount", NULL, NULL, (char * const *)mount_mculinux_src_argv, NULL)) {
        waitpid(pid, NULL, 0);
    }
#   endif

#   ifdef VSF_SRC_PATH
    mkdirs("/src/vsf", 0);
    const char *mount_vsf_src_argv[] = {
        "mount", "-t", VSF_LINUX_HOSTFS_TYPE, VSF_SRC_PATH, "/src/vsf", NULL,
    };
    if (!posix_spawnp(&pid, "mount", NULL, NULL, (char * const *)mount_vsf_src_argv, NULL)) {
        waitpid(pid, NULL, 0);
        putenv("VSF_PATH=/src/vsf");
    }
#   endif

    mkdir("/usr", 0);
    const char *mount_usr_argv[] = {
        "mount", "-t", VSF_LINUX_HOSTFS_TYPE, "./usr", "/usr", NULL,
    };
    if (!posix_spawnp(&pid, "mount", NULL, NULL, (char * const *)mount_usr_argv, NULL)) {
        waitpid(pid, NULL, 0);
    }

    mkdir("/etc", 0);
    const char *mount_etc_argv[] = {
        "mount", "-t", VSF_LINUX_HOSTFS_TYPE, "./etc", "/etc", NULL,
    };
    if (!posix_spawnp(&pid, "mount", NULL, NULL, (char * const *)mount_etc_argv, NULL)) {
        waitpid(pid, NULL, 0);
    }

    mkdir("/home", 0);
    const char *mount_home_argv[] = {
        "mount", "-t", VSF_LINUX_HOSTFS_TYPE, "./home", "/home", NULL,
    };
    if (!posix_spawnp(&pid, "mount", NULL, NULL, (char * const *)mount_home_argv, NULL)) {
        waitpid(pid, NULL, 0);
    }

    mkdir("/var", 0);
    const char *mount_var_argv[] = {
        "mount", "-t", VSF_LINUX_HOSTFS_TYPE, "./var", "/var", NULL,
    };
    if (!posix_spawnp(&pid, "mount", NULL, NULL, (char * const *)mount_var_argv, NULL)) {
        waitpid(pid, NULL, 0);
    }
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
#if VSF_LINUX_CFG_LINK_FILE != ENABLED
#   define symlink(__target, __link)    vsf_linux_fs_bind_executable(__link, lbb_main);
#endif
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/arch");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/ascii");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/awk");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/base32");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/base64");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/basename");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/bc");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/bootchartd");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/bunzip2");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/cat");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/chat");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/chmod");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/cksum");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/clear");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/cmp");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/comm");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/cp");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/crc32");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/cttyhack");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/cut");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/date");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/dc");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/devmem");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/df");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/diff");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/dirname");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/dos2unix");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/du");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/dumpkmap");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/echo");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/ed");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/egrep");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/env");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/expand");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/expr");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/factor");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/false");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/fbsplash");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/fgconsole");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/fgrep");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/find");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/fold");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/fsync");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/grep");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/halt");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/head");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/hexedit");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/hostid");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/hush");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/i2cdetect");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/i2cdump");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/i2cget");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/i2cset");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/i2ctransfer");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/init");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/install");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/less");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/link");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/linuxrc");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/ln");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/logname");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/ls");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/lsscsi");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/man");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/md5sum");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/microcom");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/mim");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/mkdir");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/mkfifo");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/mknod");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/mktemp");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/mv");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/nl");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/nohup");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/od");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/partprobe");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/paste");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/patch");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/poweroff");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/printenv");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/printf");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/pwd");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/readlink");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/realpath");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/reboot");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/reset");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/rm");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/rmdir");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/rx");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/sed");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/seq");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/setkeycodes");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/setserial");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/sh");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/sha1sum");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/sha256sum");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/sha3sum");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/sha512sum");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/showkey");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/shred");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/shuf");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/sleep");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/sort");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/split");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/stat");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/strings");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/stty");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/sum");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/sync");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/tac");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/tail");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/tee");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/test");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/timeout");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/touch");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/tr");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/tree");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/true");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/truncate");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/ts");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/tsort");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/tty");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/ttysize");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/uname");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/unexpand");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/uniq");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/unix2dos");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/unlink");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/usleep");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/uudecode");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/uuencode");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/vi");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/volname");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/wc");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/which");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/whoami");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/xargs");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/yes");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/ping");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/wget");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/telnetd");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/httpd");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/ssl_client");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/dpkg");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/gunzip");

    return 0;
}

// TODO: SDL require that main need argc and argv
int VSF_APP_ENTRY(int argc, char *argv[])
{
    vsf_board_init();

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