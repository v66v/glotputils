/* Copyright (C) 2024 by v66v <74927439+v66v@users.noreply.github.com> */
#include "arg_parse.h"

/* Parse an option flag, which may be a genuine option flag obtained
 from getopt, or a fake (a `1', indicating that a filename has
 been seen by getopt, or that filename has been seen on the command
 line after all genuine options have been processed, or that stdin
 must be read because no filenames have been seen). */

int
parse_flags (ARG_LIST *arg_list, int argc, char *argv[])
{
  switch (arg_list->option)
    {
      /* ----------- options with no argument --------------*/

    case 's': /* Don't erase display before plot, ARG NONE */
      arg_list->save_screen = true;
      break;
    case 't': /* Toggle transposition of axes, ARG NONE */
      arg_list->transpose_axes
          = (arg_list->transpose_axes == true ? false : true);
      break;
    case 'B': /* Toggle linemode auto-bumping, ARG NONE */
      arg_list->auto_bump = (arg_list->auto_bump == true ? false : true);
      break;
    case 'C': /* Toggle color/monochrome, ARG NONE */
      arg_list->new_use_color = true;
      arg_list->use_color = (arg_list->use_color == true ? false : true);
      break;
    case 'H': /* Toggle frame-on-top, ARG NONE */
      arg_list->frame_on_top = (arg_list->frame_on_top == true ? false : true);
      break;
    case 'O': /* portable format, ARG NONE */
      arg_list->meta_portable = "yes";
      break;
    case 'e' << 8: /* emulate color, ARG NONE */
      arg_list->emulate_color = xstrdup (optarg);
      break;
    case 'V' << 8: /* Version, ARG NONE		*/
      arg_list->show_version = true;
      arg_list->continue_parse = false;
      break;
    case 'h' << 8: /* Help, ARG NONE		*/
      arg_list->show_usage = true;
      arg_list->continue_parse = false;
      break;
    case 'f' << 8: /* Help on fonts, ARG NONE	*/
      arg_list->show_fonts = true;
      arg_list->continue_parse = false;
      break;
    case 'l' << 8: /* List fonts, ARG NONE		*/
      arg_list->do_list_fonts = true;
      arg_list->continue_parse = false;
      break;
    case 'Q': /* Toggle rotation of y-label, ARG NONE */
      arg_list->no_rotate_y_label
          = (arg_list->no_rotate_y_label == true ? false : true);
      break;

      /*----------- options with a single argument --------------*/

    case 'I': /* Input format, ARG REQUIRED	*/
      switch (*optarg)
        {
        case 'a':
        case 'A':
          /* ASCII format, records and fields within records are
                 separated by whitespace, and datasets are separated by a
                 pair of newlines.  Record length = 2. */
          arg_list->input_type = T_ASCII;
          break;
        case 'f':
        case 'F':
          /* Binary single precision, records and fields within records
                 are contiguous, and datasets are separated by a FLT_MAX.
                 Record length = 2. */
          arg_list->input_type = T_SINGLE;
          break;
        case 'd':
        case 'D':
          /* Binary double precision, records and fields within records
                 are contiguous, and datasets are separated by a DBL_MAX.
                 Record length = 2. */
          arg_list->input_type = T_DOUBLE;
          break;
        case 'i':
        case 'I':
          /* Binary integer, records and fields within records are
                 contiguous, and datasets are separated by an occurrence of
                 INT_MAX.  Record length = 2. */
          arg_list->input_type = T_INTEGER;
          break;
        case 'e':
        case 'E':
          /* Same as T_ASCII, but record length = 3. */
          arg_list->input_type = T_ASCII_ERRORBAR;
          break;
        case 'g':
        case 'G':
          /* Sui generis. */
          arg_list->input_type = T_GNUPLOT; /* gnuplot `table' format */
          break;
        default:
          fprintf (stderr, "%s: error: `%s' is an unrecognized data option\n",
                   progname, optarg);
          arg_list->errcnt++;
        }
      break;
    case 'f': /* Font size, ARG REQUIRED	*/
      if (sscanf (optarg, "%lf", &arg_list->local_font_size) <= 0)
        {
          fprintf (stderr,
                   "%s: error: the font size should be a number, but it "
                   "was `%s'\n",
                   progname, optarg);
          arg_list->errcnt++;
        }
      else
        {
          if (arg_list->local_font_size >= 1.0)
            fprintf (stderr,
                     "%s: the too-large font size `%f' is disregarded (it "
                     "should be less than 1.0)\n",
                     progname, arg_list->local_font_size);
          else if (arg_list->local_font_size < 0.0)
            fprintf (stderr,
                     "%s: the negative font size `%f' is disregarded\n",
                     progname, arg_list->local_font_size);
          else
            arg_list->font_size = arg_list->local_font_size;
        }
      break;
    case 'g': /* Grid style, ARG REQUIRED	*/
      if (sscanf (optarg, "%d", &arg_list->local_grid_style) <= 0)
        {
          fprintf (stderr,
                   "%s: error: the grid style should be a (small) "
                   "integer, but it was `%s'\n",
                   progname, optarg);
          arg_list->errcnt++;
          break;
        }
      switch (arg_list->local_grid_style)
        /* the subset ordering is: 0 < 1 < 2 < 3; 4 is different */
        {
        case 0:
          /* no frame at all; just the plot */
          arg_list->grid_spec = NO_AXES;
          break;
        case 1:
          /* box, ticks, gridlines, labels */
          arg_list->grid_spec = AXES;
          break;
        case 2:
          /* box, ticks, no gridlines, labels */
          arg_list->grid_spec = AXES_AND_BOX;
          break;
        case 3:
          /* `half-box', partial ticks, no gridlines, labels */
          arg_list->grid_spec = AXES_AND_BOX_AND_GRID;
          break;
        case 4:
          /* no box, no gridlines; specially positioned axes, labels */
          arg_list->grid_spec = AXES_AT_ORIGIN;
          break;
        default:
          fprintf (stderr,
                   "%s: error: the grid style number `%s' is out of bounds\n",
                   progname, optarg);
          arg_list->errcnt++;
        }
      break;
    case 'h': /* Height of plot, ARG REQUIRED	*/
      if (sscanf (optarg, "%lf", &arg_list->plot_height) <= 0)
        {
          fprintf (stderr,
                   "%s: error: the plot height should be a number, but it "
                   "was `%s'\n",
                   progname, optarg);
          arg_list->errcnt++;
        }
      break;
    case 'K': /* Clip mode, ARG REQUIRED */
      if ((sscanf (optarg, "%d", &arg_list->local_clip_mode) <= 0)
          || arg_list->local_clip_mode < 0 || arg_list->local_clip_mode > 2)
        fprintf (stderr,
                 "%s: the bad clip mode `%s' is disregarded (it should be "
                 "0, 1, or 2)\n",
                 progname, optarg);
      else
        arg_list->clip_mode = arg_list->local_clip_mode;
      break;
    case 'l': /* Toggle log/linear axis, ARG REQUIRED */
      switch (*optarg)
        {
        case 'x':
        case 'X':
          arg_list->log_axis ^= X_AXIS;
          break;
        case 'y':
        case 'Y':
          arg_list->log_axis ^= Y_AXIS;
          break;
        default:
          fprintf (stderr,
                   "%s: the unrecognized axis specification `%s' is "
                   "disregarded\n",
                   progname, optarg);
          break;
        }
      break;
    case 'N': /* Toggle omission of labels, ARG REQUIRED */
      switch (*optarg)
        {
        case 'x':
        case 'X':
          arg_list->omit_ticks ^= X_AXIS;
          break;
        case 'y':
        case 'Y':
          arg_list->omit_ticks ^= Y_AXIS;
          break;
        default:
          fprintf (stderr,
                   "%s: the unrecognized axis specification `%s' is "
                   "disregarded\n",
                   progname, optarg);
          break;
        }
      break;
    case 'm': /* Linemode, ARG REQUIRED	*/
      arg_list->new_linemode = true;
      if (sscanf (optarg, "%d", &arg_list->linemode_index) <= 0)
        {
          fprintf (stderr,
                   "%s: error: the linemode should be a (small) integer, "
                   "but it was `%s'\n",
                   progname, optarg);
          arg_list->errcnt++;
        }
      break;
    case 'q': /* Fill fraction, ARG REQUIRED	*/
      if (sscanf (optarg, "%lf", &arg_list->local_fill_fraction) <= 0)
        {
          fprintf (stderr,
                   "%s: error: the fill fraction should be a number, but "
                   "it was `%s'\n",
                   progname, optarg);
          arg_list->errcnt++;
        }
      else
        {
          if (arg_list->local_fill_fraction > 1.0)
            fprintf (stderr,
                     "%s: the region fill fraction `%f' was disregarded "
                     "(it should be less than or equal to 1.0)\n",
                     progname, arg_list->local_fill_fraction);
          else
            {
              arg_list->fill_fraction = arg_list->local_fill_fraction;
              arg_list->new_fill_fraction = true;
            }
        }
      break;
    case 'r': /* Right shift, ARG REQUIRED */
      if (sscanf (optarg, "%lf", &arg_list->margin_left) <= 0)
        {
          fprintf (stderr,
                   "%s: error: the rightward displacement for the plot "
                   "should be a number, but it was `%s'\n",
                   progname, optarg);
          arg_list->errcnt++;
        }
      break;
    case 'u': /* Upward shift, ARG REQUIRED */
      if (sscanf (optarg, "%lf", &arg_list->margin_below) <= 0)
        {
          fprintf (stderr,
                   "%s: error: the upward displacement for the plot "
                   "should be a number, but it was `%s'\n",
                   progname, optarg);
          arg_list->errcnt++;
        }
      break;
    case 'w': /* Width of plot, ARG REQUIRED 	*/
      if (sscanf (optarg, "%lf", &arg_list->plot_width) <= 0)
        {
          fprintf (stderr,
                   "%s: error: the plot width should be a number, but it "
                   "was `%s'\n",
                   progname, optarg);
          arg_list->errcnt++;
        }
      break;
    case 'T': /* Output format, ARG REQUIRED      */
    case 'T' << 8:
      arg_list->output_format = xstrdup (optarg);
      break;
    case 'F': /* Font name, ARG REQUIRED      */
      arg_list->font_name = xstrdup (optarg);
      break;
    case 'r' << 8: /* Rotation angle, ARG REQUIRED      */
      arg_list->rotation_angle = xstrdup (optarg);
      break;
    case 'Z' << 8: /* Title Font name, ARG REQUIRED      */
      arg_list->title_font_name = xstrdup (optarg);
      break;
    case 'G' << 8: /* Symbol Font name, ARG REQUIRED      */
      arg_list->symbol_font_name = xstrdup (optarg);
      arg_list->new_symbol_font_name = true;
      break;
    case 'R': /* Toggle rounding to next tick, ARG REQUIRED*/
      switch (*optarg)
        {
        case 'x':
        case 'X':
          arg_list->round_to_next_tick ^= X_AXIS;
          break;
        case 'y':
        case 'Y':
          arg_list->round_to_next_tick ^= Y_AXIS;
          break;
        default:
          fprintf (stderr,
                   "%s: the unrecognized axis specification `%s' is "
                   "disregarded\n",
                   progname, optarg);
          break;
        }
      break;
    case 'L': /* Top title, ARG REQUIRED	*/
      arg_list->top_label = xstrdup (optarg);
      break;
    case 'k': /* Tick size, ARG REQUIRED	*/
      if (sscanf (optarg, "%lf", &arg_list->tick_size) <= 0)
        {
          fprintf (stderr,
                   "%s: error: the tick size should be a number, but it "
                   "was `%s'\n",
                   progname, optarg);
          arg_list->errcnt++;
        }
      break;
    case 'W': /* Line width, ARG REQUIRED	*/
      if (sscanf (optarg, "%lf", &arg_list->local_plot_line_width) <= 0)
        {
          fprintf (stderr,
                   "%s: error: the line thickness for the plot should be "
                   "a number, but it was `%s'\n",
                   progname, optarg);
          arg_list->errcnt++;
        }
      if (arg_list->local_plot_line_width < 0.0)
        fprintf (stderr,
                 "%s: the negative plot line thickness `%f' is disregarded\n",
                 progname, arg_list->local_plot_line_width);
      else
        {
          arg_list->plot_line_width = arg_list->local_plot_line_width;
          arg_list->new_plot_line_width = true;
        }
      break;
    case 'X': /* X axis title, ARG REQUIRED	*/
      arg_list->x_label = xstrdup (optarg);
      break;
    case 'Y': /* Y axis title, ARG REQUIRED	*/
      arg_list->y_label = xstrdup (optarg);
      break;
    case 'E': /* Toggle switching of axis to other end, ARG REQUIRED */
      switch (*optarg)
        {
        case 'x':
        case 'X':
          arg_list->switch_axis_end ^= Y_AXIS;
          break;
        case 'y':
        case 'Y':
          arg_list->switch_axis_end ^= X_AXIS;
          break;
        default:
          fprintf (stderr,
                   "%s: the unrecognized axis specification `%s' is "
                   "disregarded\n",
                   progname, optarg);
          break;
        }
      break;
    case 'b' << 8: /* Blankout fraction, ARG REQUIRED */
      if (sscanf (optarg, "%lf", &arg_list->blankout_fraction) <= 0)
        {
          fprintf (stderr,
                   "%s: error: the fractional blankout should be a "
                   "number, but it was `%s'\n",
                   progname, optarg);
          arg_list->errcnt++;
        }
      break;
    case 'B' << 8: /* Bitmap size, ARG REQUIRED	*/
      arg_list->bitmap_size = xstrdup (optarg);
      break;
    case 'F' << 8: /* Title font size, ARG REQUIRED	*/
      if (sscanf (optarg, "%lf", &arg_list->local_title_font_size) <= 0)
        {
          fprintf (stderr,
                   "%s: error: the font size for the title should be a "
                   "number, but it was `%s'\n",
                   progname, optarg);
          arg_list->errcnt++;
        }
      else if (arg_list->local_title_font_size >= 1.0)
        fprintf (stderr,
                 "%s: the too-large title font size `%f' is disregarded "
                 "(it should be less than 1.0)\n",
                 progname, arg_list->local_title_font_size);
      else if (arg_list->local_title_font_size < 0.0)
        fprintf (stderr,
                 "%s: the negative title font size `%f' is disregarded\n",
                 progname, arg_list->local_title_font_size);
      if (arg_list->local_title_font_size == 0.0)
        fprintf (stderr,
                 "%s: the request for a zero title font size is disregarded\n",
                 progname);
      else
        arg_list->title_font_size = arg_list->local_title_font_size;
      break;
    case 'W' << 8: /* Frame line width, ARG REQUIRED	*/
      if (sscanf (optarg, "%lf", &arg_list->local_frame_line_width) <= 0)
        {
          fprintf (stderr,
                   "%s: error: the line thickness for the frame should be "
                   "a number, but it was `%s'\n",
                   progname, optarg);
          arg_list->errcnt++;
        }
      if (arg_list->local_frame_line_width < 0.0)
        fprintf (stderr,
                 "%s: the negative frame line thickness `%f' is disregarded\n",
                 progname, arg_list->local_frame_line_width);
      else
        arg_list->frame_line_width = arg_list->local_frame_line_width;
      break;
    case 'M' << 8: /* Max line length, ARG REQUIRED	*/
      arg_list->max_line_length = xstrdup (optarg);
      break;
    case 'P' << 8: /* Page size, ARG REQUIRED	*/
      arg_list->page_size = xstrdup (optarg);
      break;
    case 'p' << 8: /* Pen color string, ARG REQUIRED      */
      if (parse_pen_string (optarg) == false)
        {
          fprintf (stderr,
                   "%s: the unparseable pen string `%s' is disregarded\n",
                   progname, optarg);
        }
      break;
    case 'L' << 8:
      arg_list->legend_plot = true;
      arg_list->legend_position = optarg[0];
      break;
    case 'q' << 8: /* Background color, ARG REQUIRED      */
      arg_list->bg_color = xstrdup (optarg);
      break;
    case 'C' << 8: /* Frame color, ARG REQUIRED      */
      arg_list->frame_color = xstrdup (optarg);
      break;

      /*------ options with zero or more arguments ---------*/

    case 'a': /* Auto-abscissa, ARG OPTIONAL [0,1,2] */
      arg_list->auto_abscissa = true;
      if (optind >= argc)
        break;
      if (sscanf (argv[optind], "%lf", &arg_list->local_delta_x) <= 0)
        break;
      optind++; /* tell getopt we recognized delta_x */
      if (arg_list->local_delta_x == 0.0)
        /* "-a 0" turns off auto-abscissa for next file */
        {
          arg_list->auto_abscissa = false;
          break;
        }
      arg_list->delta_x = arg_list->local_delta_x;
      if (optind >= argc)
        break;
      if (sscanf (argv[optind], "%lf", &arg_list->local_x_start) <= 0)
        break;
      arg_list->x_start = arg_list->local_x_start;
      optind++; /* tell getopt we recognized x_start */
      break;
    case 'x': /* X limits, ARG OPTIONAL [0,1,2,3] */
      arg_list->matched = 0;
      if (optind >= argc
          || ((strcmp (argv[optind], "-") != 0)
              && (arg_list->matched
                  = sscanf (argv[optind], "%lf", &arg_list->local_min_x))
                     <= 0))
        {
          arg_list->spec_min_x = arg_list->spec_max_x
              = arg_list->spec_spacing_x = false;
          break;
        }
      if (arg_list->matched > 0)
        {
          arg_list->spec_min_x = true;
          arg_list->min_x = arg_list->local_min_x;
        }
      else
        {
          arg_list->spec_min_x = false;
        }
      optind++; /* tell getopt we recognized min_x */

      arg_list->matched = 0;
      if (optind >= argc
          || ((strcmp (argv[optind], "-") != 0)
              && (arg_list->matched
                  = sscanf (argv[optind], "%lf", &arg_list->local_max_x))
                     <= 0))
        {
          arg_list->spec_max_x = arg_list->spec_spacing_x = false;
          break;
        }
      if (arg_list->matched > 0)
        {
          arg_list->spec_max_x = true;
          arg_list->max_x = arg_list->local_max_x;
        }
      else
        {
          arg_list->spec_max_x = false;
        }
      optind++; /* tell getopt we recognized max_x */

      arg_list->matched = 0;
      if (optind >= argc
          || ((strcmp (argv[optind], "-") != 0)
              && (arg_list->matched
                  = sscanf (argv[optind], "%lf", &arg_list->local_spacing_x))
                     <= 0))
        {
          arg_list->spec_spacing_x = false;
          break;
        }
      if (arg_list->matched > 0)
        {
          arg_list->spec_spacing_x = true;
          arg_list->spacing_x = arg_list->local_spacing_x;
        }
      else
        arg_list->spec_spacing_x = false;
      optind++; /* tell getopt we recognized spacing_x */
      break;

    case 'y': /* Y limits, ARG OPTIONAL [0,1,2,3] */
      arg_list->matched = 0;
      if (optind >= argc
          || ((strcmp (argv[optind], "-") != 0)
              && (arg_list->matched
                  = sscanf (argv[optind], "%lf", &arg_list->local_min_y))
                     <= 0))
        {
          arg_list->spec_min_y = arg_list->spec_max_y
              = arg_list->spec_spacing_y = false;
          break;
        }
      if (arg_list->matched > 0)
        {
          arg_list->spec_min_y = true;
          arg_list->min_y = arg_list->local_min_y;
        }
      else
        arg_list->spec_min_y = false;
      optind++; /* tell getopt we recognized min_y */

      arg_list->matched = 0;
      if (optind >= argc
          || ((strcmp (argv[optind], "-") != 0)
              && (arg_list->matched
                  = sscanf (argv[optind], "%lf", &arg_list->local_max_y))
                     <= 0))
        {
          arg_list->spec_max_y = arg_list->spec_spacing_y = false;
          break;
        }
      if (arg_list->matched > 0)
        {
          arg_list->spec_max_y = true;
          arg_list->max_y = arg_list->local_max_y;
        }
      else
        arg_list->spec_max_y = false;
      optind++; /* tell getopt we recognized max_y */

      arg_list->matched = 0;
      if (optind >= argc
          || ((strcmp (argv[optind], "-") != 0)
              && (arg_list->matched
                  = sscanf (argv[optind], "%lf", &arg_list->local_spacing_y))
                     <= 0))
        {
          arg_list->spec_spacing_y = false;
          break;
        }
      if (arg_list->matched > 0)
        {
          arg_list->spec_spacing_y = true;
          arg_list->spacing_y = arg_list->local_spacing_y;
        }
      else
        arg_list->spec_spacing_y = false;
      optind++; /* tell getopt we recognized spacing_y */
      break;

    case 'S': /* Symbol, ARG OPTIONAL	[0,1,2]		*/
      arg_list->new_symbol = true;
      arg_list->symbol_index = 1; /* symbol # 1 is switched to by -S alone */
      if (optind >= argc)
        break;
      if (sscanf (argv[optind], "%d", &arg_list->local_symbol_index) <= 0)
        break;
      if (arg_list->local_symbol_index < 0
          || arg_list->local_symbol_index > 255)
        fprintf (stderr,
                 "%s: the symbol type `%d' is disregarded (it should be "
                 "in the range 0..255)\n",
                 progname, arg_list->local_symbol_index);
      else
        arg_list->symbol_index = arg_list->local_symbol_index;
      optind++; /* tell getopt we recognized symbol_index */
      if (optind >= argc)
        break;
      if (sscanf (argv[optind], "%lf", &arg_list->local_symbol_size) <= 0)
        break;
      if (arg_list->local_symbol_size < 0.0)
        fprintf (stderr, "%s: the negative symbol size `%f' is disregarded\n",
                 progname, arg_list->local_symbol_size);
      else if (arg_list->local_symbol_size == 0.0)
        fprintf (stderr,
                 "%s: the request for a zero symbol size is disregarded\n",
                 progname);
      else
        {
          arg_list->symbol_size = arg_list->local_symbol_size;
          arg_list->new_symbol_size = true;
        }
      optind++; /* tell getopt we recognized symbol_size */
      break;

      /* ---------- options with one or more arguments ---------- */

    case 'R' << 8: /* End graph and reposition, ARG REQUIRED [3]*/
      arg_list->old_reposition_trans_x = arg_list->reposition_trans_x;
      arg_list->old_reposition_trans_y = arg_list->reposition_trans_y;
      arg_list->old_reposition_scale = arg_list->reposition_scale;

      if (sscanf (optarg, "%lf", &arg_list->reposition_trans_x) <= 0)
        {
          fprintf (stderr,
                   "%s: error: the x repositioning should be a number, "
                   "but it was `%s'\n",
                   progname, optarg);
          return EXIT_FAILURE;
        }
      if (optind >= argc)
        {
          fprintf (stderr,
                   "%s: error: one or more arguments to the --reposition "
                   "option were missing\n",
                   progname);
          return EXIT_FAILURE;
        }
      if (sscanf (argv[optind], "%lf", &arg_list->reposition_trans_y) <= 0)
        {
          fprintf (stderr,
                   "%s: error: the y repositioning should be a number, "
                   "but it was `%s'\n",
                   progname, argv[optind]);
          return EXIT_FAILURE;
        }
      optind++; /* tell getopt we recognized trans_y */
      if (optind >= argc)
        {
          fprintf (stderr,
                   "%s: error: one or more arguments to the --reposition "
                   "option were missing\n",
                   progname);
          return EXIT_FAILURE;
        }
      if (sscanf (argv[optind], "%lf", &arg_list->reposition_scale) <= 0)
        {
          fprintf (stderr,
                   "%s: error: the reposition scale factor should be a "
                   "number, but it was `%s'\n",
                   progname, optarg);
          return EXIT_FAILURE;
        }
      if (arg_list->reposition_scale == 0.0)
        {
          fprintf (stderr,
                   "%s: error: the reposition scale factor should not be "
                   "zero\n",
                   progname);
          return EXIT_FAILURE;
        }
      optind++; /* tell getopt we recognized trans_x */

      if (!arg_list->first_file_of_graph)
        /* a graph is in progress (at least one file has been read), so
           it must be ended before we begin the next one */
        {
          if (!arg_list->filter)
            /* We haven't been acting as a real-time filter for the
               duration of this graph, so the graph isn't already drawn
               on the display.  Instead, we have a points array and we
               need to plot it, after computing bounds. */
            {
              if (plot_graph_no_filter (arg_list) == EXIT_FAILURE)
                return EXIT_FAILURE;
              arg_list->first_file_of_graph = false;
            }
          /* end of not-filter case */

          /* draw graph frame on top of graph, if user requested it */
          if (arg_list->frame_on_top)
            {
              end_polyline_and_flush (arg_list->multigrapher);
              draw_frame_of_graph (arg_list->multigrapher, false);
            }

          /* end graph: pop the graph-specific libplot drawing state off
 the stack of drawing states */
          end_graph (arg_list->multigrapher);

          /* on to next graph */
          arg_list->first_graph_of_multigraph = false;
          arg_list->first_file_of_graph = true;

        } /* end of not first-file-of-plot case */

      break; /* end of `--reposition' option */

    /* ---------------- pseudo-options -------------- */

    /* File specified on command line, returned in order (along with
   command-line options).  The first time we reach this point in
   any plot, we perform special initializations and in particular
   determine whether or not, for the duration of this plot, we'll
   be acting as a filter.  We can do so if xmin, xmax, ymin, ymax
   have all been specified, by this point, on the command line.

 A plot may consist of many files.  A plot in progress is
 terminated if a --reposition option (which moves us to the
 next plot of a multiplot) is seen, or when the last
 command-line option is processed. */
    case 1:
      if (arg_list->first_file_of_graph)
        {
          /* For plots with a logarithmic axis, compute logs of axis
                 limits, since coordinates along the axis, as obtained from
                 the reader, are stored in logarithmic form. */
          if (arg_list->log_axis & X_AXIS)
            {
              if (arg_list->spec_min_x)
                {
                  if (arg_list->min_x > 0.0)
                    arg_list->min_x = log10 (arg_list->min_x);
                  else
                    {
                      fprintf (stderr,
                               "%s: error: the limit %g on a logarithmic "
                               "axis is nonpositive\n",
                               progname, arg_list->min_x);
                      return EXIT_FAILURE;
                    }
                }
              if (arg_list->spec_max_x)
                {
                  if (arg_list->max_x > 0.0)
                    arg_list->max_x = log10 (arg_list->max_x);
                  else
                    {
                      fprintf (stderr,
                               "%s: error: the limit %g on a logarithmic "
                               "axis is nonpositive\n",
                               progname, arg_list->max_x);
                      return EXIT_FAILURE;
                    }
                }
            }

          if (arg_list->log_axis & Y_AXIS)
            {
              if (arg_list->spec_min_y)
                {
                  if (arg_list->min_y > 0.0)
                    arg_list->min_y = log10 (arg_list->min_y);
                  else
                    {
                      fprintf (stderr,
                               "%s: error: the limit %g on a logarithmic "
                               "axis is nonpositive\n",
                               progname, arg_list->min_y);
                      return EXIT_FAILURE;
                    }
                }
              if (arg_list->spec_max_y)
                {
                  if (arg_list->max_y > 0.0)
                    arg_list->max_y = log10 (arg_list->max_y);
                  else
                    {
                      fprintf (stderr,
                               "%s: error: the limit %g on a logarithmic "
                               "axis is nonpositive\n",
                               progname, arg_list->max_y);
                      return EXIT_FAILURE;
                    }
                }
            }

          /* We now finalize the following parameters (arguments to
                 set_graph_parameters()), even though we won't call
                 set_graph_parameters() for a while yet, if it turns out we
                 need to act as a real-time filter. */

          /* portmanteaux */
          arg_list->final_log_axis = arg_list->log_axis;
          arg_list->final_round_to_next_tick = arg_list->round_to_next_tick;

          /* bool */
          arg_list->final_transpose_axes = arg_list->transpose_axes;

          /* x-axis specific */
          arg_list->final_min_x = arg_list->min_x;
          arg_list->final_max_x = arg_list->max_x;
          arg_list->final_spacing_x = arg_list->spacing_x;
          arg_list->final_spec_min_x = arg_list->spec_min_x;
          arg_list->final_spec_max_x = arg_list->spec_max_x;
          arg_list->final_spec_spacing_x = arg_list->spec_spacing_x;

          /* y-axis specific */
          arg_list->final_min_y = arg_list->min_y;
          arg_list->final_max_y = arg_list->max_y;
          arg_list->final_spec_min_y = arg_list->spec_min_y;
          arg_list->final_spec_max_y = arg_list->spec_max_y;
          arg_list->final_spacing_y = arg_list->spacing_y;
          arg_list->final_spec_spacing_y = arg_list->spec_spacing_y;

          /* If user didn't specify either the lower limit or the upper
                 limit for an axis, by default we'll round the axis limits
                 to the nearest tick, after computing them.  (If either
                 limit was specified by the user, to request rounding the
                 user must specify the -R option as well.) */
          if (!arg_list->final_spec_min_x && !arg_list->final_spec_max_x)
            arg_list->final_round_to_next_tick |= X_AXIS;
          if (!arg_list->final_spec_min_y && !arg_list->final_spec_max_y)
            arg_list->final_round_to_next_tick |= Y_AXIS;

          /* The case when x_min, x_max, y_min, y_max are all specified
                 by the luser is special: we set the `filter' flag for the
                 duration of this plot, to indicate that we can function as
                 a real-time filter, calling read_and_plot_file() on each
                 file, rather than calling read_file() on each one
                 separately to create an array of points, and then calling
                 plot_point_array(). */
          arg_list->filter
              = ((arg_list->final_spec_min_x && arg_list->final_spec_max_x
                  && arg_list->final_spec_min_y && arg_list->final_spec_max_y)
                     ? true
                     : false);

        } /* end of first-file-of-plot case */

      if (arg_list->filter)
        /* filter flag is set, will call read_and_plot() on this file */
        {
          if (arg_list->first_file_of_graph)
            {
              if (arg_list->first_graph_of_multigraph)
                /* need to create the multigrapher */
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
                               "%s: error: the graphing device could not "
                               "be opened\n",
                               progname);
                      return EXIT_FAILURE;
                    }
                }

              /* begin graph: push a graph-specific drawing state onto
                     libplot's stack of drawing states; also concatenate
                     the current transformation matrix with a matrix formed
                     from the repositioning parameters (this will take
                     effect for the duration of the graph) */
              begin_graph (arg_list->multigrapher, arg_list->reposition_scale,
                           arg_list->reposition_trans_x,
                           arg_list->reposition_trans_y);

              /* font selection, saves typing */
              if ((arg_list->title_font_name == NULL)
                  && (arg_list->font_name != NULL))
                arg_list->title_font_name = arg_list->font_name;

              /* following will be in effect for the entire plot */
              set_graph_parameters (
                  arg_list->multigrapher, arg_list->frame_line_width,
                  arg_list->frame_color, arg_list->top_label,
                  arg_list->title_font_name,
                  arg_list->title_font_size, /* for title */
                  arg_list->tick_size, arg_list->grid_spec,
                  arg_list->final_min_x, arg_list->final_max_x,
                  arg_list->final_spacing_x, arg_list->final_min_y,
                  arg_list->final_max_y, arg_list->final_spacing_y,
                  arg_list->final_spec_spacing_x,
                  arg_list->final_spec_spacing_y, arg_list->plot_width,
                  arg_list->plot_height, arg_list->margin_below,
                  arg_list->margin_left, arg_list->font_name,
                  arg_list->font_size, /* on abscissa */
                  arg_list->x_label, arg_list->font_name,
                  arg_list->font_size, /* on ordinate */
                  arg_list->y_label, arg_list->no_rotate_y_label,
                  /* these args are portmanteaux */
                  arg_list->final_log_axis, arg_list->final_round_to_next_tick,
                  arg_list->switch_axis_end, arg_list->omit_ticks,
                  /* more args */
                  arg_list->clip_mode, arg_list->blankout_fraction,
                  arg_list->final_transpose_axes, arg_list->legend_position,
                  arg_list->legend_plot);

              /* draw the graph frame (grid, ticks, etc.); draw a
                     `canvas' (a background opaque white rectangle) only if
                     this isn't the first graph */
              draw_frame_of_graph (arg_list->multigrapher,
                                   arg_list->first_graph_of_multigraph ? false
                                                                       : true);

              arg_list->reader = new_reader (
                  arg_list->data_file, arg_list->input_type,
                  arg_list->auto_abscissa, arg_list->delta_x,
                  arg_list->x_start,
                  /* following three are graph-specific */
                  arg_list->final_transpose_axes, arg_list->final_log_axis,
                  arg_list->auto_bump,
                  /* following args are file-specific
                         (they set dataset attributes) */
                  arg_list->symbol_index, arg_list->symbol_size,
                  arg_list->symbol_font_name, arg_list->linemode_index,
                  arg_list->plot_line_width, arg_list->fill_fraction,
                  arg_list->use_color);
              arg_list->new_symbol = arg_list->new_symbol_size
                  = arg_list->new_symbol_font_name = false;
              arg_list->new_linemode = arg_list->new_plot_line_width = false;
              arg_list->new_fill_fraction = arg_list->new_use_color = false;
            }
          else
            /* not first file of plot; do some things anyway */
            {
              /* set reader parameters that may change when we move
                     from file to file within a plot */
              alter_reader_parameters (
                  arg_list->reader, arg_list->data_file, arg_list->input_type,
                  arg_list->auto_abscissa, arg_list->delta_x,
                  arg_list->x_start,
                  /* following args set dataset
                         attributes */
                  arg_list->symbol_index, arg_list->symbol_size,
                  arg_list->symbol_font_name, arg_list->linemode_index,
                  arg_list->plot_line_width, arg_list->fill_fraction,
                  arg_list->use_color,
                  /* following bools make up a mask*/
                  arg_list->new_symbol, arg_list->new_symbol_size,
                  arg_list->new_symbol_font_name, arg_list->new_linemode,
                  arg_list->new_plot_line_width, arg_list->new_fill_fraction,
                  arg_list->new_use_color);

              arg_list->new_symbol = arg_list->new_symbol_size
                  = arg_list->new_symbol_font_name = false;
              arg_list->new_linemode = arg_list->new_plot_line_width = false;
              arg_list->new_fill_fraction = arg_list->new_use_color = false;
            }

          /* call read_and_plot_file() on the file; each dataset in the
                 file yields a polyline */
          read_and_plot_file (arg_list->reader, arg_list->multigrapher);

        } /* end of filter case */

      else
        /* filter flag is set, will read and plot this file separately */

        /* Luser didn't specify enough information for us to act as a
           filter, so we do things the hard way: we call read_file() on
           each file to create a points array, and at the end of the
           plot we'll call plot_point_array() on the array.  For now,
           we don't even call set_graph_parameters(). */
        {
          if (arg_list->first_file_of_graph) /* some additional
                                               initializations */
            {
              arg_list->p = (Point *)xmalloc (arg_list->points_length
                                              * sizeof (Point));

              arg_list->reader = new_reader (
                  arg_list->data_file, arg_list->input_type,
                  arg_list->auto_abscissa, arg_list->delta_x,
                  arg_list->x_start,
                  /* following are graph-specific */
                  arg_list->final_transpose_axes, arg_list->final_log_axis,
                  arg_list->auto_bump,
                  /* following args are file-specific
                         (they set dataset attributes) */
                  arg_list->symbol_index, arg_list->symbol_size,
                  arg_list->symbol_font_name, arg_list->linemode_index,
                  arg_list->plot_line_width, arg_list->fill_fraction,
                  arg_list->use_color);
              arg_list->new_symbol = arg_list->new_symbol_size
                  = arg_list->new_symbol_font_name = false;
              arg_list->new_linemode = arg_list->new_plot_line_width = false;
              arg_list->new_fill_fraction = arg_list->new_use_color = false;
            }
          else /* not first file of plot, but do some things anyway */
            {
              /* set reader parameters that may change when we move
                     from file to file within a plot */
              alter_reader_parameters (
                  arg_list->reader, arg_list->data_file, arg_list->input_type,
                  arg_list->auto_abscissa, arg_list->delta_x,
                  arg_list->x_start,
                  /* following args set dataset
                         attributes */
                  arg_list->symbol_index, arg_list->symbol_size,
                  arg_list->symbol_font_name, arg_list->linemode_index,
                  arg_list->plot_line_width, arg_list->fill_fraction,
                  arg_list->use_color,
                  /* following bools make up a mask*/
                  arg_list->new_symbol, arg_list->new_symbol_size,
                  arg_list->new_symbol_font_name, arg_list->new_linemode,
                  arg_list->new_plot_line_width, arg_list->new_fill_fraction,
                  arg_list->new_use_color);

              arg_list->new_symbol = arg_list->new_symbol_size
                  = arg_list->new_symbol_font_name = false;
              arg_list->new_linemode = arg_list->new_plot_line_width = false;
              arg_list->new_fill_fraction = arg_list->new_use_color = false;
            }

          /* add points to points array by calling read_file() on file */
          read_file (arg_list->reader, &arg_list->p, &arg_list->points_length,
                     &arg_list->no_of_points);

        } /* end of not-filter case */

      /* close file */
      if (arg_list->data_file != stdin)
        close_file (optarg, arg_list->data_file);

      arg_list->first_file_of_graph = false;
      break; /* end of `case 1' in switch() [i.e., filename seen] */

      /*---------------- End of options ----------------*/

    default: /* Default, unknown option */
      arg_list->errcnt++;
      arg_list->continue_parse = false;
      break;
    }
  return EXIT_SUCCESS;
}

bool
parse_pen_string (const char *pen_s)
{
  const char *charp;
  char name[MAX_COLOR_NAME_LEN];
  int i;

  charp = pen_s;
  while (*charp)
    {
      int pen_num;
      bool got_digit;
      const char *tmp;

      if (*charp == ':') /* skip any ':' */
        {
          charp++;
          continue; /* back to top of while loop */
        }
      pen_num = 0;
      got_digit = false;
      while (*charp >= '0' && *charp <= '9')
        {
          pen_num = 10 * pen_num + (int)*charp - (int)'0';
          got_digit = true;
          charp++;
        }
      if (!got_digit || pen_num < 1 || pen_num > NO_OF_LINEMODES)
        return false;
      if (*charp != '=')
        return false;
      charp++;
      for (tmp = charp, i = 0; i < MAX_COLOR_NAME_LEN; tmp++, i++)
        {
          if (*tmp == ':') /* end of color name string */
            {
              name[i] = '\0';
              charp = tmp + 1;
              break;
            }
          else if (*tmp == '\0') /* end of name string */
            {
              name[i] = '\0';
              charp = tmp;
              break;
            }
          else
            name[i] = *tmp;
        }

      /* replace pen color name by user-specified color name */
      colorstyle[pen_num - 1] = xstrdup (name);
    }
  return true;
}
