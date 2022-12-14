/* SPDX-License-Identifier: Apache-2.0 */
/** @file
 * @brief RPC Test Suite
 *
 * Write file to Agent.
 *
 * Copyright (C) 2020-2022 OKTET Labs Ltd. All rights reserved.
 */

/** @page file_write Test for writing file
 *
 * @objective Demo of TAPI/RPC file writing test
 *
 * @par Scenario:
 *
 */

#define TE_TEST_NAME    "file_write"

#include "file_suite.h"

int
main(int argc, char **argv)
{
    char           *rfile = NULL;
    uint8_t        *buf = NULL;
    rcf_rpc_server *pco_iut = NULL;
    int             fd = -1;
    uint8_t        *data = NULL;
    const size_t    data_size = BUFSIZE;

    TEST_START;
    TEST_GET_PCO(pco_iut);

    TEST_STEP("Write data to a file on TA");
    data = te_make_buf_by_len(BUFSIZE);
    rfile = tapi_file_generate_name();
    if (tapi_file_create_ta(pco_iut->ta, rfile, "%s", "") != 0)
    {
        TEST_VERDICT("tapi_file_create_ta() failed");
    }
    fd = rpc_open(pco_iut, rfile, RPC_O_WRONLY | RPC_O_CREAT, 0);

    CHECK_LENGTH(rpc_write_and_close(pco_iut, fd, data, data_size), data_size);

    TEST_STEP("Read content from the file on TA");
    buf = (uint8_t *)tapi_calloc(1, data_size);
    fd = rpc_open(pco_iut, rfile, RPC_O_RDONLY | RPC_O_CREAT, 0);
    CHECK_LENGTH(rpc_read(pco_iut, fd, buf, data_size), data_size);
    rpc_close(pco_iut, fd);

    TEST_STEP("Check data");
    file_compare_and_fail(data, data_size, buf, data_size);

    TEST_SUCCESS;

cleanup:

    CLEANUP_CHECK_RC(tapi_file_ta_unlink_fmt(pco_iut->ta, "%s", rfile));

    free(buf);
    free(data);

    TEST_END;
}
