#include "plotter.h"

/* The following routines [new_multigrapher(), begin_graph(),
 * set_graph_parameters(), draw_frame_of_graph(), plot_point(),
 * end_graph(), delete_multigrapher()] are the basic routines of the
 * point-plotter .  See descriptions at the head of this file.  */

/* Create a new Multigrapher.  The arguments, after the first, are the
   libplot Plotter parameters that the `graph' user can set on the command
   line. */

Multigrapher *
new_multigrapher (const char *output_format, const char *bg_color,
                  const char *bitmap_size, const char *emulate_color,
                  const char *max_line_length, const char *meta_portable,
                  const char *page_size, const char *rotation_angle,
                  bool save_screen, bool legend_plot)
{
  plPlotterParams *plotter_params;
  plPlotter *plotter;
  Multigrapher *multigrapher;

  multigrapher = (Multigrapher *)xmalloc (sizeof (Multigrapher));

  /* set Plotter parameters */
  plotter_params = pl_newplparams ();
  pl_setplparam (plotter_params, "BG_COLOR", (void *)bg_color);
  pl_setplparam (plotter_params, "BITMAPSIZE", (void *)bitmap_size);
  pl_setplparam (plotter_params, "EMULATE_COLOR", (void *)emulate_color);
  pl_setplparam (plotter_params, "MAX_LINE_LENGTH", (void *)max_line_length);
  pl_setplparam (plotter_params, "META_PORTABLE", (void *)meta_portable);
  pl_setplparam (plotter_params, "PAGESIZE", (void *)page_size);
  pl_setplparam (plotter_params, "ROTATION", (void *)rotation_angle);

  /* create Plotter and open it */
  plotter = pl_newpl_r (output_format, NULL, stdout, stderr, plotter_params);
  if (plotter == (plPlotter *)NULL)
    return (Multigrapher *)NULL;
  pl_deleteplparams (plotter_params);
  multigrapher->plotter = plotter;
  if (pl_openpl_r (plotter) < 0)
    return (Multigrapher *)NULL;
  multigrapher->bg_color = bg_color;

  /* if called for, erase it; set up the user->device coor map */
  if (!save_screen || bg_color)
    pl_erase_r (plotter);
  pl_fspace_r (plotter, 0.0, 0.0, (double)PLOT_SIZE, (double)PLOT_SIZE);

  multigrapher->graph_num = 0;
  multigrapher->legend_plot = legend_plot;
  multigrapher->graph_symbols = (char *)xmalloc (6 * sizeof (char));
  strcpy (multigrapher->graph_symbols, "12345");
  return multigrapher;
}

int
delete_multigrapher (Multigrapher *multigrapher)
{
  int retval;

  retval = pl_closepl_r (multigrapher->plotter);
  if (retval >= 0)
    retval = pl_deletepl_r (multigrapher->plotter);

  free (multigrapher->graph_symbols);
  free (multigrapher);
  return retval;
}

void
begin_graph (Multigrapher *multigrapher, double scale, double trans_x,
             double trans_y)
{
  pl_savestate_r (multigrapher->plotter);
  pl_fconcat_r (multigrapher->plotter, scale, 0.0, 0.0, scale,
                trans_x * PLOT_SIZE, trans_y * PLOT_SIZE);
}

void
end_graph (Multigrapher *multigrapher)
{
  pl_restorestate_r (multigrapher->plotter);
}

/* ARGS:
     Multigrapher *multigrapher
     double frame_line_width 	= fractional width of lines in the frame
     const char *frame_color 	= color for frame (and plot if no -C option)
     const char *title 		= graph title
     const char *title_font_name = font for graph title (string)
     double title_font_size 	= font size for graph title
     double tick_size 		= fractional size of ticks
     grid_type grid_spec 	= gridstyle (and tickstyle) spec
     double x_min, x_max, x_spacing
     double y_min, y_max, y_spacing
     bool spec_x_spacing
     bool spec_y_spacing
     double width, height, up, right
     const char *x_font_name
     double x_font_size
     const char *x_label
     const char *y_font_name
     double y_font_size
     const char *y_label
     bool no_rotate_y_label

     -- portmanteaux args --
     int log_axis		= whether axes should be logarithmic
     int round_to_next_tick	= round limits to the next tick mark
     int switch_axis_end	= put axes at right/top, not left/bottom?
     int omit_ticks		= omit all ticks, tick labels from an axis?
     -- other args --
     int clip_mode		= 0, 1, or 2
     double blankout_fraction	= 1.0 means blank out whole box before plot
     bool transpose_axes */
void
set_graph_parameters (
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
    char legend_position, bool legend_plot)
{
  double x_subsubtick_spacing = 0.0, y_subsubtick_spacing = 0.0;
  /* local portmanteau variables */
  int reverse_axis = 0;               /* min > max on an axis? */
  int user_specified_subsubticks = 0; /* i.e. linear ticks on a log axis? */

  if (log_axis & X_AXIS)
    {
      if (spec_x_spacing)
        /* spacing is handled specially for log axes */
        {
          spec_x_spacing = false;
          user_specified_subsubticks |= X_AXIS;
          x_subsubtick_spacing = x_spacing;
        }
    }

  if (log_axis & Y_AXIS)
    {
      if (spec_y_spacing)
        {
          /* spacing is handled specially for log axes */
          spec_y_spacing = false;
          user_specified_subsubticks |= Y_AXIS;
          y_subsubtick_spacing = y_spacing;
        }
    }

  /* check for reversed axes (min > max) */
  if (x_max < x_min)
    {
      reverse_axis |= X_AXIS;
      {
        double temp;

        temp = x_min;
        x_min = x_max;
        x_max = temp;
      }
    }
  if (x_max == x_min)
    {
      fprintf (stderr,
               "%s: identical upper and lower x limits are separated\n",
               progname);
      /* separate them */
      x_max += 1.0;
      x_min -= 1.0;
    }
  /* check for reversed axes (min > max) */
  if (y_max < y_min)
    {
      reverse_axis |= Y_AXIS;
      {
        double temp;

        temp = y_min;
        y_min = y_max;
        y_max = temp;
      }
    }
  if (y_max == y_min)
    {
      fprintf (stderr,
               "%s: identical upper and lower y limits are separated\n",
               progname);
      /* separate them */
      y_max += 1.0;
      y_min -= 1.0;
    }

  /* At this point, min < max for each axis, if the user specified the two
     limits on an axis; reverse_axis portmanteau variable keeps track of
     whether either axis was discovered to be reversed. */

  /* silently accept negative spacing as equivalent as positive */
  if (spec_x_spacing)
    {
      if (x_spacing == 0.0)
        {
          fprintf (stderr,
                   "%s: error: the spacing between ticks on an axis is zero\n",
                   progname);
          exit (EXIT_FAILURE);
        }
      x_spacing = fabs (x_spacing);
    }
  if (spec_y_spacing)
    {
      if (y_spacing == 0.0)
        {
          fprintf (stderr,
                   "%s: error: the spacing between ticks on an axis is zero\n",
                   progname);
          exit (EXIT_FAILURE);
        }
      y_spacing = fabs (y_spacing);
    }

  /* now transpose the two axes (i.e. their portmanteau variables, labels,
     limits etc.) if transpose_axes was set */
  if (transpose_axes)
    {
      const char *temp_string;
      double temp_double;

      transpose_portmanteau (&log_axis);
      transpose_portmanteau (&round_to_next_tick);
      transpose_portmanteau (&switch_axis_end);
      transpose_portmanteau (&omit_ticks);

      transpose_portmanteau (&reverse_axis);
      transpose_portmanteau (&user_specified_subsubticks);

      temp_string = x_label;
      x_label = y_label;
      y_label = temp_string;

      temp_double = x_min;
      x_min = y_min;
      y_min = temp_double;

      temp_double = x_max;
      x_max = y_max;
      y_max = temp_double;

      temp_double = x_spacing;
      x_spacing = y_spacing;
      y_spacing = temp_double;

      temp_double = x_subsubtick_spacing;
      x_subsubtick_spacing = y_subsubtick_spacing;
      y_subsubtick_spacing = temp_double;
    }

  /* fill in the Multigrapher struct */

  multigrapher->frame_line_width = frame_line_width;
  multigrapher->frame_color = frame_color;
  multigrapher->no_rotate_y_label = no_rotate_y_label;
  multigrapher->blankout_fraction = blankout_fraction;

  if (title != NULL)
    multigrapher->title = xstrdup (title);
  else
    multigrapher->title = NULL;
  if (title_font_name != NULL)
    multigrapher->title_font_name = xstrdup (title_font_name);
  else
    multigrapher->title_font_name = NULL;
  multigrapher->title_font_size = title_font_size;
  multigrapher->tick_size = tick_size;
  multigrapher->subtick_size = RELATIVE_SUBTICK_SIZE * tick_size;
  multigrapher->grid_spec = grid_spec;
  multigrapher->clip_mode = clip_mode;

  /* fill in the Transform and Axis elements for each coordinate */
  prepare_axis (&multigrapher->x_axis, &multigrapher->x_trans, x_min, x_max,
                x_spacing, x_font_name, x_font_size, x_label,
                x_subsubtick_spacing,
                (bool)(user_specified_subsubticks & X_AXIS),
                (bool)(round_to_next_tick & X_AXIS), (bool)(log_axis & X_AXIS),
                (bool)(reverse_axis & X_AXIS),
                (bool)(switch_axis_end & X_AXIS), (bool)(omit_ticks & X_AXIS));
  prepare_axis (&multigrapher->y_axis, &multigrapher->y_trans, y_min, y_max,
                y_spacing, y_font_name, y_font_size, y_label,
                y_subsubtick_spacing,
                (bool)(user_specified_subsubticks & Y_AXIS),
                (bool)(round_to_next_tick & Y_AXIS), (bool)(log_axis & Y_AXIS),
                (bool)(reverse_axis & Y_AXIS),
                (bool)(switch_axis_end & Y_AXIS), (bool)(omit_ticks & Y_AXIS));

  /* fill in additional parameters in the two Transform structures */
  multigrapher->x_trans.squeezed_min = right;
  multigrapher->x_trans.squeezed_max = right + width;
  multigrapher->x_trans.squeezed_range = width;
  multigrapher->y_trans.squeezed_min = up;
  multigrapher->y_trans.squeezed_max = up + height;
  multigrapher->y_trans.squeezed_range = height;

  /* specify interval range for each coordinate, in libplot units */
  multigrapher->x_trans.output_min = 0.0;
  multigrapher->x_trans.output_max = (double)PLOT_SIZE;
  multigrapher->x_trans.output_range
      = multigrapher->x_trans.output_max - multigrapher->x_trans.output_min;
  multigrapher->x_trans.output_min = 0.0;
  multigrapher->y_trans.output_max = (double)PLOT_SIZE;
  multigrapher->y_trans.output_range
      = multigrapher->y_trans.output_max - multigrapher->y_trans.output_min;

  /* fill in fields in Axis structs dealing with location of other axis */
  if (multigrapher->grid_spec != AXES_AT_ORIGIN)
    /* Normal case */
    {
      /* axes are at left/bottom */
      multigrapher->x_axis.other_axis_loc = multigrapher->x_trans.input_min;
      multigrapher->y_axis.other_axis_loc = multigrapher->y_trans.input_min;
      /* secondary axes (used only if --switch-axis-end is specified) */
      multigrapher->x_axis.alt_other_axis_loc
          = multigrapher->x_trans.input_max;
      multigrapher->y_axis.alt_other_axis_loc
          = multigrapher->y_trans.input_max;
    }
  else
    /* Special case: grid type #4, AXES_AT_ORIGIN */
    {
      /* In this case (grid type #4), we don't allow the user to move the
         axis position by using the --switch-axis-end option.  Each axis is
         at the value 0 (the origin) if the value 0 is between the limits
         of the opposing axis.  Otherwise, the position is at the end
         closer to the value of 0. */
      multigrapher->x_axis.other_axis_loc
          = (multigrapher->x_trans.input_min * multigrapher->x_trans.input_max
             <= 0.0)
                ? 0.0
                : (multigrapher->x_trans.input_min > 0.0
                       ? multigrapher->x_trans.input_min
                       : multigrapher->x_trans.input_max);
      multigrapher->y_axis.other_axis_loc
          = (multigrapher->y_trans.input_min * multigrapher->y_trans.input_max
             <= 0.0)
                ? 0.0
                : (multigrapher->y_trans.input_min > 0.0
                       ? multigrapher->y_trans.input_min
                       : multigrapher->y_trans.input_max);
      /* secondary axes are the same */
      multigrapher->x_axis.alt_other_axis_loc
          = multigrapher->x_axis.other_axis_loc;
      multigrapher->y_axis.alt_other_axis_loc
          = multigrapher->y_axis.other_axis_loc;
      multigrapher->x_axis.switch_axis_end
          = (((multigrapher->x_trans.input_max
               - multigrapher->x_axis.other_axis_loc)
              < (multigrapher->x_axis.other_axis_loc
                 - multigrapher->x_trans.input_min))
                 ? true
                 : false);
      multigrapher->y_axis.switch_axis_end
          = (((multigrapher->y_trans.input_max
               - multigrapher->y_axis.other_axis_loc)
              < (multigrapher->y_axis.other_axis_loc
                 - multigrapher->y_trans.input_min))
                 ? true
                 : false);
    }

  /* The following is a version of (multigrapher->frame_line_width)/2
     (expressed in terms of libplot coordinates) which the plotter uses as
     an offset, to get highly accurate positioning of ticks and labels. */
  if (frame_line_width < 0.0
      || pl_havecap_r (multigrapher->plotter, "WIDE_LINES") == 0)
    multigrapher->half_line_width
        = 0.0; /* N.B. <0.0 -> default width, pres. small */
  else
    multigrapher->half_line_width
        = 0.5 * frame_line_width * multigrapher->x_trans.output_range;

  /* initialize the plotter state variables */
  multigrapher->first_point_of_polyline = true;
  multigrapher->oldpoint_x = 0.0;
  multigrapher->oldpoint_y = 0.0;

  if (legend_plot)
    {
      switch (legend_position)
        {
        case 'c':
          multigrapher->legend_pos = 0.5;
          break;
        case 'r':
          multigrapher->legend_pos = 0.8;
          break;
        case 'l':
          multigrapher->legend_pos = 0.2;
          break;
        default:
          multigrapher->legend_pos = 0.2;
          fprintf (stderr,
                   "Incorrect legend position type, defaulting to: l\n");
        }
    }
}

/* draw_frame_of_graph() plots the graph frame (grid plus axis labels and
   title), using the multigrapher's variables (the multigrapher->x_axis,
   multigrapher->y_axis, multigrapher->x_trans, multigrapher->y_trans
   structures).  It comprises several almost-independent tasks:

   0. draw opaque white rectangle (``canvas''), if requested
   1. draw title, if any, above drawing box
   2. draw axes, and a full drawing box if requested
   3. draw ticks, grid lines, and tick labels on abscissa;
      also subticks, if abscissa is a linear axis
   4. draw ticks, grid lines, and tick labels on ordinate,
      also subticks, if ordinate is a linear axis
   5. draw sub-tick marks, sub-grid lines, and sub-labels,
      if abscissa is a logarithmic axis
   6. draw sub-tick marks, sub-grid lines, and sub-labels,
      if ordinate is a logarithmic axis
   7. draw axis label on abscissa
   8. draw axis label on ordinate (normally rotated 90 degrees)

   A savestate()--restorestate() pair is wrapped around these nine tasks.
   They are not quite independent because in (4) and (6) we keep track of
   the maximum width of the tick labels on the ordinate, to position the
   rotated ordinate label properly in (8).

   At the conclusion of the eight tasks, we warn the user if (i) he/she
   didn't specify a tick spacing for a logarithmic axis by hand, and (ii)
   our default algorithm for drawing ticks on a logarithmic axis has drawn
   too few ticks (i.e. <= 2 ticks) on the axis.

   Task #0 (drawing a white canvas) isn't always performed; only if the
   argument draw_canvas is set.  It isn't set when we call this function
   for the first time; see graph.c.  */

void
draw_legend_of_graph (Multigrapher *multigrapher)
{
  // TODO: add fill behind the legend
  /* pl_colorname_r (multigrapher->plotter, "grey"); */

  /* pl_filltype_r (multigrapher->plotter, 1);	/\* turn on filling *\/ */

  /* pl_colorname_r (multigrapher->plotter, "grey"); */
  /* pl_linemod_r (multigrapher->plotter, "solid"); */
  /* pl_filltype_r (multigrapher->plotter, 1); */
  /* pl_fbox_r (multigrapher->plotter, */
  /* 			 XP(XSQ(multigrapher->legend_pos - 0.11 *
   * multigrapher->blankout_fraction)), */
  /* 			 YP(YSQ(0.8 - 0.1 * multigrapher->blankout_fraction)),
   */
  /* 			 XP(XSQ(multigrapher->legend_pos + 0.11 *
   * multigrapher->blankout_fraction)), */
  /* 			 YP(YSQ(0.8 + 0.032 * multigrapher->graph_num *
   * multigrapher->blankout_fraction))); */

  pl_colorname_r (multigrapher->plotter, "black");
  pl_filltype_r (multigrapher->plotter, 0);
  pl_fbox_r (multigrapher->plotter,
             XP (XSQ (multigrapher->legend_pos
                      - 0.11 * multigrapher->blankout_fraction)),
             YP (YSQ (0.8 + 0.1 * multigrapher->blankout_fraction)),
             XP (XSQ (multigrapher->legend_pos
                      + 0.11 * multigrapher->blankout_fraction)),
             YP (YSQ (0.865 - 0.064 * multigrapher->graph_num)));

  pl_fontname_r (multigrapher->plotter, multigrapher->title_font_name);
  double title_font_size = pl_ffontsize_r (
      multigrapher->plotter, SS (multigrapher->title_font_size * 0.5));
  pl_fmove_r (multigrapher->plotter, XP (XSQ (multigrapher->legend_pos)),
              YP (YSQ (0.85
                       + (((multigrapher->grid_spec == AXES_AND_BOX
                            || multigrapher->grid_spec == AXES)
                                   && (multigrapher->tick_size <= 0.0)
                               ? 1.0
                               : 0.5)
                          * fabs (multigrapher->tick_size))))
                  + 0.65 * title_font_size + multigrapher->half_line_width);
  /* title centered, bottom spec'd */
  pl_alabel_r (multigrapher->plotter, 'c', 'b', "Legend");
}

void
draw_frame_of_graph (Multigrapher *multigrapher, bool draw_canvas)
{
  static bool tick_warning_printed = false; /* when too few labelled ticks */

  /* wrap savestate()--restorestate() around all 9 tasks */
  pl_savestate_r (multigrapher->plotter);

  /* set color for graph frame */
  if (multigrapher->frame_color)
    pl_pencolorname_r (multigrapher->plotter, multigrapher->frame_color);

  /* set line width as a fraction of size of display, <0.0 means default */
  pl_flinewidth_r (multigrapher->plotter,
                   multigrapher->frame_line_width * (double)PLOT_SIZE);

  /* axes (or box) will be drawn in solid line style */
  pl_linemod_r (multigrapher->plotter, "solid");

  /* turn off filling */
  pl_filltype_r (multigrapher->plotter, 0);

  /* 0.  DRAW AN OPAQUE WHITE BOX */

  if (draw_canvas)
    {
      pl_savestate_r (multigrapher->plotter);
      /* use user-specified background color (if any) instead of white */
      if (pl_havecap_r (multigrapher->plotter, "SETTABLE_BACKGROUND") != 0
          && multigrapher->bg_color)
        pl_colorname_r (multigrapher->plotter, multigrapher->bg_color);
      else
        pl_colorname_r (multigrapher->plotter, "white");

      pl_filltype_r (multigrapher->plotter, 1); /* turn on filling */
      pl_fbox_r (multigrapher->plotter,
                 XP (XSQ (0.5 - 0.5 * multigrapher->blankout_fraction)),
                 YP (YSQ (0.5 - 0.5 * multigrapher->blankout_fraction)),
                 XP (XSQ (0.5 + 0.5 * multigrapher->blankout_fraction)),
                 YP (YSQ (0.5 + 0.5 * multigrapher->blankout_fraction)));
      pl_restorestate_r (multigrapher->plotter);
    }

  /* 1.  DRAW THE TITLE, I.E. THE TOP LABEL */

  if (multigrapher->grid_spec != NO_AXES
      && !multigrapher->y_axis
              .switch_axis_end /* no title if x axis is at top of plot */
      && multigrapher->title != NULL && *multigrapher->title != '\0')
    {
      double title_font_size;

      /* switch to our font for drawing title */
      pl_fontname_r (multigrapher->plotter, multigrapher->title_font_name);
      title_font_size = pl_ffontsize_r (multigrapher->plotter,
                                        SS (multigrapher->title_font_size));

      pl_fmove_r (multigrapher->plotter, XP (XSQ (0.5)),
                  YP (YSQ (1.0
                           + (((multigrapher->grid_spec == AXES_AND_BOX
                                || multigrapher->grid_spec == AXES)
                                       && (multigrapher->tick_size <= 0.0)
                                   ? 1.0
                                   : 0.5)
                              * fabs (multigrapher->tick_size))))
                      + 0.65 * title_font_size
                      + multigrapher->half_line_width);
      /* title centered, bottom spec'd */
      pl_alabel_r (multigrapher->plotter, 'c', 'b', multigrapher->title);
    }

  /* 2.  DRAW AXES FOR THE PLOT */

  switch (multigrapher->grid_spec)
    {
    case AXES_AND_BOX_AND_GRID:
    case AXES_AND_BOX:
      /* draw a box, not just a pair of axes */
      pl_fbox_r (multigrapher->plotter, XP (XSQ (0.0)), YP (YSQ (0.0)),
                 XP (XSQ (1.0)), YP (YSQ (1.0)));
      break;
    case AXES:
      {
        double xstart, ystart, xmid, ymid, xend, yend;

        xstart = (multigrapher->x_axis.switch_axis_end
                      ? XN (multigrapher->x_axis.other_axis_loc)
                            - multigrapher->half_line_width
                      : XN (multigrapher->x_axis.alt_other_axis_loc)
                            + multigrapher->half_line_width);
        ystart = (multigrapher->y_axis.switch_axis_end
                      ? YN (multigrapher->y_axis.alt_other_axis_loc)
                      : YN (multigrapher->y_axis.other_axis_loc));
        xmid = (multigrapher->x_axis.switch_axis_end
                    ? XN (multigrapher->x_axis.alt_other_axis_loc)
                    : XN (multigrapher->x_axis.other_axis_loc));
        ymid = ystart;
        xend = xmid;
        yend = (multigrapher->y_axis.switch_axis_end
                    ? YN (multigrapher->y_axis.other_axis_loc)
                          - multigrapher->half_line_width
                    : YN (multigrapher->y_axis.alt_other_axis_loc)
                          + multigrapher->half_line_width);

        pl_fmove_r (multigrapher->plotter, xstart, ystart);
        pl_fcont_r (multigrapher->plotter, xmid, ymid);
        pl_fcont_r (multigrapher->plotter, xend, yend);
      }
      break;
    case AXES_AT_ORIGIN:
      {
        double xpos, ypos;

        xpos = (multigrapher->x_axis.switch_axis_end
                    ? XN (multigrapher->x_axis.other_axis_loc)
                    : XN (multigrapher->x_axis.alt_other_axis_loc));
        ypos = (multigrapher->y_axis.switch_axis_end
                    ? YN (multigrapher->y_axis.alt_other_axis_loc)
                    : YN (multigrapher->y_axis.other_axis_loc));

        pl_fline_r (multigrapher->plotter, xpos,
                    YP (YSQ (0.0)) - multigrapher->half_line_width, xpos,
                    YP (YSQ (1.0)) + multigrapher->half_line_width);
        pl_fline_r (multigrapher->plotter,
                    XP (XSQ (0.0)) - multigrapher->half_line_width, ypos,
                    XP (XSQ (1.0)) + multigrapher->half_line_width, ypos);
      }
      break;
    case NO_AXES:
    default:
      break;
    }

  /* 3.  PLOT TICK MARKS, GRID LINES, AND TICK LABELS ON ABSCISSA */

  if (multigrapher->grid_spec != NO_AXES && !multigrapher->x_axis.omit_ticks
      && !multigrapher->x_axis.user_specified_subsubticks)
    {
      int i;
      double xval, xrange = multigrapher->x_trans.input_max
                            - multigrapher->x_trans.input_min;
      /* there is no way you could use longer labels on tick marks! */
      char labelbuf[2048];

      /* switch to our font for drawing x axis label and tick labels */
      pl_fontname_r (multigrapher->plotter, multigrapher->x_axis.font_name);
      pl_ffontsize_r (multigrapher->plotter,
                      SS (multigrapher->x_axis.font_size));

      for (i = multigrapher->x_axis.min_tick_count;
           i <= multigrapher->x_axis.max_tick_count; i++)
        /* tick range can be empty */
        {
          xval = i * multigrapher->x_axis.tick_spacing;

          /* discard tick locations outside plotting area */
          if (xval < multigrapher->x_trans.input_min - FUZZ * xrange
              || xval > multigrapher->x_trans.input_max + FUZZ * xrange)
            continue;

          /* Plot the abscissa tick labels. */
          if (!multigrapher->y_axis.switch_axis_end
              && !(multigrapher->grid_spec == AXES_AT_ORIGIN
                   /* don't plot label if it could run into an axis */
                   && NEAR_EQUALITY (xval, multigrapher->x_axis.other_axis_loc,
                                     multigrapher->x_trans.input_range)
                   && (multigrapher->y_axis.other_axis_loc
                       != multigrapher->y_trans.input_min)
                   && (multigrapher->y_axis.other_axis_loc
                       != multigrapher->y_trans.input_max)))
            /* print labels below bottom boundary */
            {
              pl_fmove_r (
                  multigrapher->plotter, XV (xval),
                  YN (multigrapher->y_axis.other_axis_loc)
                      - (SS ((multigrapher->tick_size >= 0.0 ? 0.75 : 1.75)
                             * fabs (multigrapher->tick_size))
                         + multigrapher->half_line_width));
              print_tick_label (
                  labelbuf, &multigrapher->x_axis, &multigrapher->x_trans,
                  (multigrapher->x_axis.type == A_LOG10) ? pow (10.0, xval)
                                                         : xval);
              pl_alabel_r (multigrapher->plotter, 'c', 't', labelbuf);
              multigrapher->x_axis.labelled_ticks++;
            }
          else
            /* print labels above top boundary */
            if (multigrapher->y_axis.switch_axis_end
                && !(multigrapher->grid_spec == AXES_AT_ORIGIN
                     /* don't plot label if it could run into an axis */
                     && NEAR_EQUALITY (xval,
                                       multigrapher->x_axis.other_axis_loc,
                                       multigrapher->x_trans.input_range)
                     && (multigrapher->y_axis.other_axis_loc
                         != multigrapher->y_trans.input_min)
                     && (multigrapher->y_axis.other_axis_loc
                         != multigrapher->y_trans.input_max)))
              {
                pl_fmove_r (
                    multigrapher->plotter, XV (xval),
                    YN (multigrapher->y_axis.alt_other_axis_loc)
                        + (SS ((multigrapher->tick_size >= 0.0 ? 0.75 : 1.75)
                               * fabs (multigrapher->tick_size))
                           + multigrapher->half_line_width));
                print_tick_label (
                    labelbuf, &multigrapher->x_axis, &multigrapher->x_trans,
                    (multigrapher->x_axis.type == A_LOG10) ? pow (10.0, xval)
                                                           : xval);
                pl_alabel_r (multigrapher->plotter, 'c', 'b', labelbuf);
                multigrapher->x_axis.labelled_ticks++;
              }

          /* Plot the abscissa tick marks, and vertical grid lines. */
          switch (multigrapher->grid_spec)
            {
            case AXES_AND_BOX_AND_GRID:
              pl_linemod_r (multigrapher->plotter, "dotted");
              pl_fmove_r (multigrapher->plotter, XV (xval), YP (YSQ (0.0)));
              pl_fcont_r (multigrapher->plotter, XV (xval), YP (YSQ (1.0)));
              pl_linemod_r (multigrapher->plotter, "solid");
              /* fall through */
            case AXES_AND_BOX:
              if (!multigrapher->y_axis.switch_axis_end)
                {
                  pl_fmove_r (multigrapher->plotter, XV (xval),
                              YN (multigrapher->y_axis.alt_other_axis_loc));
                  pl_fcont_r (
                      multigrapher->plotter, XV (xval),
                      YN (multigrapher->y_axis.alt_other_axis_loc)
                          - (SS (multigrapher->tick_size)
                             + (multigrapher->tick_size > 0.0
                                    ? multigrapher->half_line_width
                                    : -multigrapher->half_line_width)));
                }
              else
                {
                  pl_fmove_r (multigrapher->plotter, XV (xval),
                              YN (multigrapher->y_axis.other_axis_loc));
                  pl_fcont_r (
                      multigrapher->plotter, XV (xval),
                      YN (multigrapher->y_axis.other_axis_loc)
                          + (SS (multigrapher->tick_size)
                             + (multigrapher->tick_size > 0.0
                                    ? multigrapher->half_line_width
                                    : -multigrapher->half_line_width)));
                }
              /* fall through */
            case AXES:
            case AXES_AT_ORIGIN:
              if (!multigrapher->y_axis.switch_axis_end)
                {
                  pl_fmove_r (multigrapher->plotter, XV (xval),
                              YN (multigrapher->y_axis.other_axis_loc));
                  pl_fcont_r (
                      multigrapher->plotter, XV (xval),
                      YN (multigrapher->y_axis.other_axis_loc)
                          + (SS (multigrapher->tick_size)
                             + (multigrapher->tick_size > 0.0
                                    ? multigrapher->half_line_width
                                    : -multigrapher->half_line_width)));
                }
              else
                {
                  pl_fmove_r (multigrapher->plotter, XV (xval),
                              YN (multigrapher->y_axis.alt_other_axis_loc));
                  pl_fcont_r (
                      multigrapher->plotter, XV (xval),
                      YN (multigrapher->y_axis.alt_other_axis_loc)
                          - (SS (multigrapher->tick_size)
                             + (multigrapher->tick_size > 0.0
                                    ? multigrapher->half_line_width
                                    : -multigrapher->half_line_width)));
                }
              break;
            default: /* shouldn't happen */
              break;
            }
        }

      if (multigrapher->x_axis.have_lin_subticks)
        {
          double subtick_size; /* libplot coordinates */

          /* linearly spaced subticks on log axes are as long as reg. ticks */
          subtick_size = (multigrapher->x_axis.type == A_LOG10
                              ? SS (multigrapher->tick_size)
                              : SS (multigrapher->subtick_size));

          /* Plot the linearly spaced subtick marks on the abscissa */
          for (i = multigrapher->x_axis.min_lin_subtick_count;
               i <= multigrapher->x_axis.max_lin_subtick_count; i++)
            /* tick range can be empty */
            {
              xval = i * multigrapher->x_axis.lin_subtick_spacing;

              /* discard subtick locations outside plotting area */
              if (xval < multigrapher->x_trans.input_min - FUZZ * xrange
                  || xval > multigrapher->x_trans.input_max + FUZZ * xrange)
                continue;

              switch (multigrapher->grid_spec)
                {
                case AXES_AND_BOX_AND_GRID:
                case AXES_AND_BOX:
                  /* draw on both sides */
                  if (!multigrapher->y_axis.switch_axis_end)
                    {
                      pl_fmove_r (
                          multigrapher->plotter, XV (xval),
                          YN (multigrapher->y_axis.alt_other_axis_loc));
                      pl_fcont_r (
                          multigrapher->plotter, XV (xval),
                          YN (multigrapher->y_axis.alt_other_axis_loc)
                              - (subtick_size
                                 + (subtick_size > 0.0
                                        ? multigrapher->half_line_width
                                        : -multigrapher->half_line_width)));
                    }
                  else
                    {
                      pl_fmove_r (multigrapher->plotter, XV (xval),
                                  YN (multigrapher->y_axis.other_axis_loc));
                      pl_fcont_r (
                          multigrapher->plotter, XV (xval),
                          YN (multigrapher->y_axis.other_axis_loc)
                              + (subtick_size
                                 + (subtick_size > 0.0
                                        ? multigrapher->half_line_width
                                        : -multigrapher->half_line_width)));
                    }
                  /* fall through */
                case AXES:
                case AXES_AT_ORIGIN:
                  if (!multigrapher->y_axis.switch_axis_end)
                    /* draw on only one side */
                    {
                      pl_fmove_r (multigrapher->plotter, XV (xval),
                                  YN (multigrapher->y_axis.other_axis_loc));
                      pl_fcont_r (
                          multigrapher->plotter, XV (xval),
                          YN (multigrapher->y_axis.other_axis_loc)
                              + (subtick_size
                                 + (subtick_size > 0.0
                                        ? multigrapher->half_line_width
                                        : -multigrapher->half_line_width)));
                    }
                  else
                    {
                      pl_fmove_r (
                          multigrapher->plotter, XV (xval),
                          YN (multigrapher->y_axis.alt_other_axis_loc));
                      pl_fcont_r (
                          multigrapher->plotter, XV (xval),
                          YN (multigrapher->y_axis.alt_other_axis_loc)
                              - (subtick_size
                                 + (subtick_size > 0.0
                                        ? multigrapher->half_line_width
                                        : -multigrapher->half_line_width)));
                    }
                  break;
                default: /* shouldn't happen */
                  break;
                }
            }
        }

      /* plot a vertical dotted line at x = 0 */
      if (multigrapher->grid_spec != AXES_AT_ORIGIN
          && multigrapher->x_axis.type == A_LINEAR
          && multigrapher->x_trans.input_min * multigrapher->x_trans.input_max
                 < 0.0)
        {
          pl_linemod_r (multigrapher->plotter, "dotted");
          pl_fline_r (multigrapher->plotter, XV (0.0), YP (YSQ (0.0)),
                      XV (0.0), YP (YSQ (1.0)));
          pl_linemod_r (multigrapher->plotter, "solid");
        }
    }

  /* 4.  PLOT TICK MARKS, GRID LINES, AND TICK LABELS ON ORDINATE */

  if (multigrapher->grid_spec != NO_AXES && !multigrapher->y_axis.omit_ticks
      && !multigrapher->y_axis.user_specified_subsubticks)
    {
      int i;
      double yval, yrange = multigrapher->y_trans.input_max
                            - multigrapher->y_trans.input_min;
      /* there is no way you could use longer labels on tick marks! */
      char labelbuf[2048];

      /* switch to our font for drawing y axis label and tick labels */
      pl_fontname_r (multigrapher->plotter, multigrapher->y_axis.font_name);
      pl_ffontsize_r (multigrapher->plotter,
                      SS (multigrapher->y_axis.font_size));

      for (i = multigrapher->y_axis.min_tick_count;
           i <= multigrapher->y_axis.max_tick_count; i++)
        /* range can be empty */
        {
          yval = i * multigrapher->y_axis.tick_spacing;

          /* discard tick locations outside plotting area */
          if (yval < multigrapher->y_trans.input_min - FUZZ * yrange
              || yval > multigrapher->y_trans.input_max + FUZZ * yrange)
            continue;

          /* Plot the ordinate tick labels. */
          if (!multigrapher->x_axis.switch_axis_end
              && !(multigrapher->grid_spec == AXES_AT_ORIGIN
                   /* don't plot label if it could run into an axis */
                   && NEAR_EQUALITY (yval, multigrapher->y_axis.other_axis_loc,
                                     multigrapher->y_trans.input_range)
                   && (multigrapher->x_axis.other_axis_loc
                       != multigrapher->x_trans.input_min)
                   && (multigrapher->x_axis.other_axis_loc
                       != multigrapher->x_trans.input_max)))
            /* print labels to left of left boundary */
            {
              double new_width;

              pl_fmove_r (
                  multigrapher->plotter,
                  XN (multigrapher->x_axis.other_axis_loc)
                      - (SS ((multigrapher->tick_size >= 0.0 ? 0.75 : 1.75)
                             * fabs (multigrapher->tick_size))
                         + multigrapher->half_line_width),
                  YV (yval));
              print_tick_label (
                  labelbuf, &multigrapher->y_axis, &multigrapher->y_trans,
                  (multigrapher->y_axis.type == A_LOG10) ? pow (10.0, yval)
                                                         : yval);
              new_width = pl_flabelwidth_r (multigrapher->plotter, labelbuf);
              pl_alabel_r (multigrapher->plotter, 'r', 'c', labelbuf);
              multigrapher->y_axis.max_label_width
                  = DMAX (multigrapher->y_axis.max_label_width, new_width);
              multigrapher->y_axis.labelled_ticks++;
            }
          else
            /* print labels to right of right boundary */
            if (multigrapher->x_axis.switch_axis_end
                && !(multigrapher->grid_spec == AXES_AT_ORIGIN
                     /* don't plot label if it could run into an axis */
                     && NEAR_EQUALITY (yval,
                                       multigrapher->y_axis.other_axis_loc,
                                       multigrapher->y_trans.input_range)
                     && (multigrapher->x_axis.other_axis_loc
                         != multigrapher->x_trans.input_min)
                     && (multigrapher->x_axis.other_axis_loc
                         != multigrapher->x_trans.input_max)))
              {
                double new_width;

                pl_fmove_r (
                    multigrapher->plotter,
                    XN (multigrapher->x_axis.alt_other_axis_loc)
                        + (SS ((multigrapher->tick_size >= 0.0 ? 0.75 : 1.75)
                               * fabs (multigrapher->tick_size))
                           + multigrapher->half_line_width),
                    YV (yval));
                print_tick_label (
                    labelbuf, &multigrapher->y_axis, &multigrapher->y_trans,
                    (multigrapher->y_axis.type == A_LOG10) ? pow (10.0, yval)
                                                           : yval);
                new_width = pl_flabelwidth_r (multigrapher->plotter, labelbuf);
                pl_alabel_r (multigrapher->plotter, 'l', 'c', labelbuf);
                multigrapher->y_axis.max_label_width
                    = DMAX (multigrapher->y_axis.max_label_width, new_width);
                multigrapher->y_axis.labelled_ticks++;
              }

          /* Plot the tick marks on the y-axis, and horizontal grid lines. */
          switch (multigrapher->grid_spec)
            {
            case AXES_AND_BOX_AND_GRID:
              pl_linemod_r (multigrapher->plotter, "dotted");
              pl_fmove_r (multigrapher->plotter, XP (XSQ (0.0)), YV (yval));
              pl_fcont_r (multigrapher->plotter, XP (XSQ (1.0)), YV (yval));
              pl_linemod_r (multigrapher->plotter, "solid");
              /* fall through */
            case AXES_AND_BOX:
              if (!multigrapher->x_axis.switch_axis_end)
                {
                  pl_fmove_r (multigrapher->plotter,
                              XN (multigrapher->x_axis.alt_other_axis_loc),
                              YV (yval));
                  pl_fcont_r (multigrapher->plotter,
                              XN (multigrapher->x_axis.alt_other_axis_loc)
                                  - (SS (multigrapher->tick_size)
                                     + (multigrapher->tick_size > 0.0
                                            ? multigrapher->half_line_width
                                            : -multigrapher->half_line_width)),
                              YV (yval));
                }
              else
                {
                  pl_fmove_r (multigrapher->plotter,
                              XN (multigrapher->x_axis.other_axis_loc),
                              YV (yval));
                  pl_fcont_r (multigrapher->plotter,
                              XN (multigrapher->x_axis.other_axis_loc)
                                  + (SS (multigrapher->tick_size)
                                     + (multigrapher->tick_size > 0.0
                                            ? multigrapher->half_line_width
                                            : -multigrapher->half_line_width)),
                              YV (yval));
                }
              /* fall through */
            case AXES:
            case AXES_AT_ORIGIN:
              if (!multigrapher->x_axis.switch_axis_end)
                {
                  pl_fmove_r (multigrapher->plotter,
                              XN (multigrapher->x_axis.other_axis_loc),
                              YV (yval));
                  pl_fcont_r (multigrapher->plotter,
                              XN (multigrapher->x_axis.other_axis_loc)
                                  + (SS (multigrapher->tick_size)
                                     + (multigrapher->tick_size > 0.0
                                            ? multigrapher->half_line_width
                                            : -multigrapher->half_line_width)),
                              YV (yval));
                }
              else
                {
                  pl_fmove_r (multigrapher->plotter,
                              XN (multigrapher->x_axis.alt_other_axis_loc),
                              YV (yval));
                  pl_fcont_r (multigrapher->plotter,
                              XN (multigrapher->x_axis.alt_other_axis_loc)
                                  - (SS (multigrapher->tick_size)
                                     + (multigrapher->tick_size > 0.0
                                            ? multigrapher->half_line_width
                                            : -multigrapher->half_line_width)),
                              YV (yval));
                }
              break;
            default: /* shouldn't happen */
              break;
            }
        }

      if (multigrapher->y_axis.have_lin_subticks)
        {
          double subtick_size; /* libplot coordinates */

          /* linearly spaced subticks on a log axis are as long as regular
           * ticks */
          subtick_size = (multigrapher->y_axis.type == A_LOG10
                              ? SS (multigrapher->tick_size)
                              : SS (multigrapher->subtick_size));

          /* Plot the linearly spaced subtick marks on the ordinate */
          for (i = multigrapher->y_axis.min_lin_subtick_count;
               i <= multigrapher->y_axis.max_lin_subtick_count; i++)
            /* range can be empty */
            {
              yval = i * multigrapher->y_axis.lin_subtick_spacing;

              /* discard subtick locations outside plotting area */
              if (yval < multigrapher->y_trans.input_min - FUZZ * yrange
                  || yval > multigrapher->y_trans.input_max + FUZZ * yrange)
                continue;

              /* Plot the tick marks on the y-axis, and horizontal grid lines.
               */
              switch (multigrapher->grid_spec)
                {
                case AXES_AND_BOX_AND_GRID:
                case AXES_AND_BOX:
                  if (!multigrapher->x_axis.switch_axis_end)
                    {
                      pl_fmove_r (multigrapher->plotter,
                                  XN (multigrapher->x_axis.alt_other_axis_loc),
                                  YV (yval));
                      pl_fcont_r (
                          multigrapher->plotter,
                          XN (multigrapher->x_axis.alt_other_axis_loc)
                              - (subtick_size
                                 + (subtick_size > 0.0
                                        ? multigrapher->half_line_width
                                        : -multigrapher->half_line_width)),
                          YV (yval));
                    }
                  else
                    {
                      pl_fmove_r (multigrapher->plotter,
                                  XN (multigrapher->x_axis.other_axis_loc),
                                  YV (yval));
                      pl_fcont_r (
                          multigrapher->plotter,
                          XN (multigrapher->x_axis.other_axis_loc)
                              + (subtick_size
                                 + (subtick_size > 0.0
                                        ? multigrapher->half_line_width
                                        : -multigrapher->half_line_width)),
                          YV (yval));
                    }
                  /* fall through */
                case AXES:
                case AXES_AT_ORIGIN:
                  if (!multigrapher->x_axis.switch_axis_end)
                    {
                      pl_fmove_r (multigrapher->plotter,
                                  XN (multigrapher->x_axis.other_axis_loc),
                                  YV (yval));
                      pl_fcont_r (
                          multigrapher->plotter,
                          XN (multigrapher->x_axis.other_axis_loc)
                              + (subtick_size
                                 + (subtick_size > 0.0
                                        ? multigrapher->half_line_width
                                        : -multigrapher->half_line_width)),
                          YV (yval));
                    }
                  else
                    {
                      pl_fmove_r (multigrapher->plotter,
                                  XN (multigrapher->x_axis.alt_other_axis_loc),
                                  YV (yval));
                      pl_fcont_r (
                          multigrapher->plotter,
                          XN (multigrapher->x_axis.alt_other_axis_loc)
                              - (subtick_size
                                 + (subtick_size > 0.0
                                        ? multigrapher->half_line_width
                                        : -multigrapher->half_line_width)),
                          YV (yval));
                    }
                  break;
                default: /* shouldn't happen */
                  break;
                }
            }
        }

      /* plot a horizontal dotted line at y = 0 */
      if (multigrapher->grid_spec != AXES_AT_ORIGIN
          && multigrapher->y_axis.type == A_LINEAR
          && multigrapher->y_trans.input_min * multigrapher->y_trans.input_max
                 < 0.0)
        {
          pl_linemod_r (multigrapher->plotter, "dotted");
          pl_fline_r (multigrapher->plotter, XP (XSQ (0.0)), YV (0.0),
                      XP (XSQ (1.0)), YV (0.0));
          pl_linemod_r (multigrapher->plotter, "solid");
        }
    }

  /* 5.  DRAW LOGARITHMIC SUBSUBTICKS AND THEIR LABELS ON ABSCISSA */

  /* first, draw normal logarithmic subsubticks if any */
  if (multigrapher->grid_spec != NO_AXES
      && multigrapher->x_axis.have_normal_subsubticks
      && !multigrapher->x_axis.user_specified_subsubticks
      && !multigrapher->x_axis.omit_ticks)
    {
      int i, m, imin, imax;
      double xval, xrange = multigrapher->x_trans.input_max
                            - multigrapher->x_trans.input_min;

      /* compute an integer range (of powers of 10) large enough to include
         the entire desired axis */
      imin = (int)(floor (multigrapher->x_trans.input_min - FUZZ * xrange));
      imax = (int)(ceil (multigrapher->x_trans.input_max + FUZZ * xrange));

      for (i = imin; i < imax; i++)
        {
          for (m = 1; m <= 9; m++)
            {
              xval = i + log10 ((double)m);

              /* Plot subsubtick and label, if desired. */
              /* N.B. if tick is outside axis range, nothing will be printed */
              plot_abscissa_log_subsubtick (multigrapher, xval);
            }
        }
    }

  /* second, draw user-specified logarithmic subsubticks instead, if any */
  if (multigrapher->grid_spec != NO_AXES
      && multigrapher->x_axis.user_specified_subsubticks
      && !multigrapher->x_axis.omit_ticks)
    {
      int i, imin, imax;
      double xval, xrange = multigrapher->x_trans.input_max
                            - multigrapher->x_trans.input_min;

      /* compute an integer range large enough to include the entire
         desired axis */
      imin = (int)(floor (
          pow (10.0, multigrapher->x_trans.input_min - FUZZ * xrange)
          / multigrapher->x_axis.subsubtick_spacing));
      imax = (int)(ceil (
          pow (10.0, multigrapher->x_trans.input_max + FUZZ * xrange)
          / multigrapher->x_axis.subsubtick_spacing));

      /* draw user-specified subsubticks */
      for (i = imin; i <= imax; i++)
        {
          xval = log10 (i * multigrapher->x_axis.subsubtick_spacing);

          /* Plot subsubtick and label, if desired. */
          /* N.B. if tick is outside axis range, nothing will be printed */
          plot_abscissa_log_subsubtick (multigrapher, xval);
        }
    }

  /* 6.  DRAW LOGARITHMIC SUBSUBTICKS AND THEIR LABELS ON ORDINATE */

  /* first, draw normal logarithmic subsubticks if any */
  if (multigrapher->grid_spec != NO_AXES
      && multigrapher->y_axis.have_normal_subsubticks
      && !multigrapher->y_axis.user_specified_subsubticks
      && !multigrapher->y_axis.omit_ticks)
    {
      int i, m, imin, imax;
      double yval, yrange = multigrapher->y_trans.input_max
                            - multigrapher->y_trans.input_min;

      /* compute an integer range (of powers of 10) large enough to include
         the entire desired axis */
      imin = (int)(floor (multigrapher->y_trans.input_min - FUZZ * yrange));
      imax = (int)(ceil (multigrapher->y_trans.input_max + FUZZ * yrange));

      /* draw normal subticks */
      for (i = imin; i < imax; i++)
        {
          for (m = 1; m <= 9; m++)
            {
              yval = i + log10 ((double)m);

              /* Plot subsubtick and label, if desired. */
              /* N.B. if tick is outside axis range, nothing will be printed */
              plot_ordinate_log_subsubtick (multigrapher, yval);
            }
        }
    }

  /* second, draw user-specified logarithmic subsubticks instead, if any */
  if (multigrapher->grid_spec != NO_AXES
      && multigrapher->y_axis.user_specified_subsubticks
      && !multigrapher->y_axis.omit_ticks)
    {
      int i, imin, imax;
      double yval, yrange = multigrapher->y_trans.input_max
                            - multigrapher->y_trans.input_min;

      /* compute an integer range large enough to include the entire
         desired axis */
      imin = (int)(floor (
          pow (10.0, multigrapher->y_trans.input_min - FUZZ * yrange)
          / multigrapher->y_axis.subsubtick_spacing));
      imax = (int)(ceil (
          pow (10.0, multigrapher->y_trans.input_max + FUZZ * yrange)
          / multigrapher->y_axis.subsubtick_spacing));

      /* draw user-specified subsubticks */
      for (i = imin; i <= imax; i++)
        {
          yval = log10 (i * multigrapher->y_axis.subsubtick_spacing);

          /* Plot subsubtick and label, if desired. */
          /* N.B. if tick is outside axis range, nothing will be printed */
          plot_ordinate_log_subsubtick (multigrapher, yval);
        }
    }

  /* 7.  DRAW THE ABSCISSA LABEL */

  if ((multigrapher->grid_spec != NO_AXES)
      && multigrapher->x_axis.label != NULL
      && strlen (multigrapher->x_axis.label) == 0)
    {
      double x_axis_font_size;
      double xloc;

      /* switch to our font for drawing x axis label and tick labels */
      pl_fontname_r (multigrapher->plotter, multigrapher->x_axis.font_name);
      x_axis_font_size = pl_ffontsize_r (multigrapher->plotter,
                                         SS (multigrapher->x_axis.font_size));

      if (multigrapher->grid_spec != AXES_AT_ORIGIN)
        /* center the label on the axis */
        xloc = 0.5
               * (multigrapher->x_trans.input_max
                  + multigrapher->x_trans.input_min);
      else
        {
          if ((multigrapher->y_axis.other_axis_loc
               == multigrapher->y_trans.input_min)
              || (multigrapher->y_axis.other_axis_loc
                  == multigrapher->y_trans.input_max))

            xloc = 0.5
                   * (multigrapher->x_trans.input_max
                      + multigrapher->x_trans.input_min);
          else
            /* center label in the larger of the two halves */
            xloc = multigrapher->x_trans.input_max
                               - multigrapher->x_axis.other_axis_loc
                           >= multigrapher->x_axis.other_axis_loc
                                  - multigrapher->x_trans.input_min
                       ? 0.5
                             * (multigrapher->x_trans.input_max
                                + multigrapher->x_axis.other_axis_loc)
                       : 0.5
                             * (multigrapher->x_axis.other_axis_loc
                                + multigrapher->x_trans.input_min);
        }

      if (!multigrapher->y_axis
               .switch_axis_end) /* axis on bottom, label below it */
        {
          pl_fmove_r (
              multigrapher->plotter, XV (xloc),
              YN (multigrapher->y_axis.other_axis_loc)
                  - (SS ((multigrapher->tick_size >= 0.0 ? 0.875 : 2.125)
                         * fabs (multigrapher->tick_size))
                     + (6 * x_axis_font_size) / 5
                     + multigrapher->half_line_width));
          pl_alabel_r (multigrapher->plotter, 'c', 't',
                       multigrapher->x_axis.label);
        }
      else /* axis on top, label above it */
        {
          pl_fmove_r (
              multigrapher->plotter, XV (xloc),
              YN (multigrapher->y_axis.alt_other_axis_loc)
                  + (SS ((multigrapher->tick_size >= 0.0 ? 0.875 : 2.125)
                         * fabs (multigrapher->tick_size))
                     + (6 * x_axis_font_size) / 5
                     + multigrapher->half_line_width));
          pl_alabel_r (multigrapher->plotter, 'c', 'b',
                       multigrapher->x_axis.label);
        }
    }

  /* 8.  DRAW THE ORDINATE LABEL */

  if ((multigrapher->grid_spec != NO_AXES)
      && (multigrapher->y_axis.label != NULL
          && *(multigrapher->y_axis.label) != '\0'))
    {
      double y_axis_font_size;
      double yloc;

      /* switch to our font for drawing y axis label and tick labels */
      pl_fontname_r (multigrapher->plotter, multigrapher->y_axis.font_name);
      y_axis_font_size = pl_ffontsize_r (multigrapher->plotter,
                                         SS (multigrapher->y_axis.font_size));

      if (multigrapher->grid_spec != AXES_AT_ORIGIN)
        /* center the label on the axis */
        yloc = 0.5
               * (multigrapher->y_trans.input_min
                  + multigrapher->y_trans.input_max);
      else
        {
          if ((multigrapher->x_axis.other_axis_loc
               == multigrapher->x_trans.input_min)
              || (multigrapher->x_axis.other_axis_loc
                  == multigrapher->x_trans.input_max))
            yloc = 0.5
                   * (multigrapher->y_trans.input_min
                      + multigrapher->y_trans.input_max);
          else
            /* center label in the larger of the two halves */
            yloc = multigrapher->y_trans.input_max
                               - multigrapher->y_axis.other_axis_loc
                           >= multigrapher->y_axis.other_axis_loc
                                  - multigrapher->y_trans.input_min
                       ? 0.5
                             * (multigrapher->y_trans.input_max
                                + multigrapher->y_axis.other_axis_loc)
                       : 0.5
                             * (multigrapher->y_axis.other_axis_loc
                                + multigrapher->y_trans.input_min);
        }

/* a relic of temps perdus */
#define libplot_has_font_metrics 1

      if (!multigrapher->x_axis.switch_axis_end)
        {
          pl_fmove_r (
              multigrapher->plotter,
              XN (multigrapher->x_axis.other_axis_loc)
                  - (libplot_has_font_metrics
                         ? (SS ((multigrapher->tick_size >= 0.0 ? 0.75 : 1.75)
                                * fabs (multigrapher->tick_size))
                            + 1.15 * multigrapher->y_axis.max_label_width
                            + 0.5 * y_axis_font_size
                            + multigrapher->half_line_width)
                         : (SS ((multigrapher->tick_size >= 0.0 ? 0.75 : 1.75)
                                * fabs (multigrapher->tick_size)) /* backup */
                            + 1.0 * y_axis_font_size
                            + multigrapher->half_line_width)),
              YV (yloc));

          if (libplot_has_font_metrics
              && !multigrapher->no_rotate_y_label) /* can rotate label */
            {
              pl_textangle_r (multigrapher->plotter, 90);
              pl_alabel_r (multigrapher->plotter, 'c', 'x',
                           multigrapher->y_axis.label);
              pl_textangle_r (multigrapher->plotter, 0);
            }
          else
            /* non-rotated axis label, right justified */
            pl_alabel_r (multigrapher->plotter, 'r', 'c',
                         multigrapher->y_axis.label);
        }
      else
        {
          pl_fmove_r (
              multigrapher->plotter,
              XN (multigrapher->x_axis.alt_other_axis_loc)
                  + (libplot_has_font_metrics
                         ? (SS ((multigrapher->tick_size >= 0.0 ? 0.75 : 1.75)
                                * fabs (multigrapher->tick_size))
                            + 1.15 * multigrapher->y_axis.max_label_width
                            + 0.5 * y_axis_font_size
                            + multigrapher->half_line_width)
                         : (SS ((multigrapher->tick_size >= 0.0 ? 0.75 : 1.75)
                                * fabs (multigrapher->tick_size)) /* backup */
                            + 1.0 * y_axis_font_size
                            + multigrapher->half_line_width)),
              YV (yloc));

          if (libplot_has_font_metrics
              && !multigrapher->no_rotate_y_label) /* can rotate label */
            {
              pl_textangle_r (multigrapher->plotter, 90);
              pl_alabel_r (multigrapher->plotter, 'c', 't',
                           multigrapher->y_axis.label);
              pl_textangle_r (multigrapher->plotter, 0);
            }
          else
            /* non-rotated axis label, left justified */
            pl_alabel_r (multigrapher->plotter, 'l', 'c',
                         multigrapher->y_axis.label);
        }
    }

  /* END OF TASKS */

  /* flush frame to device */
  pl_flushpl_r (multigrapher->plotter);

  pl_restorestate_r (multigrapher->plotter);

  if (multigrapher->grid_spec != NO_AXES)
    {
      if (!tick_warning_printed
          && ((!multigrapher->x_axis.omit_ticks
               && multigrapher->x_axis.labelled_ticks <= 2)
              || (!multigrapher->y_axis.omit_ticks
                  && multigrapher->y_axis.labelled_ticks <= 2)))
        {
          fprintf (stderr,
                   "%s: the tick spacing is adjusted, as there were too few "
                   "labelled axis ticks\n",
                   progname);
          tick_warning_printed = true;
        }
    }
}

/* set_line_style() maps from line modes to physical line modes.  See
 * explanation at head of file. */

void
set_line_style (Multigrapher *multigrapher, int style, bool use_color)
{
  if (!use_color) /* monochrome */
    {
      if (style > 0)
        /* don't issue pl_linemod_r() if style<=0, since no polyline will
           be drawn */
        {
          int i;

          i = (style - 1) % NO_OF_LINEMODES;
          pl_linemod_r (multigrapher->plotter, linemodes[i]);
        }

      /* use same color as used for plot frame */
      pl_colorname_r (multigrapher->plotter, multigrapher->frame_color);
    }
  else /* color */
    {
      int i, j;

      if (style > 0) /* solid lines, various colors */
        {
          i = ((style - 1) / NO_OF_LINEMODES) % NO_OF_LINEMODES;
          j = (style - 1) % NO_OF_LINEMODES;
          pl_linemod_r (multigrapher->plotter, linemodes[i]);
        }

      else if (style == 0) /* use first color, as if -m 1 was spec'd */
                           /* (no line will be drawn) */
        j = 0;

      else /* neg. pl_linemode_r (no line will be drawn)*/
        j = (-style - 1) % (NO_OF_LINEMODES - 1);

      pl_colorname_r (multigrapher->plotter, colorstyle[j]);
    }
}

/* plot_point_array() calls plot_point() on each point in an array of
 * points.
 */

void
plot_point_array (Multigrapher *multigrapher, const Point *p, int length)
{
  int index;

  for (index = 0; index < length; index++)
    plot_point (multigrapher, &(p[index]));
}

/* plot_point() plots a single point, including the appropriate symbol and
 * errorbar(s) if any.  It may call either pl_fcont_r() or pl_fmove_r(),
 * depending on whether the pendown flag is set or not.  Gnuplot-style
 * clipping (clip mode = 0,1,2) is supported.
 *
 * plot_point() makes heavy use of the multigrapher->x_trans and
 * multigrapher->y_trans structures, which specify the linear
 * transformation from user coordinates to device coordinates.  It also
 * updates the multigrapher's internal state variables.  */
void
add_to_legend (Multigrapher *multigrapher, const Point *point)
{
  pl_fontname_r (multigrapher->plotter, multigrapher->title_font_name);
  double title_font_size = pl_ffontsize_r (
      multigrapher->plotter, SS (multigrapher->title_font_size * 0.5));
  pl_fmove_r (multigrapher->plotter, XP (XSQ (multigrapher->legend_pos)),
              YP (YSQ (0.79
                       + (((multigrapher->grid_spec == AXES_AND_BOX
                            || multigrapher->grid_spec == AXES)
                                   && (multigrapher->tick_size <= 0.0)
                               ? 1.0
                               : 0.5)
                          * fabs (multigrapher->tick_size))))
                  + 0.65 * title_font_size - multigrapher->graph_num * 150
                  + multigrapher->half_line_width);
  /* title centered, bottom spec'd */
  char symbol[2] = { 0 };
  strncpy (symbol, multigrapher->graph_symbols + multigrapher->graph_num, 1);
  pl_alabel_r (multigrapher->plotter, 'c', 'b', symbol);

  float yskip = multigrapher->graph_num * 0.06;
  pl_fmove_r (multigrapher->plotter, XP (XSQ (multigrapher->legend_pos - 0.1)),
              YP (YSQ (0.81 - yskip)));
  pl_fcont_r (multigrapher->plotter, XP (XSQ (multigrapher->legend_pos + 0.1)),
              YP (YSQ (0.81 - yskip)));
  pl_fmarker_r (multigrapher->plotter, XP (XSQ (0.3)), YP (YSQ (0.82 - yskip)),
                point->symbol, SS (point->symbol_size));

  multigrapher->graph_num++;
  /* TODO: */
}

void
plot_point (Multigrapher *multigrapher, const Point *point)
{
  double local_x0, local_y0, local_x1, local_y1;
  int clipval;

  /* If new polyline is beginning, take its line style, color/monochrome
     attribute, and line width and fill fraction attributes from the first
     point of the polyline.  We assume all such attribute fields are the
     same for all points in the polyline (our point reader arranges this
     for us). */
  if (!(point->pendown) || multigrapher->first_point_of_polyline)
    {
      int intfill;

      set_line_style (multigrapher, point->linemode, point->use_color);

      /* N.B. linewidth < 0.0 means use libplot default */
      pl_flinewidth_r (multigrapher->plotter,
                       point->line_width * (double)PLOT_SIZE);

      if (point->fill_fraction < 0.0)
        intfill = 0; /* transparent */
      else           /* guaranteed to be <= 1.0 */
        intfill = 1 + IROUND ((1.0 - point->fill_fraction) * 0xfffe);
      pl_filltype_r (multigrapher->plotter, intfill);

      if (multigrapher->legend_plot)
        {
          add_to_legend (multigrapher, point);
        }
    }

  /* determine endpoints of new line segment (for the first point of a
     polyline, use a zero-length line segment) */
  if (multigrapher->first_point_of_polyline)
    {
      local_x0 = point->x;
      local_y0 = point->y;
    }
  else
    {
      local_x0 = multigrapher->oldpoint_x;
      local_y0 = multigrapher->oldpoint_y;
    }
  local_x1 = point->x;
  local_y1 = point->y;

  /* save current point for use as endpoint of next line segment */
  multigrapher->oldpoint_x = point->x;
  multigrapher->oldpoint_y = point->y;

  /* apply Cohen-Sutherland clipper to new line segment */
  clipval
      = clip_line (multigrapher, &local_x0, &local_y0, &local_x1, &local_y1);

  if (!(clipval & ACCEPTED)) /* rejected in toto */
    {
      pl_fmove_r (multigrapher->plotter, XV (point->x),
                  YV (point->y)); /* move with pen up */
      multigrapher->first_point_of_polyline = false;
      return;
    }

  /* not rejected, ideally move with pen down */
  if (point->pendown && (point->linemode > 0))
    {
      switch (multigrapher->clip_mode) /* gnuplot style clipping (0,1, or 2) */
        {
        case 0:
          if ((clipval & CLIPPED_FIRST) || (clipval & CLIPPED_SECOND))
            /* clipped on at least one end, so move with pen up */
            pl_fmove_r (multigrapher->plotter, XV (point->x), YV (point->y));
          else
            /* line segment within box, so move with pen down */
            {
              if (!multigrapher->first_point_of_polyline)
                pl_fcont_r (multigrapher->plotter, XV (point->x),
                            YV (point->y));
              else
                pl_fmove_r (multigrapher->plotter, XV (point->x),
                            YV (point->y));
            }
          break;
        case 1:
        default:
          if ((clipval & CLIPPED_FIRST) && (clipval & CLIPPED_SECOND))
            /* both OOB, so move with pen up */
            pl_fmove_r (multigrapher->plotter, XV (point->x), YV (point->y));
          else
            /* at most one point is OOB */
            {
              if (clipval & CLIPPED_FIRST) /*current pt. OOB, new pt. not OOB*/
                {
                  if (!multigrapher->first_point_of_polyline)
                    {
                      /* move to clipped current point, draw line segment */
                      pl_fmove_r (multigrapher->plotter, XV (local_x0),
                                  YV (local_y0));
                      pl_fcont_r (multigrapher->plotter, XV (point->x),
                                  YV (point->y));
                    }
                  else
                    pl_fmove_r (multigrapher->plotter, XV (point->x),
                                YV (point->y));
                }
              else /* current point not OOB, new point OOB */
                {
                  if (!multigrapher->first_point_of_polyline)
                    {
                      /* draw line segment to clipped new point */
                      pl_fcont_r (multigrapher->plotter, XV (local_x1),
                                  YV (local_y1));
                      /* N.B. lib's notion of position now differs from ours */
                    }
                  else
                    pl_fmove_r (multigrapher->plotter, XV (point->x),
                                YV (point->y));
                }
            }
          break;
        case 2:
          if ((clipval & CLIPPED_FIRST)
              || multigrapher->first_point_of_polyline)
            /* move to clipped current point if necc. */
            pl_fmove_r (multigrapher->plotter, XV (local_x0), YV (local_y0));

          /* draw line segment to clipped new point */
          pl_fcont_r (multigrapher->plotter, XV (local_x1), YV (local_y1));

          if (clipval & CLIPPED_SECOND)
            /* new point OOB, so move to new point, breaking polyline */
            pl_fmove_r (multigrapher->plotter, XV (point->x), YV (point->y));
          break;
        }
    }
  else /* linemode=0 or pen up; so move with pen up */
    pl_fmove_r (multigrapher->plotter, XV (point->x), YV (point->y));

  multigrapher->first_point_of_polyline = false;

  /* if target point is OOB, return without plotting symbol or errorbar */
  if (clipval & CLIPPED_SECOND)
    return;

  /* plot symbol and errorbar, doing a pl_savestate_r()--pl_restorestate()
     to keep from breaking the polyline under construction (if any) */
  if (point->symbol >= 32) /* yow, a character */
    {
      /* will do a font change, so save & restore state */
      pl_savestate_r (multigrapher->plotter);
      plot_errorbar (multigrapher, point);
      pl_fontname_r (multigrapher->plotter, point->symbol_font_name);
      pl_fmarker_r (multigrapher->plotter, XV (point->x), YV (point->y),
                    point->symbol, SS (point->symbol_size));
      pl_restorestate_r (multigrapher->plotter);
    }

  else if (point->symbol > 0) /* a marker symbol */
    {
      if (point->linemode > 0)
        /* drawing a line, so (to keep from breaking it) save & restore state*/
        {
          pl_savestate_r (multigrapher->plotter);
          plot_errorbar (multigrapher, point); /* may or may not have one */
          pl_fmarker_r (multigrapher->plotter, XV (point->x), YV (point->y),
                        point->symbol, SS (point->symbol_size));
          pl_restorestate_r (multigrapher->plotter);
        }
      else
        /* not drawing a line, so just place the marker */
        {
          plot_errorbar (multigrapher, point);
          pl_fmarker_r (multigrapher->plotter, XV (point->x), YV (point->y),
                        point->symbol, SS (point->symbol_size));
        }
    }

  else if (point->symbol == 0 && point->linemode == 0)
    /* backward compatibility: -m 0 (even with -S 0) plots a dot */
    {
      plot_errorbar (multigrapher, point);
      pl_fmarker_r (multigrapher->plotter, XV (point->x), YV (point->y), M_DOT,
                    SS (point->symbol_size));
    }

  else /* no symbol, but may be an errorbar */
    plot_errorbar (multigrapher, point);

  return;
}

/* clip_line() takes two points, the endpoints of a line segment, and
 * destructively passes back two points: the endpoints of the line segment
 * clipped by Cohen-Sutherland to the rectangular plotting area.  Return
 * value contains bitfields ACCEPTED, CLIPPED_FIRST, and CLIPPED_SECOND.
 */

int
clip_line (Multigrapher *multigrapher, double *x0_p, double *y0_p,
           double *x1_p, double *y1_p)
{
  double x0 = *x0_p;
  double y0 = *y0_p;
  double x1 = *x1_p;
  double y1 = *y1_p;
  outcode outcode0 = compute_outcode (multigrapher, x0, y0, true);
  outcode outcode1 = compute_outcode (multigrapher, x1, y1, true);
  bool accepted;
  int clipval = 0;

  for (;;)
    {
      if (!(outcode0 | outcode1)) /* accept */
        {
          accepted = true;
          break;
        }
      else if (outcode0 & outcode1) /* reject */
        {
          accepted = false;
          break;
        }
      else
        {
          /* at least one endpoint is outside; choose one that is */
          outcode outcode_out = (outcode0 ? outcode0 : outcode1);
          double x, y; /* intersection with clip edge */

          if (outcode_out & RIGHT)
            {
              x = multigrapher->x_trans.input_max;
              y = y0
                  + (y1 - y0) * (multigrapher->x_trans.input_max - x0)
                        / (x1 - x0);
            }
          else if (outcode_out & LEFT)
            {
              x = multigrapher->x_trans.input_min;
              y = y0
                  + (y1 - y0) * (multigrapher->x_trans.input_min - x0)
                        / (x1 - x0);
            }
          else if (outcode_out & TOP)
            {
              x = x0
                  + (x1 - x0) * (multigrapher->y_trans.input_max - y0)
                        / (y1 - y0);
              y = multigrapher->y_trans.input_max;
            }
          else
            {
              x = x0
                  + (x1 - x0) * (multigrapher->y_trans.input_min - y0)
                        / (y1 - y0);
              y = multigrapher->y_trans.input_min;
            }

          if (outcode_out == outcode0)
            {
              x0 = x;
              y0 = y;
              outcode0 = compute_outcode (multigrapher, x0, y0, true);
            }
          else
            {
              x1 = x;
              y1 = y;
              outcode1 = compute_outcode (multigrapher, x1, y1, true);
            }
        }
    }

  if (accepted)
    {
      clipval |= ACCEPTED;
      if ((x0 != *x0_p) || (y0 != *y0_p))
        clipval |= CLIPPED_FIRST;
      if ((x1 != *x1_p) || (y1 != *y1_p))
        clipval |= CLIPPED_SECOND;
      *x0_p = x0;
      *y0_p = y0;
      *x1_p = x1;
      *y1_p = y1;
    }

  return clipval;
}

/* An alternative means of ending a polyline in progress.  Rather than
   ending it by passing plot_point() a point with the `pendown' flag not
   set, one may call this function.  This yields faster response in
   real-time work; e.g. in reader.c it is called by read_and_plot_file()
   after all dataset(s) have been read from the file and plotted. */

void
end_polyline_and_flush (Multigrapher *multigrapher)
{
  pl_endpath_r (multigrapher->plotter);
  pl_flushpl_r (multigrapher->plotter);
  multigrapher->first_point_of_polyline = true;
}

int
plot_graph_no_filter (ARG_LIST *arg_list)
{

  /* fill in any of min_? and max_? that user didn't specify (the
     prefix "final_" means these arguments were finalized at the
     time the first file of the plot was processed) */
  array_bounds (arg_list->p, arg_list->no_of_points,
                arg_list->final_transpose_axes, arg_list->clip_mode,
                &arg_list->final_min_x, &arg_list->final_min_y,
                &arg_list->final_max_x, &arg_list->final_max_y,
                arg_list->final_spec_min_x, arg_list->final_spec_min_y,
                arg_list->final_spec_max_x, arg_list->final_spec_max_y);

  if (arg_list->first_graph_of_multigraph)
    /* still haven't created multigrapher, do so now */
    {
      if ((arg_list->multigrapher = new_multigrapher (
               arg_list->output_format, arg_list->bg_color,
               arg_list->bitmap_size, arg_list->emulate_color,
               arg_list->max_line_length, arg_list->meta_portable,
               arg_list->page_size, arg_list->rotation_angle,
               arg_list->save_screen, arg_list->legend_plot))
          == NULL)
        {
          fprintf (stderr,
                   "%s: error: the graphing device could not be opened\n",
                   progname);
          return EXIT_FAILURE;
        }
    }

  /* begin graph: push new libplot drawing state onto stack of
     states; also concatenate the current transformation matrix
     with a matrix formed from the repositioning parameters (this
     will take effect for the duration of the graph) */
  begin_graph (arg_list->multigrapher, arg_list->reposition_scale,
               arg_list->reposition_trans_x, arg_list->reposition_trans_y);

  /* font selection, saves typing */
  if ((arg_list->title_font_name == NULL) && (arg_list->font_name != NULL))
    arg_list->title_font_name = arg_list->font_name;

  set_graph_parameters (
      arg_list->multigrapher, arg_list->frame_line_width,
      arg_list->frame_color, arg_list->top_label, arg_list->title_font_name,
      arg_list->title_font_size, /*for title*/
      arg_list->tick_size, arg_list->grid_spec, arg_list->final_min_x,
      arg_list->final_max_x, arg_list->final_spacing_x, arg_list->final_min_y,
      arg_list->final_max_y, arg_list->final_spacing_y,
      arg_list->final_spec_spacing_x, arg_list->final_spec_spacing_y,
      arg_list->plot_width, arg_list->plot_height, arg_list->margin_below,
      arg_list->margin_left, arg_list->font_name,
      arg_list->font_size, /* for abscissa label */
      arg_list->x_label, arg_list->font_name,
      arg_list->font_size, /* for ordinate label */
      arg_list->y_label, arg_list->no_rotate_y_label,
      /* these args are portmanteaux */
      arg_list->final_log_axis, arg_list->final_round_to_next_tick,
      arg_list->switch_axis_end, arg_list->omit_ticks,
      /* more args */
      arg_list->clip_mode, arg_list->blankout_fraction,
      arg_list->final_transpose_axes, arg_list->legend_position,
      arg_list->legend_plot);

  /* draw the graph frame (grid, ticks, etc.); draw a `canvas' (a
     background opaque white rectangle) only if this isn't the
     first graph */
  draw_frame_of_graph (arg_list->multigrapher,
                       arg_list->first_graph_of_multigraph ? false : true);

  /* plot the laboriously read-in array */
  plot_point_array (arg_list->multigrapher, arg_list->p,
                    arg_list->no_of_points);

  /* free points array */
  free (arg_list->p);
  arg_list->no_of_points = 0;
  return EXIT_SUCCESS;
}
