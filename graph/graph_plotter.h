/* Copyright (C) 2024 by v66v <74927439+v66v@users.noreply.github.com> */

#pragma once
#include "axis.h"
#include "multigrapher.h"

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

#include "arg/arg.h" // TODO: remove
#include "extern.h"
#include "libcommon.h"
#include "axis.h"
#include "transform.h"
#include "plot.h"
#include "sys-defines.h"

/* we use floating point libplot coordinates in the range [0,PLOT_SIZE] */
#define PLOT_SIZE 4096.0

enum
{
  ACCEPTED = 0x1,
  CLIPPED_FIRST = 0x2,
  CLIPPED_SECOND = 0x4
};

/* forward references */
int clip_line (Multigrapher *multigrapher, double *x0_p, double *y0_p,
               double *x1_p, double *y1_p);
void set_line_style (Multigrapher *multigrapher, int style, bool use_color);
void set_graph_parameters (
    Multigrapher *multigrapher, double frame_line_width,
    const char *frame_color, const char *title, const char *title_font_name,
    double title_font_size, double tick_size, grid_type grid_spec,
    double x_min, double x_max, double x_spacing, double y_min, double y_max,
    double y_spacing, bool spec_x_spacing, bool spec_y_spacing, double width,
    double height, double up, double right, const char *x_font_name,
    double x_font_size, const char *x_label, const char *y_font_name,
    double y_font_size, const char *y_label, bool no_rotate_y_label,
    int log_axis, int round_to_next_tick, int switch_axis_end, int omit_ticks,
    int clip_mode, double blankout_fraction, bool transpose_axes,
    char legend_position, bool legend_plot);
/* print_tick_label() prints a label on an axis tick.  The format depends
 * on whether the axis is a log axis or a linear axis; also, the magnitude
 * of the axis labels.
 */
void draw_legend_of_graph (Multigrapher *multigrapher);
int plot_graph_no_filter (ARG_LIST *arg_list);
Multigrapher *
new_multigrapher (const char *output_format, const char *bg_color,
                  const char *bitmap_size, const char *emulate_color,
                  const char *max_line_length, const char *meta_portable,
                  const char *page_size, const char *rotation_angle,
                  bool save_screen, bool legend_plot);
