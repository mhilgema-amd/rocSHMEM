/******************************************************************************
 * Copyright (c) Advanced Micro Devices, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *****************************************************************************/

#ifndef __ROCSHMEM_EXAMPLES_UTIL_H__
#define __ROCSHMEM_EXAMPLES_UTIL_H__

#include <iostream>

#include <hip/hip_runtime_api.h>
#include <hip/hip_runtime.h>

#define CHECK_HIP(condition) {                                            \
        hipError_t error = condition;                                     \
        if(error != hipSuccess){                                          \
            fprintf(stderr,"HIP error: %d line: %d\n", error,  __LINE__); \
            MPI_Abort(MPI_COMM_WORLD, error));                            \
        }                                                                 \
    }

/*
 * Utility to check for MPI errors
 */
#if defined (DEBUG)
#define CHECK_MPI(call)                                                        \
  do {                                                                         \
    int my_status = call;                                                      \
    char error_string[128];                                                    \
    int len;                                                                   \
    fprintf(stderr, "Calling MPI in %s at %s(%d)\n", __FUNCTION__, __FILE__,   \
            __LINE__);                                                         \
    fflush(stderr);                                                            \
    if (my_status != MPI_SUCCESS) {                                            \
      MPI_Error_string(my_status, error_string, &len);                         \
      fprintf(stderr, "MPI error at %s:%d: %s\n", __FILE__, __LINE__,          \
              error_string);                                                   \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)
#else
#define CHECK_MPI(call)                                                        \
  do {                                                                         \
    int my_status = call;                                                      \
    char error_string[128];                                                    \
    int len;                                                                   \
    if (my_status != MPI_SUCCESS) {                                            \
      MPI_Error_string(my_status, error_string, &len);                         \
      fprintf(stderr, "MPI error at %s:%d: %s\n", __FILE__, __LINE__,          \
              error_string);                                                   \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)
#endif

static int get_launcher_local_rank() {
  int rank, local_rank, is_initialized = 0;
 
  CHECK_MPI(MPI_Initialized(&is_initialized));
 
  if (! is_initialized) {
    int provided;
    CHECK_MPI(MPI_Init_thread(nullptr, nullptr, MPI_THREAD_MULTIPLE, &provided));
  }

  /**
    * Find the local MPI rank number
    */
  MPI_Comm local_comm;
  CHECK_MPI(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
  CHECK_MPI(MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED,
                                rank, MPI_INFO_NULL, &local_comm));
  CHECK_MPI(MPI_Comm_rank(local_comm, &local_rank));
  CHECK_MPI(MPI_Comm_free(&local_comm));

  return local_rank;
}

#endif /* __ROCSHMEM_EXAMPLES_UTIL_H__ */
