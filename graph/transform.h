/* Copyright (C) 2024 by v66v <74927439+v66v@users.noreply.github.com> */
#pragma once
#include "sys-defines.h"

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
