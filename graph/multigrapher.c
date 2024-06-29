#include "multigrapher.h"

void
plot_errorbar (Multigrapher *multigrapher, const Point *p)
{
  if (p->have_x_errorbar || p->have_y_errorbar)
    /* save & restore state, since we invoke pl_linemod_r() */
    {
      pl_savestate_r (multigrapher->plotter);
      pl_linemod_r (multigrapher->plotter, "solid");

      if (p->have_x_errorbar)
        {
          pl_fline_r (multigrapher->plotter, XV (p->xmin),
                      YV (p->y) - 0.5 * SS (p->symbol_size), XV (p->xmin),
                      YV (p->y) + 0.5 * SS (p->symbol_size));
          pl_fline_r (multigrapher->plotter, XV (p->xmin), YV (p->y),
                      XV (p->xmax), YV (p->y));
          pl_fline_r (multigrapher->plotter, XV (p->xmax),
                      YV (p->y) - 0.5 * SS (p->symbol_size), XV (p->xmax),
                      YV (p->y) + 0.5 * SS (p->symbol_size));
        }
      if (p->have_y_errorbar)
        {
          pl_fline_r (multigrapher->plotter,
                      XV (p->x) - 0.5 * SS (p->symbol_size), YV (p->ymin),
                      XV (p->x) + 0.5 * SS (p->symbol_size), YV (p->ymin));
          pl_fline_r (multigrapher->plotter, XV (p->x), YV (p->ymin),
                      XV (p->x), YV (p->ymax));
          pl_fline_r (multigrapher->plotter,
                      XV (p->x) - 0.5 * SS (p->symbol_size), YV (p->ymax),
                      XV (p->x) + 0.5 * SS (p->symbol_size), YV (p->ymax));
        }

      pl_restorestate_r (multigrapher->plotter);
    }
}

/* plot_abscissa_log_subsubtick() and plot_ordinate_log_subsubtick() are
   called to plot both normal log subticks and special (user-requested)
   ones */

/* ARGS: xval = log of location */
void
plot_abscissa_log_subsubtick (Multigrapher *multigrapher, double xval)
{
  double xrange
      = multigrapher->x_trans.input_max - multigrapher->x_trans.input_min;
  /* there is no way you could use longer labels on tick marks! */
  char labelbuf[2048];
  double tick_size = SS (multigrapher->tick_size); /* for positioning labels */
  double subsubtick_size = SS (multigrapher->subtick_size);

  /* switch to our font for drawing x axis label and tick labels */
  pl_fontname_r (multigrapher->plotter, multigrapher->x_axis.font_name);
  pl_ffontsize_r (multigrapher->plotter, SS (multigrapher->x_axis.font_size));

  /* discard subsubtick locations outside plotting area */
  if (xval < multigrapher->x_trans.input_min - FUZZ * xrange
      || xval > multigrapher->x_trans.input_max + FUZZ * xrange)
    return;

  /* label subsubtick if it seems appropriate */
  if (multigrapher->x_axis.user_specified_subsubticks)
    {
      print_tick_label (labelbuf, &multigrapher->x_axis,
                        &multigrapher->x_trans, pow (10.0, xval));
      if (!multigrapher->y_axis.switch_axis_end)
        {
          pl_fmove_r (
              multigrapher->plotter, XV (xval),
              YN (multigrapher->y_axis.other_axis_loc)
                  - ((tick_size >= 0 ? 0.75 : 1.75) * fabs ((double)tick_size)
                     + multigrapher->half_line_width));
          pl_alabel_r (multigrapher->plotter, 'c', 't', labelbuf);
          multigrapher->x_axis.labelled_ticks++;
        }
      else
        {
          pl_fmove_r (
              multigrapher->plotter, XV (xval),
              YN (multigrapher->y_axis.alt_other_axis_loc)
                  + ((tick_size >= 0 ? 0.75 : 1.75) * fabs ((double)tick_size)
                     + multigrapher->half_line_width));
          pl_alabel_r (multigrapher->plotter, 'c', 'b', labelbuf);
          multigrapher->x_axis.labelled_ticks++;
        }
    }

  /* draw subsubtick */
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
          pl_fcont_r (multigrapher->plotter, XV (xval),
                      YN (multigrapher->y_axis.alt_other_axis_loc)
                          - (subsubtick_size
                             + (subsubtick_size > 0.0
                                    ? multigrapher->half_line_width
                                    : -multigrapher->half_line_width)));
        }
      else
        {
          pl_fmove_r (multigrapher->plotter, XV (xval),
                      YN (multigrapher->y_axis.other_axis_loc));
          pl_fcont_r (multigrapher->plotter, XV (xval),
                      YN (multigrapher->y_axis.other_axis_loc)
                          + (subsubtick_size
                             + (subsubtick_size > 0.0
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
          pl_fcont_r (multigrapher->plotter, XV (xval),
                      YN (multigrapher->y_axis.other_axis_loc)
                          + (subsubtick_size
                             + (subsubtick_size > 0.0
                                    ? multigrapher->half_line_width
                                    : -multigrapher->half_line_width)));
        }
      else
        {
          pl_fmove_r (multigrapher->plotter, XV (xval),
                      YN (multigrapher->y_axis.alt_other_axis_loc));
          pl_fcont_r (multigrapher->plotter, XV (xval),
                      YN (multigrapher->y_axis.alt_other_axis_loc)
                          - (subsubtick_size
                             + (subsubtick_size > 0.0
                                    ? multigrapher->half_line_width
                                    : -multigrapher->half_line_width)));
        }
      break;
    default: /* shouldn't happen */
      break;
    }
}

/* ARGS: yval = log of location */
void
plot_ordinate_log_subsubtick (Multigrapher *multigrapher, double yval)
{
  double yrange
      = multigrapher->y_trans.input_max - multigrapher->y_trans.input_min;
  /* there is no way you could use longer labels on tick marks! */
  char labelbuf[2048];
  double tick_size = SS (multigrapher->tick_size); /* for positioning labels */
  double subsubtick_size = SS (multigrapher->subtick_size);

  /* switch to our font for drawing y axis label and tick labels */
  pl_fontname_r (multigrapher->plotter, multigrapher->y_axis.font_name);
  pl_ffontsize_r (multigrapher->plotter, SS (multigrapher->y_axis.font_size));

  /* discard subsubtick locations outside plotting area */
  if (yval < multigrapher->y_trans.input_min - FUZZ * yrange
      || yval > multigrapher->y_trans.input_max + FUZZ * yrange)
    return;

  /* label subsubtick if it seems appropriate */
  if (multigrapher->y_axis.user_specified_subsubticks)
    {
      double new_width;

      print_tick_label (labelbuf, &multigrapher->y_axis,
                        &multigrapher->y_trans, pow (10.0, yval));
      if (!multigrapher->x_axis.switch_axis_end)
        {
          pl_fmove_r (
              multigrapher->plotter,
              XN (multigrapher->x_axis.other_axis_loc)
                  - ((tick_size >= 0 ? 0.75 : 1.75) * fabs ((double)tick_size)
                     + multigrapher->half_line_width),
              YV (yval));
          new_width = pl_flabelwidth_r (multigrapher->plotter, labelbuf);
          pl_alabel_r (multigrapher->plotter, 'r', 'c', labelbuf);
          multigrapher->y_axis.max_label_width
              = DMAX (multigrapher->y_axis.max_label_width, new_width);
          multigrapher->y_axis.labelled_ticks++;
        }
      else
        {
          pl_fmove_r (
              multigrapher->plotter,
              XN (multigrapher->x_axis.alt_other_axis_loc)
                  + ((tick_size >= 0 ? 0.75 : 1.75) * fabs ((double)tick_size)
                     + multigrapher->half_line_width),
              YV (yval));
          new_width = pl_flabelwidth_r (multigrapher->plotter, labelbuf);
          pl_alabel_r (multigrapher->plotter, 'l', 'c', labelbuf);
          multigrapher->y_axis.max_label_width
              = DMAX (multigrapher->y_axis.max_label_width, new_width);
          multigrapher->y_axis.labelled_ticks++;
        }
    }

  /* draw subsubtick */
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
                      XN (multigrapher->x_axis.alt_other_axis_loc), YV (yval));
          pl_fcont_r (multigrapher->plotter,
                      XN (multigrapher->x_axis.alt_other_axis_loc)
                          - (subsubtick_size
                             + (subsubtick_size > 0.0
                                    ? multigrapher->half_line_width
                                    : -multigrapher->half_line_width)),
                      YV (yval));
        }
      else
        {
          pl_fmove_r (multigrapher->plotter,
                      XN (multigrapher->x_axis.other_axis_loc), YV (yval));
          pl_fcont_r (multigrapher->plotter,
                      XN (multigrapher->x_axis.other_axis_loc)
                          + (subsubtick_size
                             + (subsubtick_size > 0.0
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
                      XN (multigrapher->x_axis.other_axis_loc), YV (yval));
          pl_fcont_r (multigrapher->plotter,
                      XN (multigrapher->x_axis.other_axis_loc)
                          + (subsubtick_size
                             + (subsubtick_size > 0.0
                                    ? multigrapher->half_line_width
                                    : -multigrapher->half_line_width)),
                      YV (yval));
        }
      else
        {
          pl_fmove_r (multigrapher->plotter,
                      XN (multigrapher->x_axis.alt_other_axis_loc), YV (yval));
          pl_fcont_r (multigrapher->plotter,
                      XN (multigrapher->x_axis.alt_other_axis_loc)
                          - (subsubtick_size
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

/* Compute usual Cohen-Sutherland outcode, containing bitfields LEFT,
   RIGHT, BOTTOM, TOP.  Nine possibilities:
   {LEFT, interior, RIGHT} x {BOTTOM, interior, TOP}.
   The `tolerant' flag specifies how we handle points on the boundary. */
outcode
compute_outcode (Multigrapher *multigrapher, double x, double y, bool tolerant)
{
  outcode code = 0;
  double xfuzz = FUZZ * multigrapher->x_trans.input_range;
  double yfuzz = FUZZ * multigrapher->y_trans.input_range;
  int sign = (tolerant == true ? 1 : -1);

  if (x > multigrapher->x_trans.input_max + sign * xfuzz)
    code |= RIGHT;
  else if (x < multigrapher->x_trans.input_min - sign * xfuzz)
    code |= LEFT;
  if (y > multigrapher->y_trans.input_max + sign * yfuzz)
    code |= TOP;
  else if (y < multigrapher->y_trans.input_min - sign * yfuzz)
    code |= BOTTOM;

  return code;
}
