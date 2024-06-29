/* Copyright (C) 2024 by v66v <74927439\+v66v@users.noreply.github.com> */

#pragma once
#include "point.h"
#include "transform.h"

/* The `x_axis' and `y_axis' elements of a Multigrapher, which are of type
   `Axis', specify the layout of the two axes of a graph.  They are used in
   the drawing of a graph frame.  All elements that are doubles are
   expressed in user coordinates (unless the axis is logarithmic, in which
   case logs are taken before this structure is filled in). */

/* The `x_axis' and `y_axis' elements are filled in by calls to
   prepare_axis() when set_graph_parameters() is called.  The only
   exceptions to this are the elements `max_width' and `non_user_ticks',
   which are filled in by draw_frame_of_graph(), as the frame for a graph
   is being drawn. */

typedef struct
{
  const char *font_name; /* fontname for axis label and tick labels */
  double font_size;      /* font size for axis label and tick labels */
  const char *label;     /* axis label (a string) */
  int type;              /* axis layout type (A_LINEAR or A_LOG10) */
  double tick_spacing;   /* distance between ticks */
  int min_tick_count, max_tick_count; /* tick location = count * spacing */
  bool have_lin_subticks;     /* does axis have linearly spaced subticks? */
  double lin_subtick_spacing; /* distance between linearly spaced subticks */
  int min_lin_subtick_count, max_lin_subtick_count;
  bool have_normal_subsubticks;    /* does axis have logarithmic subsubticks?*/
  bool user_specified_subsubticks; /* axis has user-spec'd subsubticks? */
  double subsubtick_spacing;       /* spacing for user-specified ones */
  double other_axis_loc;           /* location of intersection w/ other axis */
  double alt_other_axis_loc; /* alternative loc. (e.g. right end vs. left)*/
  bool switch_axis_end;      /* other axis at right/top, not left/bottom? */
  bool omit_ticks;           /* just plain omit them (and their labels) ? */
  double max_label_width;    /* max width of labels placed on axis, in
                                libplot coors (we update this during
                                plotting, for y axis only) */
  int labelled_ticks;        /* number of labelled ticks, subticks, and
                                subsubticks drawn on the axis
                                (we update this during plotting, so we
                                can advise the user to specify a tick
                                spacing by hand if labelled_ticks <= 2) */
} Axis;

/* style of graph frame; the 1st four of these are increasingly fancy, but
   the last (AXES_AT_ORIGIN) is an altogether different style */
typedef enum
{
  NO_AXES = 0,
  AXES = 1,
  AXES_AND_BOX = 2,
  AXES_AND_BOX_AND_GRID = 3,
  AXES_AT_ORIGIN = 4
} grid_type;

#define NORMAL_AXES(grid_spec)                                                \
  ((grid_spec == AXES) || (grid_spec == AXES_AND_BOX)                         \
   || (grid_spec == AXES_AND_BOX_AND_GRID))

/* bit fields in portmanteau variables */
enum
{
  X_AXIS = 0x1,
  Y_AXIS = 0x2
};

/* valid graph axis layout types; A_LOG2, anyone? */
#define A_LINEAR 0
#define A_LOG10 1

#define TRIAL_NUMBER_OF_TICK_INTERVALS 5
#define MAX_NUM_SUBTICKS 29       /* max num. of linearly spaced subticks */
#define RELATIVE_SUBTICK_SIZE 0.4 /* subtick_size / tick_size */
/* if a log axis spans >5.0 orders of magnitude, don't plot log subsubticks */
#define MAX_DECADES_WITH_LOG_SUBSUBTICKS 5.0

/* inter-tick spacing types, returned by scale1() and spacing_type() */
#define S_ONE 0
#define S_TWO 1
#define S_FIVE 2
#define S_TWO_FIVE 3 /* we don't use this one, but user may request it */
#define S_UNKNOWN -2

void print_tick_label (char *labelbuf, const Axis *axis,
                       const Transform *transform, double val);
void scale1 (double min, double max, double *tick_spacing,
             int *tick_spacing_type);
int spacing_type (double spacing);
void prepare_axis (Axis *axisp, Transform *trans, double min, double max,
                   double spacing, const char *font_name, double font_size,
                   const char *label, double subsubtick_spacing,
                   bool user_specified_subsubticks, bool round_to_next_tick,
                   bool log_axis, bool reverse_axis, bool switch_axis_end,
                   bool omit_ticks);
void transpose_portmanteau (int *val);
