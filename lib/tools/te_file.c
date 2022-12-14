/* SPDX-License-Identifier: Apache-2.0 */
/** @file
 * @brief API to deal with files
 *
 * Functions to operate the files.
 *
 * Copyright (C) 2018-2022 OKTET Labs Ltd. All rights reserved.
 */

#include "te_config.h"
#include <libgen.h>
#include "te_defs.h"
#include "te_errno.h"
#include "te_file.h"
#include "te_string.h"
#include "te_dbuf.h"
#include "logger_api.h"

#ifdef STDC_HEADERS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#if HAVE_STDARG_H
#include <stdarg.h>
#endif

/* See description in te_file.h */
char *
te_basename(const char *pathname)
{
    char path[(pathname == NULL ? 0 : strlen(pathname)) + 1];
    char *name;

    if (pathname == NULL)
        return NULL;

    memcpy(path, pathname, sizeof(path));
    name = basename(path);
    if (*name == '/' || *name == '.')
        return NULL;

    return strdup(name);
}

/* See description in te_file.h */
char *
te_readlink_fmt(const char *path_fmt, ...)
{
    va_list ap;
    te_string path = TE_STRING_INIT;
    te_dbuf linkbuf = TE_DBUF_INIT(100);
    te_errno rc;
    te_bool resolved = FALSE;
#define CHECK_ERR(_msg)                         \
    do {                                        \
        if (TE_RC_GET_ERROR(rc) != 0)           \
        {                                       \
            ERROR("%s(): " _msg ": %r",         \
                  __FUNCTION__, rc);            \
            te_string_free(&path);              \
            te_dbuf_free(&linkbuf);             \
            return NULL;                        \
        }                                       \
    } while(0)

    va_start(ap, path_fmt);
    rc = te_string_append_va(&path, path_fmt, ap);
    va_end(ap);
    CHECK_ERR("cannot format the path");

    rc = te_dbuf_append(&linkbuf, NULL, path.len + 1);
    CHECK_ERR("cannot allocate link contents buffer");

    while (!resolved)
    {
        ssize_t nbytes = readlink(path.ptr, (char *)linkbuf.ptr, linkbuf.len);

        if (nbytes < 0)
        {
            ERROR("%s(): cannot resolve '%s': %r", __FUNCTION__, path.ptr,
                  TE_OS_RC(TE_MODULE_NONE, errno));

            te_string_free(&path);
            te_dbuf_free(&linkbuf);
            return NULL;
        }
        if (nbytes < linkbuf.len)
        {
            resolved = TRUE;
            linkbuf.ptr[nbytes] = '\0';
        }
        else
        {
            rc = te_dbuf_append(&linkbuf, NULL, linkbuf.len);
            CHECK_ERR("cannot extend link contents buffer");
        }
    }

    te_string_free(&path);
    return (char *)linkbuf.ptr;
#undef CHECK_ERR
}

/* See description in te_file.h */
char *
te_dirname(const char *pathname)
{
    char path[(pathname == NULL ? 0 : strlen(pathname)) + 1];

    if (pathname == NULL)
        return NULL;

    memcpy(path, pathname, sizeof(path));
    return strdup(dirname(path));
}

/* See description in te_file.h */
int
te_file_create_unique_fd_va(char **filename, const char *prefix_format,
                            const char *suffix, va_list ap)
{
    int fd;
    te_errno rc;
    te_string file = TE_STRING_INIT;
    int suffix_len;

    assert(filename != NULL);

    rc = te_string_append_va(&file, prefix_format, ap);
    if (rc != 0)
        return -1;

    if (suffix == NULL)
    {
        rc = te_string_append(&file, "%s", "XXXXXX");
        suffix_len = 0;
    }
    else
    {
        rc = te_string_append(&file, "%s%s", "XXXXXX", suffix);
        suffix_len = strlen(suffix);
    }
    if (rc != 0)
    {
        te_string_free(&file);
        return -1;
    }

    fd = mkstemps(file.ptr, suffix_len);
    if (fd != -1)
    {
        INFO("File has been created: '%s'", file.ptr);
        *filename = file.ptr;
    }
    else
    {
        ERROR("Failed to create file '%s': %s", file.ptr, strerror(errno));
        te_string_free(&file);
    }

    return fd;
}

/* See description in te_file.h */
int
te_file_create_unique_fd(char **filename, const char *prefix_format,
                         const char *suffix, ...)
{
    int fd;
    va_list ap;

    va_start(ap, suffix);
    fd = te_file_create_unique_fd_va(filename, prefix_format, suffix, ap);
    va_end(ap);

    return fd;
}

/* See description in te_file.h */
char *
te_file_create_unique(const char *prefix_format, const char *suffix, ...)
{
    va_list ap;
    char *filename = NULL;
    int fd;

    va_start(ap, suffix);
    fd = te_file_create_unique_fd_va(&filename, prefix_format, suffix, ap);
    va_end(ap);
    if (fd != -1)
        close(fd);

    return filename;
}

/* See description in te_file.h */
pid_t
te_file_read_pid(const char *pid_path)
{
    pid_t       pid = -1;
    FILE       *f;

    assert(pid_path != NULL);

    f = fopen(pid_path, "r");
    if (f == NULL)
        return -1;

    if (fscanf(f, "%d", &pid) != 1)
    {
        fclose(f);
        return -1;
    }

    fclose(f);

    return pid;
}

/* See description in te_file.h */
FILE *
te_fopen_fmt(const char *mode, const char *path_fmt, ...)
{
    te_string path = TE_STRING_INIT;
    te_errno rc;
    va_list ap;
    FILE *f;

    va_start(ap, path_fmt);
    rc = te_string_append_va(&path, path_fmt, ap);
    va_end(ap);
    if (rc != 0)
    {
        ERROR("%s(): te_string_append_va() failed to fill file path, rc=%r",
              __FUNCTION__, rc);
        te_string_free(&path);
        return NULL;
    }

    f = fopen(path.ptr, mode);
    if (f == NULL)
    {
        ERROR("%s(): failed to open '%s' with mode '%s', errno=%d ('%s')",
              __FUNCTION__, path.ptr, mode, errno, strerror(errno));
    }

    te_string_free(&path);

    return f;
}

te_errno
te_file_resolve_pathname_vec(const char *filename, const te_vec *pathvec,
                             int mode, char **resolved)
{
    te_errno rc = 0;

    if (filename == NULL || pathvec == NULL)
        return TE_EFAULT;

    assert(pathvec->element_size == sizeof(const char *));

    if (*filename == '/' || te_vec_size(pathvec) == 0)
    {
        if (access(filename, mode))
            rc = TE_OS_RC(TE_MODULE_NONE, errno);
        else if (resolved != NULL)
            *resolved = strdup(filename);
    }
    else
    {
        const char * const *dir;
        te_string fullpath = TE_STRING_INIT;

        TE_VEC_FOREACH(pathvec, dir)
        {
            rc = te_string_append(&fullpath, "%s/%s", *dir, filename);
            if (rc != 0)
            {
                te_string_free(&fullpath);
                return rc;
            }
            if (!access(fullpath.ptr, mode))
            {
                if (resolved != NULL)
                    *resolved = fullpath.ptr;
                else
                    te_string_free(&fullpath);
                return 0;
            }
            rc = TE_OS_RC(TE_MODULE_NONE, errno);
            te_string_reset(&fullpath);
        }
        te_string_free(&fullpath);
    }

    return rc;
}

te_errno
te_file_resolve_pathname(const char *filename, const char *path,
                         int mode, const char *basename, char **resolved)
{
    te_errno rc;
    te_vec pathvec = TE_VEC_INIT(char *);

    if (basename != NULL)
    {
        char *basedir;
        struct stat st;

        if (stat(basename, &st))
        {
            WARN("Cannot stat '%s': %r", basename,
                 TE_OS_RC(TE_MODULE_NONE, errno));
        }
        else
        {
            if (S_ISDIR(st.st_mode))
                basedir = strdup(basename);
            else
                basedir = te_dirname(basename);

            if (basedir == NULL)
            {
                rc = TE_OS_RC(TE_MODULE_NONE, errno);
                ERROR("Cannot determine dirname for '%s'", basename);
                te_vec_free(&pathvec);
                return rc;
            }
            TE_VEC_APPEND(&pathvec, basedir);
        }
    }

    rc = te_vec_split_string(path, &pathvec, ':', TRUE);
    if (rc != 0)
    {
        te_vec_deep_free(&pathvec);
        return rc;
    }

    rc = te_file_resolve_pathname_vec(filename, &pathvec, mode, resolved);
    te_vec_deep_free(&pathvec);

    return rc;
}

te_errno
te_file_check_executable(const char *path)
{
    return te_file_resolve_pathname(path, getenv("PATH"),
                                    X_OK, NULL, NULL);
}

te_errno
te_file_read_text(const char *path, char *buffer, size_t bufsize)
{
    int fd = open(path, O_RDONLY);
    off_t size;
    ssize_t actual;
    ssize_t i;
    int saved_errno;

    if (fd < 0)
        return TE_OS_RC(TE_MODULE_NONE, errno);
    size = lseek(fd, 0, SEEK_END);
    if (size == (off_t)-1)
    {
        saved_errno = errno;
        close(fd);
        return TE_OS_RC(TE_MODULE_NONE, saved_errno);
    }

    if (size >= bufsize)
    {
        ERROR("File %s's size (%zu) is larger than %zu", path,
              (size_t)size, bufsize);
        close(fd);
        return TE_RC(TE_MODULE_NONE, TE_EFBIG);
    }

    if (lseek(fd, 0, SEEK_SET) == (off_t)-1)
    {
        saved_errno = errno;
        close(fd);
        return TE_OS_RC(TE_MODULE_NONE, saved_errno);
    }

    actual = read(fd, buffer, size);
    if (actual < 0)
    {
        saved_errno = errno;
        close(fd);
        return TE_OS_RC(TE_MODULE_NONE, saved_errno);
    }
    close(fd);

    if (actual != size)
    {
        ERROR("Could not read %zu bytes from %s, only %zu were read", size,
              path, (size_t)actual);
        return TE_RC(TE_MODULE_NONE, TE_EIO);
    }

    for (i = 0; i < actual; i++)
    {
        if (buffer[i] == '\0')
        {
            ERROR("File '%s' contains an embedded zero at %zu", path, actual);
            return TE_RC(TE_MODULE_NONE, TE_EILSEQ);
        }
    }
    buffer[actual] = '\0';
    while (actual > 0 && buffer[actual - 1] == '\n')
    {
        actual--;
        buffer[actual] = '\0';
    }

    return 0;
}
