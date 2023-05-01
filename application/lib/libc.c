#define __VSF_APPLET_LINUX_FUNDMENTAL_LIB__
#define __VSF_APPLET_LINUX_LIBC_STDIO_LIB__
#define __VSF_APPLET_LINUX_LIBC_STDLIB_LIB__
#define __VSF_APPLET_LINUX_LIBC_STRING_LIB__
#define __VSF_APPLET_LINUX_LIBC_TIME_LIB__
#define __VSF_APPLET_LINUX_UNISTD_LIB__
#define __VSF_APPLET_LINUX_DIRENT_LIB__
#define __VSF_APPLET_LINUX_DLFCN_LIB__
#define __VSF_APPLET_LINUX_GETOPT_LIB__
#define __VSF_APPLET_LINUX_LIBGEN_LIB__
#define __VSF_APPLET_LINUX_POLL_LIB__
#define __VSF_APPLET_LINUX_PWD_LIB__
#define __VSF_APPLET_LINUX_SEMAPHORE_LIB__
#define __VSF_APPLET_LINUX_SIGNAL_LIB__
#define __VSF_APPLET_LINUX_SYSLOG_LIB__
#define __VSF_APPLET_LINUX_SYS_EPOLL_LIB__
#define __VSF_APPLET_LINUX_SYS_EVENTFD_LIB__
#define __VSF_APPLET_LINUX_SYS_FILE_LIB__
#define __VSF_APPLET_LINUX_SYS_IOCTL_LIB__
#define __VSF_APPLET_LINUX_SYS_IPC_LIB__
#define __VSF_APPLET_LINUX_SYS_MMAN_LIB__
#define __VSF_APPLET_LINUX_SYS_MOUNT_LIB__
#define __VSF_APPLET_LINUX_SYS_PRCTL_LIB__
#define __VSF_APPLET_LINUX_SYS_RANDOM_LIB__
#define __VSF_APPLET_LINUX_SYS_REBOOT_LIB__
#define __VSF_APPLET_LINUX_SYS_SELECT_LIB__
#define __VSF_APPLET_LINUX_SYS_SEM_LIB__
#define __VSF_APPLET_LINUX_SYS_SENDFILE_LIB__
#define __VSF_APPLET_LINUX_SYS_SHM_LIB__
#define __VSF_APPLET_LINUX_SYS_SIGNALFD_LIB__
#define __VSF_APPLET_LINUX_SYS_SOCKET_LIB__
#define __VSF_APPLET_LINUX_SYS_STAT_LIB__
#define __VSF_APPLET_LINUX_SYS_STATVFS_LIB__
#define __VSF_APPLET_LINUX_SYS_SYSCALL_LIB__
#define __VSF_APPLET_LINUX_SYS_TIME_LIB__
#define __VSF_APPLET_LINUX_SYS_TIMES_LIB__
#define __VSF_APPLET_LINUX_SYS_UTSNAME_LIB__
#define __VSF_APPLET_LINUX_SYS_WAIT_LIB__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <dirent.h>
#include <dlfcn.h>
#include <getopt.h>
#include <libgen.h>
#include <poll.h>
#include <pwd.h>
#include <semaphore.h>
#include <signal.h>
#include <syslog.h>

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/prctl.h>
#include <sys/random.h>
#include <sys/reboot.h>
#include <sys/select.h>
#include <sys/sem.h>
#include <sys/sendfile.h>
#include <sys/shm.h>
#include <sys/signalfd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/utsname.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    // do necessary library init
    return 0;
}
