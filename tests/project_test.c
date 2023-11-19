/**
 * @file project_test.c
 * @author Christopher Kok (chris@forcelightning.xyz)
 * @brief This file runs the tests for the project.
 * @version 0.1
 * @date 2023-11-19
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void
report_error (const char *p_msg,
              const char *p_file,
              int         line,
              const char *p_func_name,
              bool        require)
{
    printf("%s:%d:%s: (FAIL)\n", p_file, line, p_func_name);
    if (require)
    {
        exit(-1);
    }
}