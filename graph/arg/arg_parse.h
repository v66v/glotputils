#pragma once
#include "arg_common.h"
#include "libcommon.h"
#include "graph_plotter.h"
#include "file.h"

int parse_flags (ARG_LIST *arg_list, int argc, char *argv[]);
bool parse_pen_string (const char *pen_s);
