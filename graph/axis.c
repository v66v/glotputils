/* Copyright (C) 2024 by v66v <74927439+v66v@users.noreply.github.com> */
#include "axis.h"
#include "arg/arg_common.h"

void
print_tick_label (char *labelbuf, const Axis *axis, const Transform *transform,
                  double val)
{
  int prec;
  char *eloc, *ptr;
  char labelbuf_tmp[64], incrbuf[64];
  double spacing;
  bool big_exponents;
  double min, max;

  /* two possibilities: large/small exponent magnitudes */

  min = (axis->type == A_LOG10 ? pow (10.0, transform->input_min)
                               : transform->input_min);
  max = (axis->type == A_LOG10 ? pow (10.0, transform->input_max)
                               : transform->input_max);

  big_exponents = (((min != 0.0 && fabs (log10 (fabs (min))) >= 4.0)
                    || (max != 0.0 && fabs (log10 (fabs (max))) >= 4.0))
                       ? true
                       : false);

  if (big_exponents)
    /* large exponents, rewrite as foo x 10^bar, using escape sequences */
    {
      char *src = labelbuf_tmp, *dst = labelbuf;
      int exponent;
      char floatbuf[64];
      char *fptr = floatbuf;
      double prefactor;

      sprintf (labelbuf_tmp, "%e", val);
      if ((eloc = strchr (labelbuf_tmp, (int)'e')) == NULL)
        return;

      if (axis->type == A_LOG10 && !axis->user_specified_subsubticks)
        /* a hack: this must be a power of 10, so just print "10^bar" */
        {
          sscanf (++eloc, "%d", &exponent);
          sprintf (dst, "10\\sp%d\\ep", exponent);
          return;
        }

      /* special case: zero prints as `0', not 0.0x10^whatever */
      if (val == 0.0)
        {
          *dst++ = '0';
          *dst = '\0';
          return;
        }

      while (src < eloc)
        *fptr++ = *src++;
      *fptr = '\0';
      sscanf (floatbuf, "%lf", &prefactor); /* get foo */
      sscanf (++src, "%d", &exponent);      /* get bar */

      spacing
          = (axis->type == A_LINEAR
                 ? axis->tick_spacing
                 : axis->subsubtick_spacing); /* user-specified, for log axis*/
      sprintf (incrbuf, "%f", spacing / pow (10.0, (double)exponent));
      ptr = strchr (incrbuf, (int)'.');
      prec = 0;
      if (ptr != NULL)
        {
          int count = 0;

          while (*(++ptr))
            {
              count++;
              if (*ptr != '0')
                prec = count;
            }
        }

      /* \sp ... \ep is start_superscript ... end_superscript, and \r6 is
         right-shift by 1/6 em.  \mu is the `times' character. */
      sprintf (dst, "%.*f\\r6\\mu10\\sp%d\\ep", prec, prefactor, exponent);

      return;
    }

  else /* small-size exponent magnitudes */
    {
      if (axis->type == A_LOG10 && !axis->user_specified_subsubticks)
        /* a hack: this must be a (small) power of 10, so we'll just use
           %g format (same as %f, no trailing zeroes) */
        {
          sprintf (labelbuf, "%.9g", val);
          return;
        }

      /* always use no. of digits of precision present in increment */
      spacing
          = (axis->type == A_LINEAR
                 ? axis->tick_spacing
                 : axis->subsubtick_spacing); /* user-specified, for log axis*/
      sprintf (incrbuf, "%.9f", spacing);
      ptr = strchr (incrbuf, (int)'.');
      prec = 0;
      if (ptr != NULL)
        {
          int count = 0;

          while (*(++ptr))
            {
              count++;
              if (*ptr != '0')
                prec = count;
            }
        }
      sprintf (labelbuf, "%.*f", prec, val);
      return;
    }
}

/* Algorithm SCALE1, for selecting an inter-tick spacing that will yield a
 * good-looking linear-format axis.  The spacing is always 1.0, 2.0, or 5.0
 * times a power of ten.
 *
 * Reference: Lewart, C. R., "Algorithms SCALE1, SCALE2, and
 *	SCALE3 for Determination of Scales on Computer Generated
 *	Plots", Communications of the ACM, 10 (1973), 639-640.
 *      Also cited as ACM Algorithm 463.
 *
 * We call this routine even when the axis is logarithmic rather than
 * linear.  In that case the arguments are the logs of the actual
 * arguments, so that it computes an optimal inter-tick factor rather than
 * an optimal inter-tick spacing.
 */

/* ARGS: min,max = data min, max
         tick_spacing = inter-tick spacing
         tick_spacing_type = 0,1,2 i.e. S_ONE,S_TWO,S_FIVE */
void
scale1 (double min, double max, double *tick_spacing, int *tick_spacing_type)
{
  int k;
  double nal;
  double a, b;

  /* valid interval lengths */
  static const double vint[] = { 1.0, 2.0, 5.0, 10.0 };

  /* Corresponding breakpoints.  The published algorithm uses geometric
     means, i.e. sqrt(2), sqrt(10), sqrt(50), but using sqrt(10)=3.16...
     will (if nticks=5, as we choose it to be) cause intervals of length
     1.5 to yield an inter-tick distance of 0.2 rather than 0.5.  So we
     could reduce it to 2.95.  Similarly we could reduce sqrt(50) to 6.95
     so that intervals of length 3.5 will yield an inter-tick distance of
     1.0 rather than 0.5. */
  static const double sqr[] = { M_SQRT2, 3.16228, 7.07107 };

  /* compute trial inter-tick interval length */
  a = (max - min) / TRIAL_NUMBER_OF_TICK_INTERVALS;
  a *= (max > min) ? 1.0 : -1.0; /* paranoia, max>min always */
  if (a <= 0.0)
    {
      fprintf (stderr, "%s: error: the trial inter-tick spacing '%g' is bad\n",
               progname, a);
      exit (EXIT_FAILURE);
    }
  nal = floor (log10 (a));
  b = a * pow (10.0, -nal); /* 1.0 <= b < 10.0 */

  /* round to closest permissible inter-tick interval length */
  k = 0;
  do
    {
      if (b < sqr[k])
        break;
      k++;
    }
  while (k < 3);

  *tick_spacing = (max > min ? 1.0 : -1.0) * vint[k] * pow (10.0, nal);
  /* for increment type, 0,1,2 means 1,2,5 times a power of 10 */
  *tick_spacing_type = (k == 3 ? 0 : k);
  return;
}

/* Determine whether an inter-tick spacing (in practice, one specified by
   the user) is 1.0, 2.0, or 5.0 times a power of 10. */
int
spacing_type (double incr)
{
  int i;
  int i_tenpower = (int)(floor (log10 (incr)));
  double tenpower = 1.0;
  bool neg_power = false;

  if (i_tenpower < 0)
    {
      neg_power = true;
      i_tenpower = -i_tenpower;
    }

  for (i = 0; i < i_tenpower; i++)
    tenpower *= 10;
  if (neg_power)
    tenpower = 1.0 / tenpower;

  if (NEAR_EQUALITY (incr, tenpower, tenpower))
    return S_ONE;
  else if (NEAR_EQUALITY (incr, 2 * tenpower, tenpower))
    return S_TWO;
  else if (NEAR_EQUALITY (incr, 2.5 * tenpower, tenpower))
    return S_TWO_FIVE;
  else if (NEAR_EQUALITY (incr, 5 * tenpower, tenpower))
    return S_FIVE;
  else
    return S_UNKNOWN;
}

/* prepare_axis() fills in the Axis structure for an axis, and some of
 * the linear transformation variables in the Transform structure also.
 */

/* ARGS: user_specified_subticks = linear ticks on a log axis?
         round_to_next_tick = round limits to next tick mark?
         log_axis = log axis?
         reverse_axis = reverse min, max?
         switch_axis_end = intersection w/ other axis in alt. pos.?
         omit_ticks = suppress all ticks and tick labels? */
void
prepare_axis (Axis *axisp, Transform *trans, double min, double max,
              double spacing, const char *font_name, double font_size,
              const char *label, double subsubtick_spacing,
              bool user_specified_subsubticks, bool round_to_next_tick,
              bool log_axis, bool reverse_axis, bool switch_axis_end,
              bool omit_ticks)
{
  double range;
  int tick_spacing_type = 0;
  double tick_spacing, lin_subtick_spacing;
  int min_tick_count, max_tick_count;
  int min_lin_subtick_count, max_lin_subtick_count;
  bool have_lin_subticks;

  if (min > max)
    /* paranoia, max < min is swapped at top level */
    {
      fprintf (stderr,
               "%s: error: min > max for an axis, which is not allowed\n",
               progname);
      exit (EXIT_FAILURE);
    }

  if (min == max) /* expand in a clever way */
    {
      max = floor (max + 1.0);
      min = ceil (min - 1.0);
    }

  if (log_axis) /* log axis, data are stored in logarithmic form */
    /* compute a tick spacing; user can't specify it */
    {
      scale1 (min, max, &tick_spacing, &tick_spacing_type);
      if (tick_spacing <= 1.0)
        {
          tick_spacing = 1.0;
          tick_spacing_type = S_ONE;
        }
    }
  else /* linear axis */
    {
      if (spacing == 0.0) /* i.e., not specified by user */
        scale1 (min, max, &tick_spacing, &tick_spacing_type);
      else /* luser is boss, don't use SCALE1 */
        {
          tick_spacing = spacing;
          tick_spacing_type = spacing_type (spacing);
        }
    }

  range = max - min; /* range is not negative */

  if (round_to_next_tick) /* expand both limits to next tick */
    {
      if (user_specified_subsubticks)
        /* Special Case.  If user specified the `spacing' argument to -x or
           -y on a logarithmic axis, our usual tick-generating and
           tick-plotting algorithms are disabled.  So we don't bother with
           min_tick_count or several other fields of the axis struct;
           instead we just compute a new (rounded) max, min, and range.
           Since most data are stored as logs, this is complicated. */
        {
          double true_min = pow (10.0, min), true_max = pow (10.0, max);
          double true_range = true_max - true_min;
          int min_count, max_count;

          min_count = (int)(floor ((true_min + FUZZ * true_range)
                                   / subsubtick_spacing));
          max_count = (int)(ceil ((true_max - FUZZ * true_range)
                                  / subsubtick_spacing));
          /* avoid core dump, do *not* reduce minimum to zero! */
          if (min_count > 0)
            min = log10 (min_count * subsubtick_spacing);
          max = log10 (max_count * subsubtick_spacing);
          range = max - min;
          min_tick_count = max_tick_count = 0; /* keep gcc happy */
        }
      else /* normal `expand limits to next tick' case */
        {
          min_tick_count = (int)(floor ((min + FUZZ * range) / tick_spacing));
          max_tick_count = (int)(ceil ((max - FUZZ * range) / tick_spacing));
          /* max_tick_count > min_tick_count always */
          /* tickval = tick_spacing * count,
             for all count in [min_count,max_count]; must have >=2 ticks */
          min = tick_spacing * min_tick_count;
          max = tick_spacing * max_tick_count;
          range = max - min;
        }
    }
  else /* don't expand limits to next tick */
    {
      min_tick_count = (int)(ceil ((min - FUZZ * range) / tick_spacing));
      max_tick_count = (int)(floor ((max + FUZZ * range) / tick_spacing));
      /* max_tick_count <= min_tick_count is possible */
      /* tickval = incr * count,
         for all count in [min_count,max_count]; can have 0,1,2,3... ticks */
    }

  /* Allow 5 subticks per tick if S_FIVE or S_TWO_FIVE, 2 if S_TWO.  Case
     S_ONE is special; we try 10, 5, and 2 in succession */
  switch (tick_spacing_type)
    {
    case S_FIVE:
    case S_TWO_FIVE:
      lin_subtick_spacing = tick_spacing / 5;
      break;
    case S_TWO:
      lin_subtick_spacing = tick_spacing / 2;
      break;
    case S_ONE:
      lin_subtick_spacing = tick_spacing / 10;
      min_lin_subtick_count
          = (int)(ceil ((min - FUZZ * range) / lin_subtick_spacing));
      max_lin_subtick_count
          = (int)(floor ((max + FUZZ * range) / lin_subtick_spacing));
      if (max_lin_subtick_count - min_lin_subtick_count > MAX_NUM_SUBTICKS)
        {
          lin_subtick_spacing = tick_spacing / 5;
          min_lin_subtick_count
              = (int)(ceil ((min - FUZZ * range) / lin_subtick_spacing));
          max_lin_subtick_count
              = (int)(floor ((max + FUZZ * range) / lin_subtick_spacing));
          if (max_lin_subtick_count - min_lin_subtick_count > MAX_NUM_SUBTICKS)
            lin_subtick_spacing = tick_spacing / 2;
        }
      break;
    default:
      /* in default case, i.e. S_UNKNOWN, we won't plot linear subticks */
      lin_subtick_spacing
          = tick_spacing; /* not actually needed, since not plotted */
      break;
    }

  /* smallest possible inter-subtick factor for a log axis is 10.0 */
  if (log_axis && lin_subtick_spacing <= 1.0)
    lin_subtick_spacing = 1.0;

  min_lin_subtick_count
      = (int)(ceil ((min - FUZZ * range) / lin_subtick_spacing));
  max_lin_subtick_count
      = (int)(floor ((max + FUZZ * range) / lin_subtick_spacing));
  have_lin_subticks
      = ((tick_spacing_type != S_UNKNOWN /* S_UNKNOWN -> no subticks */
          && (max_lin_subtick_count - min_lin_subtick_count)
                 <= MAX_NUM_SUBTICKS)
             ? true
             : false);

  /* fill in parameters for axis-specific affine transformation */
  trans->input_min = min;
  trans->input_max = max;
  trans->input_range = range; /* precomputed for speed */
  trans->reverse = reverse_axis;

  /* fill in axis-specific plot frame variables */
  axisp->switch_axis_end = switch_axis_end;
  axisp->omit_ticks = omit_ticks;
  axisp->label = label;
  axisp->font_name = font_name;
  axisp->font_size = font_size;
  axisp->max_label_width = 0.0;
  axisp->type = log_axis ? A_LOG10 : A_LINEAR;
  axisp->tick_spacing = tick_spacing;
  axisp->min_tick_count = min_tick_count;
  axisp->max_tick_count = max_tick_count;
  axisp->have_lin_subticks = have_lin_subticks;
  axisp->lin_subtick_spacing = lin_subtick_spacing;
  axisp->min_lin_subtick_count = min_lin_subtick_count;
  axisp->max_lin_subtick_count = max_lin_subtick_count;
  axisp->user_specified_subsubticks = user_specified_subsubticks;
  axisp->subsubtick_spacing = subsubtick_spacing;
  axisp->labelled_ticks = 0; /* updated during drawing of frame */

  if (log_axis) /* logarithmic axis */
    /* do we have special logarithmic subsubticks, and should we label them? */
    {
      if (max - min <= MAX_DECADES_WITH_LOG_SUBSUBTICKS + FUZZ)
        /* not too many orders of magnitude, so plot normal log subsubticks */
        axisp->have_normal_subsubticks = true;
      else
        /* too many orders of magnitude, don't plot log subsubticks */
        axisp->have_normal_subsubticks = false;
    }
  else /* linear axes don't have log subsubticks */
    axisp->have_normal_subsubticks = false;
}

void
transpose_portmanteau (int *val)
{
  bool xtrue, ytrue;
  int newval;

  xtrue = ((*val & X_AXIS) ? true : false);
  ytrue = ((*val & Y_AXIS) ? true : false);

  newval = (xtrue ? Y_AXIS : 0) | (ytrue ? X_AXIS : 0);
  *val = newval;
}
