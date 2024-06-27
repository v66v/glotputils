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

/* This file contains the point plotter half of GNU graph.  The point
   plotter could easily be linked with other software.  It translates a
   sequence of points, regarded as defining a polyline or a sequence of
   polylines, to a sequence of libplot calls.  There is support for
   multigraphing, i.e. producing a plot consisting of more than a single
   graph.  Each graph may be drawn from more than one file, i.e., input
   stream, and each input stream may provide more than a single polyline.

   A `point' is a structure.  Each point structure contains the following
   fields:

      x and y coordinates of the point
      a `have_x_errorbar' flag (true or false)
      a `have_y_errorbar' flag (true or false)
      xmin and xmax (meaningful only if have_x_errorbar is set)
      ymin and ymax (meaningful only if have_y_errorbar is set)
      a `pendown' flag

      a symbol type (a small integer, interpreted as a marker type)
      a symbol size (a fraction of the size of the plotting box)
      a symbol font name (relevant only for symbol types >= 32)
      a linemode (a small integer)
      a linewidth (a fraction of the size of the display device)
      a polyline fill-fraction (in the interval [0,1], <0 means no fill)
      a use_color flag (true or false)

   The point plotter constructs a polyline from each successive run of
   points that have the pendown flag set.  It assumes that the final seven
   fields are assumed to be the same for each point in such a run, i.e., it
   takes their values from the first point of the run.  At the location of
   each point on the polyline, the appropriate marker symbol, if any, will
   be plotted.  Symbol types greater than or equal to 32 are interpreted as
   single characters to be plotted, rather than symbols.

   Points without the pendown flag set cause the polyline to be broken, and
   a new one to begin, before the symbol (if any) is plotted.

   The plotter supports five basic linemodes: 1 through 5.  The
   interpretation of `linemode' depends on the polyline's use_color flag.

           linemode		If monochrome 		If color

               1		solid			red
               2		dotted			green
               3		dotdashed		blue
               4		shortdashed		magenta
               5		longdashed		cyan

   In the monochrome case, the pattern simply repeats: 6,7,8,9,10 are
   equivalent to 1,2,3,4,5, etc.  In the colored case, the sequence of
   colors also repeats.  But linemodes 1,2,3,4,5 are drawn solid, while
   6,7,8,9,10 are drawn dotted, 11,12,13,14,15 are drawn dotdashed, etc.
   So there are 25 distinct colored linemodes, and 5 distinct monochrome
   (black) ones.

   The color of a symbol will be the same as the color of the polyline on
   which it is plotted.

   linemodes -1, -2, etc. have a special interpretation.  They are
   `disconnected' linemodes: no polyline will appear, but if color is
   being used, the color of the plotted symbols (if any) will be
   linemode-dependent.  -1,-2,-3,-4,5 signify red,green,blue,magenta,cyan
   (the same sequence as for 1,2,3,4,5); thereafter the sequence repeats.

   linemode 0 is special (for backward compatibility).  No line is drawn;
   symbol #1 (a point) will be used.  So using linemode 0 is the same as
   using linemode -1, symbol 1.

   The point plotter is invoked by calling the following, in order.

   new_multigrapher() creates a new point plotter.
   begin_graph()
   set_graph_parameters() initializes global structures used by
        the draw_frame_of_graph() and plot_point() routines.  These include
        the structures that specify the linear transformation from user
        coordinates to the coordinates used by libplot, and structures
        that specify the style of the plot frame.
   draw_frame_of_graph() plots the graph frame.  [Optional.]
   plot_point() uses libplot routines to plot a single point, together
        with (possibly)	a line extending to it from the last point, and
        a symbol. [Alternatively, plot_point_array() can be used, to plot
        an array of points.]
   end_graph()
   ..
   [The begin_graph()..end_graph() block can be repeated indefinitely
    if desired, to create a multigraph.  set_graph_parameters() allows
    for repositioning of later graphs.]
   ..
   delete_multigrapher() deletes the point plotter.

   There is also a function end_polyline_and_flush(), which is useful for
   real-time display. */
#include "arg.h" // TODO: remove
#include "extern.h"
#include "libcommon.h"
#include "plot.h"
#include "sys-defines.h"
/* #include <string.h> */

/* we use floating point libplot coordinates in the range [0,PLOT_SIZE] */
#define PLOT_SIZE 4096.0

#define FUZZ 0.000001 /* bd. on floating pt. roundoff error */

#define NEAR_EQUALITY(a, b, scale) (fabs ((a) - (b)) < (FUZZ * fabs (scale)))

typedef unsigned int outcode; /* for Cohen-Sutherland clipper */
enum
{
  TOP = 0x1,
  BOTTOM = 0x2,
  RIGHT = 0x4,
  LEFT = 0x8
};
enum
{
  ACCEPTED = 0x1,
  CLIPPED_FIRST = 0x2,
  CLIPPED_SECOND = 0x4
};

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

/* valid graph axis layout types; A_LOG2, anyone? */
#define A_LINEAR 0
#define A_LOG10 1

/* The x_trans and y_trans elements of a Multigrapher specify the current
   linear transformation from user coordinates to device coordinates.  They
   are used both in the plotting of a graph frame, and in the plotting of
   data points within a graph. */

typedef struct
{
  /* Input (user) coordinates, all floating point.  These are the
     coordinates used in the original data points (or their base-10 logs,
     for an axis of log type).  We'll map them to the unit interval
     [0.0,1.0]. */
  double input_min, input_max; /* min, max */
  double input_range;          /* max - min, precomputed for speed */
  /* If we're reversing axes, we'll then map [0.0,1.0] to [1.0,0.0] */
  bool reverse;
  /* We'll map [0.0,1.0] to another (smaller) interval, linearly */
  double squeezed_min, squeezed_max; /* min, max */
  double squeezed_range;             /* max - min */
  /* Output [i.e., libplot] coordinates.  The interval [0.0,1.0] will be
     mapped to this range, and the squeezed interval to a sub-range.  This
     is so that the box within which points are plotted will be smaller
     than the full area of the graphics display. */
  double output_min, output_max; /* min */
  double output_range;           /* max - min */
} Transform;

/* Affine transformation macros */

/* X Scale: convert from user x value to normalized x coordinate (floating
   point, 0.0 to 1.0). */
#define XS(x)                                                                 \
  (((x)-multigrapher->x_trans.input_min) / multigrapher->x_trans.input_range)
/* X Reflect: map [0,1] to [1,0], if that's called for */
#define XR(x) (multigrapher->x_trans.reverse ? 1.0 - (x) : (x))
/* X Squeeze: map [0,1] range for normalized x coordinate into a smaller
   interval, the x range for the plotting area within the graphics display */
#define XSQ(x)                                                                \
  (multigrapher->x_trans.squeezed_min                                         \
   + (x)*multigrapher->x_trans.squeezed_range)
/* X Plot: convert from normalized x coordinate to floating point libplot
   coordinate. */
#define XP(x)                                                                 \
  (multigrapher->x_trans.output_min + (x)*multigrapher->x_trans.output_range)
/* X Value: convert from user x value (floating point) to floating point
   libplot coordinate. */
#define XV(x) XP (XSQ (XR (XS (x))))
/* X Normalize: the same, but do not perform reflection if any.  (We use
   this for plotting of axes and their labels.) */
#define XN(y) XP (XSQ (XS (y)))

/* Y Scale: convert from user y value to normalized y coordinate (floating
   point, 0.0 to 1.0). */
#define YS(y)                                                                 \
  (((y)-multigrapher->y_trans.input_min) / multigrapher->y_trans.input_range)
/* Y Reflect: map [0,1] to [1,0], if that's called for */
#define YR(y) (multigrapher->y_trans.reverse ? 1.0 - (y) : (y))
/* Y Squeeze: map [0,1] range for normalized y coordinate into a smaller
   interval, the y range for the plotting area within the graphics display */
#define YSQ(y)                                                                \
  (multigrapher->y_trans.squeezed_min                                         \
   + (y)*multigrapher->y_trans.squeezed_range)
/* Y Plot: convert from normalized y coordinate to floating point libplot
   coordinate. */
#define YP(y)                                                                 \
  (multigrapher->y_trans.output_min + (y)*multigrapher->y_trans.output_range)
/* Y Value: convert from user y value (floating point) to floating point
   libplot coordinate.  (We use this for plotting of points.) */
#define YV(y) YP (YSQ (YR (YS (y))))
/* Y Normalize: the same, but do not perform reflection if any.  (We use
   this for plotting of axes and their labels.) */
#define YN(y) YP (YSQ (YS (y)))

/* Size Scale: convert distances, or sizes, from normalized coors to
   libplot coordinates.  (Used for tick, symbol, and font sizes.)  The min
   should really be precomputed. */
#define SS(x)                                                                 \
  (DMIN (multigrapher->x_trans.output_range                                   \
             * multigrapher->x_trans.squeezed_range,                          \
         multigrapher->y_trans.output_range                                   \
             * multigrapher->y_trans.squeezed_range)                          \
   * (x))

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

/* The Multigrapher structure.  A pointer to one of these is passed as the
   first argument to each Multigrapher method (e.g., plot_point()). */

struct MultigrapherStruct
{
  /* multigrapher parameters (not updated over a multigrapher's lifetime) */
  plPlotter *plotter;        /* GNU libplot Plotter handle */
  const char *output_format; /* type of libplot device driver [unused] */
  const char *bg_color;      /* color of background, if non-NULL */
  bool save_screen;          /* erase display when opening plotter? */
  /* graph parameters (constant over any single graph) */
  Transform x_trans, y_trans;  /* user->device coor transformations */
  Axis x_axis, y_axis;         /* information on each axis */
  grid_type grid_spec;         /* frame specification */
  double blankout_fraction;    /* 1.0 means blank whole box before drawing */
  bool no_rotate_y_label;      /* useful for pre-X11R6 X servers */
  double tick_size;            /* fractional tick size */
  double subtick_size;         /* fractional subtick size (for linear axes) */
  double frame_line_width;     /* fractional width of lines in the frame */
  double half_line_width;      /* approx. half of this, in libplot coors */
  const char *frame_color;     /* color for frame (and graph, if no -C) */
  const char *title;           /* graph title */
  const char *title_font_name; /* font for graph title */
  double title_font_size;      /* fractional height of graph title */
  int clip_mode;               /* 0, 1, or 2 (cf. clipping in gnuplot) */
  /* following elements are updated during plotting of points; they're the
     chief repository for internal state */
  bool first_point_of_polyline;  /* true only at beginning of each polyline */
  double oldpoint_x, oldpoint_y; /* last-plotted point */
  int symbol;                    /* symbol being plotted at each point */
  int linemode;                  /* linemode used for polyline */
  int graph_num;
  char *graph_symbols;
  float legend_pos;
  bool legend_plot;
};

/* forward references */
int clip_line (Multigrapher *multigrapher, double *x0_p, double *y0_p,
               double *x1_p, double *y1_p);
int spacing_type (double spacing);
outcode compute_outcode (Multigrapher *multigrapher, double x, double y,
                         bool tolerant);
void plot_abscissa_log_subsubtick (Multigrapher *multigrapher, double xval);
void plot_errorbar (Multigrapher *multigrapher, const Point *p);
void plot_ordinate_log_subsubtick (Multigrapher *multigrapher, double xval);
void prepare_axis (Axis *axisp, Transform *trans, double min, double max,
                   double spacing, const char *font_name, double font_size,
                   const char *label, double subsubtick_spacing,
                   bool user_specified_subsubticks, bool round_to_next_tick,
                   bool log_axis, bool reverse_axis, bool switch_axis_end,
                   bool omit_ticks);
void print_tick_label (char *labelbuf, const Axis *axis,
                       const Transform *transform, double val);
void scale1 (double min, double max, double *tick_spacing,
             int *tick_spacing_type);
void set_line_style (Multigrapher *multigrapher, int style, bool use_color);
void transpose_portmanteau (int *val);
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
