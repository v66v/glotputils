/* Copyright (C) 2024 by v66v <74927439\+v66v@users.noreply.github.com> */

#pragma once
#include "sys-defines.h"

/* Definition of the Point structure.  The point-reader (in reader.c)
   returns a list of these from a specified input stream, and the
   multigrapher (in plotter.c) interprets them as polyline vertices, and
   plots the resulting polyline(s).  Each polyline comprises a run of
   points, each (except the first) connected to the previous point,
   provided that `pendown' is true.  The final seven fields should be the
   same for each point in a polyline. */

typedef struct
{
  double x, y; /* location of the point in user coordinates */
  bool have_x_errorbar, have_y_errorbar;
  double xmin, xmax; /* meaningful only if have_x_errorbar field is set */
  double ymin, ymax; /* meaningful only if have_y_errorbar field is set */
  bool pendown;      /* connect to previous point? (if false, polyline ends) */
  /* following fields are polyline attributes: constant over a polyline */
  int symbol;         /* either a number indicating which standard marker
                       symbol is to be plotted at the point (<0 means none)
                       or an character to be plotted, depending on the value:
                       0-31: a marker number, or 32-up: a character. */
  double symbol_size; /* symbol size, as frac. of size of plotting area */
  const char *symbol_font_name; /* font from which symbols >= 32 are taken */
  int linemode;         /* linemode of polyline (<0 means no polyline) */
  double line_width;    /* line width as fraction of size of the display */
  double fill_fraction; /* in interval [0,1], <0 means polyline isn't filled */
  bool use_color;       /* color/monochrome interpretation of linemode */
} Point;

#define FUZZ 0.000001 /* bd. on floating pt. roundoff error */
#define NEAR_EQUALITY(a, b, scale) (fabs ((a) - (b)) < (FUZZ * fabs (scale)))
