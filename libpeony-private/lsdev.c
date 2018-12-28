#include "lsdev.h"
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <inttypes.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <locale.h>
#include <sys/sysmacros.h>

#define PATH_MAX 4096

#define F_DUPFD_CLOEXEC	17

#define PATH_SYS_DEVBLOCK "/sys/dev/block"
#define PATH_SYS_BLOCK "/sys/block"

static dev_t sysfs_devname_to_devno(const char *name, const char *parent);


struct sysfs_cxt {
        dev_t   devno;
        int     dir_fd;         /* /sys/block/<name> */
        char    *dir_path;
        struct sysfs_cxt *parent;

        unsigned int    scsi_host,
                        scsi_channel,
                        scsi_target,
                        scsi_lun;

        unsigned int    has_hctl : 1;
};

struct blkdev_cxt {
        struct blkdev_cxt *parent;

        //struct libscols_line *scols_line;
        struct stat     st;

        char *name;             /* kernel name in /sys/block */
        char *dm_name;          /* DM name (dm/block) */

        char *filename;         /* path to device node */

        struct sysfs_cxt  sysfs;

        int partition;          /* is partition? TRUE/FALSE */

        int probed;             /* already probed */
        char *fstype;           /* detected fs, NULL or "?" if cannot detect */
        char *uuid;             /* filesystem UUID (or stack uuid) */
        char *label;            /* filesystem label */
        char *parttype;         /* partiton type UUID */
        char *partuuid;         /* partition UUID */
        char *partlabel;        /* partiton label */
        char *partflags;        /* partition flags */
        char *wwn;              /* storage WWN */
        char *serial;           /* disk serial number */

        int npartitions;        /* # of partitions this device has */
        int nholders;           /* # of devices mapped directly to this device
                                 * /sys/block/.../holders */
        int nslaves;            /* # of devices this device maps to */
        int maj, min;           /* devno */
        int discard;            /* supports discard */

        uint64_t size;          /* device size */
};

static struct dirent *xreaddir(DIR *dp)
{
        struct dirent *d;

        while ((d = readdir(dp))) {
                if (!strcmp(d->d_name, ".") ||
                    !strcmp(d->d_name, ".."))
                        continue;

                /* blacklist here? */
                break;
        }
        return d;
}

static int is_dm(const char *name)
{
        return strncmp(name, "dm-", 3) ? 0 : 1;
}

static char *sysfs_devno_path(dev_t devno, char *buf, size_t bufsiz)
{
	int len;
	len = snprintf(buf, bufsiz, PATH_SYS_DEVBLOCK "/%d:%d",
                        major(devno), minor(devno));
        return (len < 0 || (size_t) len + 1 > bufsiz) ? NULL : buf;
}

static void sysfs_deinit(struct sysfs_cxt *cxt)
{
        if (!cxt)
                return;

        if (cxt->dir_fd >= 0)
               close(cxt->dir_fd);
        free(cxt->dir_path);

        memset(cxt, 0, sizeof(*cxt));

        cxt->dir_fd = -1;
}

static char *devno_to_sysfs_name(dev_t devno, char *devname, char *buf, size_t buf_size)
{
        char path[PATH_MAX];
        ssize_t len;

        if (!sysfs_devno_path(devno, path, sizeof(path))) {
                g_warning(("%s: failed to compose sysfs path"), devname);
                return NULL;
        }

        len = readlink(path, buf, buf_size - 1);
        if (len < 0) {
                g_warning(("%s: failed to read link"), path);
                return NULL;
        }
        buf[len] = '\0';

        return g_strdup(strrchr(buf, '/') + 1);
}

int sysfs_stat(struct sysfs_cxt *cxt, const char *attr, struct stat *st)
{
        int rc = fstatat(cxt->dir_fd, attr, st, 0);

        if (rc != 0 && errno == ENOENT &&
            strncmp(attr, "queue/", 6) == 0 && cxt->parent) {

                /* Exception for "queue/<attr>". These attributes are available
                 * for parental devices only
                 */
                return fstatat(cxt->parent->dir_fd,
                                 attr, st, 0);
        }
        return rc;
}

int sysfs_has_attribute(struct sysfs_cxt *cxt, const char *attr)
{
        struct stat st;

        return sysfs_stat(cxt, attr, &st) == 0;
}

static int sysfs_open(struct sysfs_cxt *cxt, const char *attr, int flags)
{
        int fd = openat(cxt->dir_fd,  attr, flags);

        if (fd == -1 && errno == ENOENT &&
            strncmp(attr, "queue/", 6) == 0 && cxt->parent) {

                /* Exception for "queue/<attr>". These attributes are available
                 * for parental devices only
                 */
                fd = openat(cxt->parent->dir_fd, cxt->dir_path, attr, flags);
        }
        return fd;
}

static FILE *sysfs_fopen(struct sysfs_cxt *cxt, const char *attr)
{
        int fd = sysfs_open(cxt, attr, O_RDONLY|O_CLOEXEC);

        return fd < 0 ? NULL : fdopen(fd, "r" UL_CLOEXECSTR);
}


static int sysfs_scanf(struct sysfs_cxt *cxt,  const char *attr, const char *fmt, ...)
{
        FILE *f = sysfs_fopen(cxt, attr);
        va_list ap;
        int rc;

        if (!f)
                return -EINVAL;
        va_start(ap, fmt);
        rc = vfscanf(f, fmt, ap);
        va_end(ap);

        fclose(f);
        return rc;
}


char *sysfs_strdup(struct sysfs_cxt *cxt, const char *attr)
{
        char buf[BUFSIZ];
        return sysfs_scanf(cxt, attr, "%1023[^\n]", buf) == 1 ?
                                                strdup(buf) : NULL;
}

static int sysfs_init(struct sysfs_cxt *cxt, dev_t devno, struct sysfs_cxt *parent)
{
        char path[PATH_MAX];
        int fd, rc;

        memset(cxt, 0, sizeof(*cxt));
        cxt->dir_fd = -1;

        if (!sysfs_devno_path(devno, path, sizeof(path)))
                goto err;

        fd = open(path, O_RDONLY|O_CLOEXEC);
        if (fd < 0)
                goto err;
        cxt->dir_fd = fd;

        cxt->dir_path = g_strdup(path);
        if (!cxt->dir_path)
                goto err;
        cxt->devno = devno;
        cxt->parent = parent;
        return 0;
err:
        rc = errno > 0 ? -errno : -1;
        sysfs_deinit(cxt);
        return rc;
}

int dup_fd_cloexec(int oldfd, int lowfd)
{
        int fd, flags, errno_save;

#ifdef F_DUPFD_CLOEXEC
        fd = fcntl(oldfd, F_DUPFD_CLOEXEC, lowfd);
        if (fd >= 0)
                return fd;
#endif

        fd = dup(oldfd);
        if (fd < 0)
                return fd;

        flags = fcntl(fd, F_GETFD);
        if (flags < 0)
                goto unwind;
        if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) < 0)
                goto unwind;

        return fd;

unwind:
        errno_save = errno;
        close(fd);
        errno = errno_save;

        return -1;
}

/**
 * sysfs_devname_sys_to_dev:
 * @name: devname to be converted in place
 *
 * Linux kernel linux/drivers/base/core.c: device_get_devnode()
 * defines a replacement of '!' in the /sys device name by '/' in the
 * /dev device name. This helper replaces all ocurrences of '!' in
 * @name by '/' to convert from /sys to /dev.
 */
static inline void sysfs_devname_sys_to_dev(char *name)
{
        char *c;

        if (name)
                while ((c = strchr(name, '!')))
                        c[0] = '/';
}

/**
 * sysfs_devname_dev_to_sys:
 * @name: devname to be converted in place
 *
 * See sysfs_devname_sys_to_dev().
 */
static inline void sysfs_devname_dev_to_sys(char *name)
{
        char *c;

        if (name)
                while ((c = strchr(name, '/')))
                        c[0] = '!';
}

static DIR *sysfs_opendir(struct sysfs_cxt *cxt, const char *attr)
{
        DIR *dir;
        int fd = -1;

        if (attr)
                fd = sysfs_open(cxt, attr, O_RDONLY|O_CLOEXEC);

        else if (cxt->dir_fd >= 0)
                /* request to open root of device in sysfs (/sys/block/<dev>)
                 * -- we cannot use cxt->sysfs_fd directly, because closedir()
                 * will close this our persistent file descriptor.
                 */
                fd = dup_fd_cloexec(cxt->dir_fd, STDERR_FILENO + 1);

        if (fd < 0)
                return NULL;

        dir = fdopendir(fd);
        if (!dir) {
                close(fd);
                return NULL;
        }
        if (!attr)
                 rewinddir(dir);
        return dir;
}

static char *sysfs_get_slave(struct sysfs_cxt *cxt)
{
        DIR *dir;
        struct dirent *d;
        char *name = NULL;

        if (!(dir = sysfs_opendir(cxt, "slaves")))
                return NULL;

        while ((d = xreaddir(dir))) {
                if (name)
                        goto err;       /* more slaves */

                name = strdup(d->d_name);
        }

        closedir(dir);
        return name;
err:
        free(name);
        closedir(dir);
        return NULL;
}

ssize_t readlink_at(int dir, const char *dirname __attribute__ ((__unused__)),
                    const char *pathname, char *buf, size_t bufsiz)
{
        return readlinkat(dir, pathname, buf, bufsiz);
}


ssize_t sysfs_readlink(struct sysfs_cxt *cxt, const char *attr,
                   char *buf, size_t bufsiz)
{
        if (!cxt->dir_path)
                return -1;

        if (attr)
                return readlink_at(cxt->dir_fd, cxt->dir_path, attr, buf, bufsiz);

        /* read /sys/dev/block/<maj:min> link */
        return readlink(cxt->dir_path, buf, bufsiz);
}

static char *sysfs_get_devname(struct sysfs_cxt *cxt, char *buf, size_t bufsiz)
{
        char linkpath[PATH_MAX];
        char *name;
        ssize_t sz;

        sz = sysfs_readlink(cxt, NULL, linkpath, sizeof(linkpath) - 1);
        if (sz < 0)
                return NULL;
        linkpath[sz] = '\0';

        name = strrchr(linkpath, '/');
        if (!name)
                return NULL;

        name++;
        sz = strlen(name);

        if ((size_t) sz + 1 > bufsiz)
                return NULL;

        memcpy(buf, name, sz + 1);
        sysfs_devname_sys_to_dev(buf);

        return buf;
}

static char *canonicalize_dm_name(const char *ptname)
{
        FILE    *f;
        size_t  sz;
        char    path[256], name[256], *res = NULL;

        if (!ptname || !*ptname)
                return NULL;

        snprintf(path, sizeof(path), "/sys/block/%s/dm/name", ptname);
        if (!(f = fopen(path, "r" UL_CLOEXECSTR)))
                return NULL;

        /* read "<name>\n" from sysfs */
        if (fgets(name, sizeof(name), f) && (sz = strlen(name)) > 1) {
                name[sz - 1] = '\0';
                snprintf(path, sizeof(path), "/dev/mapper/%s", name);

                if (access(path, F_OK) == 0)
                        res = strdup(path);
        }
        fclose(f);
        return res;
}

static char *get_device_path(struct blkdev_cxt *cxt)
{
        char path[PATH_MAX];

        g_assert(cxt);
        g_assert(cxt->name);

        if (is_dm(cxt->name))
                return canonicalize_dm_name(cxt->name);

        snprintf(path, sizeof(path), "/dev/%s", cxt->name);
        sysfs_devname_sys_to_dev(path);
        return strdup(path);
}


static int get_dm_wholedisk(struct sysfs_cxt *cxt, char *diskname,
                size_t len, dev_t *diskdevno)
{
    int rc = 0;
    char *name;

    /* Note, sysfs_get_slave() returns the first slave only,
     * if there is more slaves, then return NULL
     */
    name = sysfs_get_slave(cxt);
    if (!name)
        return -1;

    printf ("=====\n");
    printf ("name: %s\n", name);
    if (diskname && len) {
        strncpy(diskname, name, len);
        diskname[len - 1] = '\0';
        printf ("diskname: %s\n");
    }

    if (diskdevno) {
        *diskdevno = sysfs_devname_to_devno(name, NULL);
        if (!*diskdevno)
            rc = -1;
    }

    free(name);
    return rc;
}



static int sysfs_devno_to_wholedisk(dev_t dev, char *diskname,
            size_t len, dev_t *diskdevno)
{
    struct sysfs_cxt cxt;
    int is_part = 0;

    if (!dev || sysfs_init(&cxt, dev, NULL) != 0)
        return -1;

    is_part = sysfs_has_attribute(&cxt, "partition");
    if (!is_part) {
        /*
         * Extra case for partitions mapped by device-mapper.
         *
         * All regualar partitions (added by BLKPG ioctl or kernel PT
         * parser) have the /sys/.../partition file. The partitions
         * mapped by DM don't have such file, but they have "part"
         * prefix in DM UUID.
         */
        char *uuid = sysfs_strdup(&cxt, "dm/uuid");
        char *tmp = uuid;
        char *prefix = uuid ? strsep(&tmp, "-") : NULL;

        if (prefix && strncasecmp(prefix, "part", 4) == 0)
            is_part = 1;
        free(uuid);

        //if (is_part &&
          //  get_dm_wholedisk(&cxt, diskname, len, diskdevno) == 0)
            /*
             * partitioned device, mapped by DM
             */
            //goto done;

        is_part = 0;
    }

    if (!is_part) {
        /*
         * unpartitioned device
         */
        if (diskname && len) {
            if (!sysfs_get_devname(&cxt, diskname, len))
                g_warning("There is an error");
        }
        if (diskdevno)
            *diskdevno = dev;

    } 

    sysfs_deinit(&cxt);
    return 0;

}

static dev_t sysfs_devname_to_devno(const char *name, const char *parent)
{
        char buf[PATH_MAX], *path = NULL;
        dev_t dev = 0;

        if (strncmp("/dev/", name, 5) == 0) {
                /*
                 * Read from /dev
                 */
                struct stat st;

                if (stat(name, &st) == 0)
                        dev = st.st_rdev;
                else
                        name += 5;      /* unaccesible, or not node in /dev */
        }

        if (!dev && parent && strncmp("dm-", name, 3)) {
                /*
                 * Create path to /sys/block/<parent>/<name>/dev
                 */
                char *_name = strdup(name), *_parent = strdup(parent);
                int len;

                if (!_name || !_parent) {
                        free(_name);
                        free(_parent);
                        return 0;
                }
                sysfs_devname_dev_to_sys(_name);
                sysfs_devname_dev_to_sys(_parent);

                len = snprintf(buf, sizeof(buf),
                                PATH_SYS_BLOCK "/%s/%s/dev", _parent, _name);
                free(_name);
                free(_parent);
                if (len < 0 || (size_t) len + 1 > sizeof(buf))
                        return 0;
                path = buf;
        } else if (!dev) {
                /*
                 * Create path to /sys/block/<sysname>/dev
                 */
                char *_name = strdup(name);
                int len;

                if (!_name)
                        return 0;

                sysfs_devname_dev_to_sys(_name);
                len = snprintf(buf, sizeof(buf),
                                PATH_SYS_BLOCK "/%s/dev", _name);
                free(_name);
                if (len < 0 || (size_t) len + 1 > sizeof(buf))
                        return 0;
                path = buf;
        }

        if (path) {
                /*
                 * read devno from sysfs
                 */
                FILE *f;
                int maj = 0, min = 0;

                f = fopen(path, "r" UL_CLOEXECSTR);
                if (!f)
                        return 0;

                if (fscanf(f, "%d:%d", &maj, &min) == 2)
                        dev = makedev(maj, min);
                fclose(f);
        }
        return dev;
}

static int sysfs_read_u64(struct sysfs_cxt *cxt, const char *attr, uint64_t *res)
{
        uint64_t x = 0;

        if (sysfs_scanf(cxt, attr, "%"SCNu64, &x) == 1) {
                if (res)
                        *res = x;
                return 0;
        }
        return -1;
}

static int set_cxt(struct blkdev_cxt *cxt,
                    struct blkdev_cxt *parent,
                    struct blkdev_cxt *wholedisk,
                    const char *name)
{
        dev_t devno;

        cxt->parent = parent;
        cxt->name = g_strdup(name);
        cxt->partition = wholedisk != NULL;

        cxt->filename = get_device_path(cxt);
        if (!cxt->filename) {
                g_warning(("%s: failed to get device path"), cxt->name);
                return -1;
        }

        devno = sysfs_devname_to_devno(cxt->name, wholedisk ? wholedisk->name : NULL);

        if (!devno) {
                g_warning(("%s: unknown device name"), cxt->name);
                return -1;
        }


        if (sysfs_init(&cxt->sysfs, devno, parent ? &parent->sysfs : NULL)) {
                       g_warning(("%s: failed to initialize sysfs handler"), cxt->name);
                        return -1;
                }
        

        cxt->maj = major(devno);
        cxt->min = minor(devno);
        cxt->size = 0;

        if (sysfs_read_u64(&cxt->sysfs, "size", &cxt->size) == 0)       /* in sectors */
                cxt->size <<= 9;                                        /* in bytes */


        return 0;
}

static void reset_blkdev_cxt(struct blkdev_cxt *cxt)
{
        if (!cxt)
                return;


        free(cxt->name);
        free(cxt->dm_name);
        free(cxt->filename);
        free(cxt->fstype);
        free(cxt->uuid);
        free(cxt->label);
        free(cxt->parttype);
        free(cxt->partuuid);
        free(cxt->partlabel);
        free(cxt->wwn);
        free(cxt->serial);

        sysfs_deinit(&cxt->sysfs);

        memset(cxt, 0, sizeof(*cxt));
}


goffset process_one_device(char *devname)
{
        struct blkdev_cxt parent = { 0 }, cxt = { 0 };
        struct stat st;
	goffset lsblk_space;
        char buf[PATH_MAX + 1]="", *name = NULL, *diskname = NULL;
        dev_t disk = 0;
        int real_part = 0;

        if (stat(devname, &st) || !S_ISBLK(st.st_mode)) {
                g_warning(("%s: not a block device"), devname);
                goto leave;
        }

        if (!(name = devno_to_sysfs_name(st.st_rdev, devname, buf, PATH_MAX))) {
                g_warning(("%s: failed to get sysfs name"), devname);
                goto leave;
        }

        if (!strncmp(name, "dm-", 3)) {
                /* dm mapping is never a real partition! */
                real_part = 0;
        } else {
                if (sysfs_devno_to_wholedisk(st.st_rdev, buf, sizeof(buf), &disk)) {
                        g_warning(("%s: failed to get whole-disk device number"), devname);
                        goto leave;
                }
                diskname = buf;
                real_part = st.st_rdev != disk;
        }

        if (!real_part) {
                /*
                 * Device is not a partition.
                 */
                if (set_cxt(&cxt, NULL, NULL, name))
                        goto leave;
               
        } 

        printf ("===============\n");
        printf ("buf: %s, name: %s, diskname: %s\n", buf, name, diskname);
        printf ("===============\n");

leave:
	lsblk_space = cxt.size;
        free(name);
        reset_blkdev_cxt(&cxt);

        if (real_part)
                reset_blkdev_cxt(&parent);

        return lsblk_space;
}
	

