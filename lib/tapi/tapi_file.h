/* SPDX-License-Identifier: Apache-2.0 */
/** @file
 * @brief Test API to work with the files on the engine and TA.
 *
 * @defgroup ts_tapi_file Engine and TA files management
 * @ingroup te_ts_tapi
 * @{
 *
 * Functions for convinient work with the files on the engine and TA.
 *
 *
 * Copyright (C) 2004-2022 OKTET Labs Ltd. All rights reserved.
 */

#ifndef __TE_TAPI_FILE_H__
#define __TE_TAPI_FILE_H__

#include "te_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Generate unique basename for file.
 *
 * @return generated name
 *
 * @note The function returns a pointer to 
 *       a static buffer, so the caller should
 *       in most cases use strdup() on it or
 *       something similiar.
 */
extern char *tapi_file_generate_name(void);

/**
 * Generate unique pathname for file on the engine.
 *
 * @return generated name
 *
 * @note the function is not thread-safe
 */
extern char *tapi_file_generate_pathname(void);

/**
 * Create file in the TE temporary directory.
 *
 * @param len   file length
 * @param c     file content pattern
 *
 * @return name (memory is allocated) of the file or
 *         NULL in the case of failure
 *
 * @note The function is not thread-safe
 */
extern char *tapi_file_create_pattern(size_t len, char c);

/**
 * Create file in the TE temporary directory with the specified content.
 *
 * @param len     file length
 * @param buf     buffer with the file content
 * @param random  if TRUE, fill buffer with random data
 *
 * @return name (memory is allocated) of the file or
 *         NULL in the case of failure
 *
 * @note The function is not thread-safe
 */
extern char *tapi_file_create(size_t len, char *buf, te_bool random);

/**
 * Create file in the specified directory on the TA.
 *
 * @param ta            Test Agent name
 * @param filename      pathname of the file
 * @param fmt           format string for the file content
 *
 * @return Status code
 *
 * @note the function is not thread-safe
 */
extern te_errno tapi_file_create_ta(const char *ta, const char *filename,
                                    const char *fmt, ...)
    __attribute__((format(printf, 3, 4)));

/**
 * Create local file, copy it to TA, remove local file.
 * The function does the same thing as tapi_file_create_ta(),
 * but it creates local file with specified name instead of
 * using automatically generated name. This is useful when
 * you need to create files in a thread safe manner.
 *
 * @param ta            Test Agent name
 * @param lfile         pathname of the local file
 * @param rfile         pathname of the file on TA
 * @param fmt           format string for the file content
 *
 * @return Status code
 */
extern te_errno tapi_file_create_ta_r(const char *ta,
                                      const char *lfile,
                                      const char *rfile,
                                      const char *fmt, ...)
    __attribute__((format(printf, 4, 5)));;

/**
 * Read file content from the TA.
 *
 * @param ta            Test Agent name
 * @param filename      pathname of the file
 * @param pbuf          location for buffer allocated by the routine
 *
 * @return Status code
 *
 * @note the function is not thread-safe
 */
extern te_errno tapi_file_read_ta(const char *ta, const char *filename,
                                  char **pbuf);


/**
 * Like tapi_file_create_ta(), but it appends data to the file.
 *
 * If the file does not exist, it is created.
 *
 * @param ta            Test Agent name
 * @param filename      pathname of the file
 * @param fmt           format string for the file content
 *
 * @return Status code
 *
 * @note the function is not thread-safe
 */
extern te_errno tapi_file_append_ta(const char *ta, const char *filename,
                                    const char *fmt, ...)
    __attribute__((format(printf, 3, 4)));



/**
 * Copy file from the one TA to other or between the Engine and an agent.
 *
 * @param ta_src        source Test Agent
 *                      (@c NULL if the source is the Engine)
 * @param src           source file name
 * @param ta_dst        destination Test Agent
 *                      (@c NULL if the source is the Engine)
 * @param dst           destination file name
 *
 * @return Status code
 *
 * @todo Currently the function does not support copying files on the Engine
 *       locally, so both @p ta_src and @p ta_dst cannot be @c NULL at the
 *       same time.
 */
extern te_errno tapi_file_copy_ta(const char *ta_src, const char *src,
                                  const char *ta_dst, const char *dst);

/**
 * Unlink file on the TA.
 *
 * @param ta            Test Agent name
 * @param path_fmt      Format string to make path to be deleted
 *
 * @return Status code.
 */
extern te_errno tapi_file_ta_unlink_fmt(const char *ta,
                                        const char *path_fmt, ...)
    __attribute__((format(printf, 2, 3)));

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* !__TE_TAPI_FILE_H__ */

/**@} <!-- END ts_tapi_file --> */
