#pragma once
/* This file is part of the GNU plotutils package.  Copyright (C) 1989,
   1990, 1991, 1995, 1996, 1997, 1998, 1999, 2000, 2005, 2008, Free
   Software Foundation, Inc.

   The GNU plotutils package is free software.  You may redistribute it
   and/or modify it under the terms of the GNU General Public License as
   published by the Free Software foundation; either version 2, or (at your
   option) any later version.

   The GNU plotutils package is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with the GNU plotutils package; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin St., Fifth Floor,
   Boston, MA 02110-1301, USA. */

#include "sys-defines.h"
#include "multigrapher.h"
#include <stdio.h>

extern const char *progname; /* program name */

/* type of data in input stream */
typedef enum
{
  T_ASCII,
  T_SINGLE,
  T_DOUBLE,
  T_INTEGER,
  T_GNUPLOT,
  T_ASCII_ERRORBAR
} data_type;

#define NO_OF_LINEMODES 5     /* see linemode.c */
#define MAX_COLOR_NAME_LEN 32 /* long enough for all of libplot's colors */

/* types of line */
extern const char *linemodes[NO_OF_LINEMODES];
extern const char *colorstyle[NO_OF_LINEMODES];

/*----------------- prototypes for functions in plotter.h -------------------*/

extern int delete_multigrapher (Multigrapher *multigrapher);

extern void begin_graph (Multigrapher *multigrapher, double scale,
                         double trans_x, double trans_y);

extern void end_graph (Multigrapher *multigrapher);

extern void draw_frame_of_graph (Multigrapher *multigrapher, bool draw_canvas);

extern void plot_point (Multigrapher *multigrapher, const Point *point);

extern void plot_point_array (Multigrapher *multigrapher, const Point *p,
                              int length);

extern void end_polyline_and_flush (Multigrapher *multigrapher);

/*----------------- prototypes for functions in reader.h -------------------*/

typedef struct ReaderStruct Reader;

extern Reader *new_reader (FILE *input, data_type input_type,
                           bool auto_abscissa, double delta_x, double abscissa,
                           bool transpose_axes, int log_axis, bool auto_bump,
                           int symbol, double symbol_size,
                           const char *symbol_font_name, int linemode,
                           double line_width, double fill_fraction,
                           bool use_color);

extern void delete_reader (Reader *reader);

extern void read_file (Reader *reader, Point **p, int *length,
                       int *no_of_points);

extern void read_and_plot_file (Reader *reader, Multigrapher *multigrapher);

extern void alter_reader_parameters (
    Reader *reader, FILE *input, data_type input_type, bool auto_abscissa,
    double delta_x, double abscissa, int symbol, double symbol_size,
    const char *symbol_font_name, int linemode, double line_width,
    double fill_fraction, bool use_color, bool new_symbol,
    bool new_symbol_size, bool new_symbol_font_name, bool new_linemode,
    bool new_line_width, bool new_fill_fraction, bool new_use_color);

/*----------------- prototypes for functions in misc.h -------------------*/

extern void array_bounds (const Point *p, int length, bool transpose_axes,
                          int clip_mode, double *min_x, double *min_y,
                          double *max_x, double *max_y, bool spec_min_x,
                          bool spec_min_y, bool spec_max_x, bool spec_max_y);

/*------------------------------------------------------------------------*/
