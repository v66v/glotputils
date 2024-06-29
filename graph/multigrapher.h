/* Copyright (C) 2024 by v66v <74927439+v66v@users.noreply.github.com> */

#pragma once
#include "axis.h"
#include "plot.h"
#include "point.h"
#include "transform.h"

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

typedef struct MultigrapherStruct Multigrapher;

typedef unsigned int outcode; /* for Cohen-Sutherland clipper */
enum
{
  TOP = 0x1,
  BOTTOM = 0x2,
  RIGHT = 0x4,
  LEFT = 0x8
};

void plot_errorbar (Multigrapher *multigrapher, const Point *p);
void plot_abscissa_log_subsubtick (Multigrapher *multigrapher, double xval);
void plot_ordinate_log_subsubtick (Multigrapher *multigrapher, double yval);
outcode compute_outcode (Multigrapher *multigrapher, double x, double y,
                         bool tolerant);
