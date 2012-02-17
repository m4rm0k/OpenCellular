// Copyright (c) 2012 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cgpt.h"

#define __STDC_FORMAT_MACROS
#include <getopt.h>
#include <inttypes.h>
#include <string.h>
#include "cgpt_params.h"

static void Usage(void)
{
  printf("\nUsage: %s show [OPTIONS] DRIVE\n\n"
         "Display the GPT table\n\n"
         "Options:\n"
         "  -n           Numeric output only\n"
         "  -v           Verbose output\n"
         "  -q           Quick output\n"
         "  -i NUM       Show specified partition only - pick one of:\n"
         "               -b  beginning sector\n"
         "               -s  partition size\n"
         "               -t  type guid\n"
         "               -u  unique guid\n"
         "               -l  label\n"
         "               -S  Successful flag\n"
         "               -T  Tries flag\n"
         "               -P  Priority flag\n"
         "               -A  raw 64-bit attribute value\n"
         "\n", progname);
}

int cmd_show(int argc, char *argv[]) {
  struct drive drive;

  CgptShowParams params;
  memset(&params, 0, sizeof(params));

  int c;
  int errorcnt = 0;
  char *e = 0;

  opterr = 0;                     // quiet, you
  while ((c=getopt(argc, argv, ":hnvqi:bstulSTPA")) != -1)
  {
    switch (c)
    {
    case 'n':
      params.numeric = 1;
      break;
    case 'v':
      params.verbose = 1;
      break;
    case 'q':
      params.quick = 1;
      break;
    case 'i':
      params.partition = (uint32_t)strtoul(optarg, &e, 0);
      if (!*optarg || (e && *e))
      {
        Error("invalid argument to -%c: \"%s\"\n", c, optarg);
        errorcnt++;
      }
      break;
    case 'b':
    case 's':
    case 't':
    case 'u':
    case 'l':
    case 'S':
    case 'T':
    case 'P':
    case 'A':
      params.single_item = c;
      break;

    case 'h':
      Usage();
      return CGPT_OK;
    case '?':
      Error("unrecognized option: -%c\n", optopt);
      errorcnt++;
      break;
    case ':':
      Error("missing argument to -%c\n", optopt);
      errorcnt++;
      break;
    default:
      errorcnt++;
      break;
    }
  }
  if (errorcnt)
  {
    Usage();
    return CGPT_FAILED;
  }

  if (optind >= argc) {
    Error("missing drive argument\n");
    Usage();
    return CGPT_FAILED;
  }

  params.drive_name = argv[optind];

  return cgpt_show(&params);
}
