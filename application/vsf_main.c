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

typedef struct vsf_linux_dev_res_t {
    vsf_queue_stream_t pty_streams[1][2];
} vsf_linux_dev_res_t;
static vsf_linux_dev_res_t __vsf_linux_dev_res;

static vsf_linux_term_priv_t *priv_master;
static vsf_linux_term_priv_t *priv_slave;
int vsf_linux_create_fhs(void)
{
    // 0. devfs, busybox, etc
    vsf_linux_vfs_init();

    // 1. hardware driver
    char pty_name[] = "/dev/ptyp0";
    int fd_master, fd_slave;
    vsf_linux_fd_t *sfd_master;
    vsf_linux_fd_t *sfd_slave;
    vsf_queue_stream_t *queue_stream0, *queue_stream1;
    vk_vfs_file_t *vfs_file;
    extern void __vsf_linux_rx_stream_init(vsf_linux_stream_priv_t *priv_tx);
    extern vk_vfs_file_t * __vsf_linux_get_vfs(int fd);
    for (int i = 0; i < dimof(__vsf_linux_dev_res.pty_streams); i++) {
        queue_stream0 = &__vsf_linux_dev_res.pty_streams[i][0];
        queue_stream0->max_buffer_size = -1;
        queue_stream0->max_entry_num = -1;
        queue_stream0->op = &vsf_queue_stream_op;
        VSF_STREAM_INIT(queue_stream0);
        queue_stream1 = &__vsf_linux_dev_res.pty_streams[i][1];
        queue_stream1->max_buffer_size = -1;
        queue_stream1->max_entry_num = -1;
        queue_stream1->op = &vsf_queue_stream_op;
        VSF_STREAM_INIT(queue_stream1);

        pty_name[9] = '0' + i;

        pty_name[5] = 'p';
        vsf_linux_fs_bind_target_ex(pty_name, NULL, &vsf_linux_term_fdop,
                NULL, NULL,
                VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE | VSF_FILE_ATTR_TTY, 0);
        fd_master = open(pty_name, 0);
        sfd_master = vsf_linux_fd_get(fd_master);
        priv_master = (vsf_linux_term_priv_t *)sfd_master->priv;
        priv_master->subop = &__vsf_linux_stream_fdop;
        priv_master->stream_rx = &queue_stream0->use_as__vsf_stream_t;
        priv_master->stream_tx = &queue_stream1->use_as__vsf_stream_t;
        __vsf_linux_rx_stream_init(priv_master);
        __vsf_linux_tx_stream_init(priv_master);
        vsf_linux_term_fdop.fn_init(sfd_master);
        vfs_file = __vsf_linux_get_vfs(fd_master);
        vfs_file->f.param = priv_master;
        vfs_file->attr |= __VSF_FILE_ATTR_SHARE_PRIV;

        pty_name[5] = 't';
        vsf_linux_fs_bind_target_ex(pty_name, NULL, &vsf_linux_term_fdop,
                NULL, NULL,
                VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE | VSF_FILE_ATTR_TTY, 0);
        fd_slave = open(pty_name, 0);
        sfd_slave = vsf_linux_fd_get(fd_slave);
        priv_slave = (vsf_linux_term_priv_t *)sfd_slave->priv;
        priv_slave->subop = &__vsf_linux_stream_fdop;
        priv_slave->stream_rx = &queue_stream1->use_as__vsf_stream_t;
        priv_slave->stream_tx = &queue_stream0->use_as__vsf_stream_t;
        __vsf_linux_rx_stream_init(priv_slave);
        __vsf_linux_tx_stream_init(priv_slave);
        vsf_linux_term_fdop.fn_init(sfd_slave);
        vfs_file = __vsf_linux_get_vfs(fd_slave);
        vfs_file->f.param = priv_slave;
        vfs_file->attr |= __VSF_FILE_ATTR_SHARE_PRIV;
    }

    // 2. fs

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

#   ifdef VSF_SRC_PATH
    mkdirs("/src/vsf", 0);
    const char *mount_src_argv[] = {
        "mount", "-t", VSF_LINUX_HOSTFS_TYPE, VSF_SRC_PATH, "/src/vsf", NULL,
    };
    if (!posix_spawnp(&pid, "mount", NULL, NULL, (char * const *)mount_src_argv, NULL)) {
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
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/arch", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/ascii", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/awk", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/base32", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/base64", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/basename", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/bc", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/bootchartd", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/bunzip2", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/cat", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/chat", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/chmod", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/cksum", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/clear", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/cmp", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/comm", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/cp", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/crc32", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/cttyhack", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/cut", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/date", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/dc", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/devmem", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/df", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/diff", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/dirname", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/dos2unix", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/du", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/dumpkmap", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/echo", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/ed", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/egrep", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/env", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/expand", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/expr", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/factor", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/false", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/fbsplash", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/fgconsole", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/fgrep", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/find", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/fold", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/fsync", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/grep", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/halt", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/head", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/hexedit", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/hostid", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/hush", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/i2cdetect", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/i2cdump", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/i2cget", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/i2cset", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/i2ctransfer", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/init", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/install", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/less", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/link", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/linuxrc", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/ln", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/logname", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/ls", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/lsscsi", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/man", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/md5sum", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/microcom", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/mim", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/mkdir", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/mkfifo", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/mknod", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/mktemp", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/mv", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/nl", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/nohup", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/od", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/partprobe", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/paste", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/patch", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/poweroff", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/printenv", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/printf", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/pwd", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/readlink", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/realpath", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/reboot", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/reset", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/rm", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/rmdir", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/rx", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/sed", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/seq", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/setkeycodes", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/setserial", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/sh", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/sha1sum", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/sha256sum", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/sha3sum", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/sha512sum", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/showkey", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/shred", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/shuf", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/sleep", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/sort", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/split", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/stat", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/strings", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/stty", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/sum", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/sync", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/tac", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/tail", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/tee", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/test", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/timeout", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/touch", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/tr", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/tree", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/true", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/truncate", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/ts", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/tsort", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/tty", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/ttysize", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/uname", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/unexpand", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/uniq", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/unix2dos", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/unlink", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/usleep", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/uudecode", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/uuencode", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/vi", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/volname", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/wc", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/which", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/whoami", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/xargs", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/yes", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/ping", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/wget", lbb_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/telnetd", lbb_main);

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