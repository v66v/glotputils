#include "arg.h"
#include "file.h"
#include "libcommon.h"
#include "sys-defines.h"

const char *optstring = "-BCHOQVstE:F:f:g:h:k:K:I:l:L:m:N:q:R:r:T:u:w:W:X:Y:a:"
                        ":x::y::S::";

struct option long_options[] = {
  /* The most important option ("--display-type" is an obsolete variant) */
  { "output-format", ARG_REQUIRED, NULL, 'T' },
  { "display-type", ARG_REQUIRED, NULL, 'T' << 8 }, /* hidden */
  /* Other frequently used options */
  { "auto-abscissa", ARG_OPTIONAL, NULL, 'a' }, /* 0 or 1 or 2 */
  { "clip-mode", ARG_REQUIRED, NULL, 'K' },
  { "fill-fraction", ARG_REQUIRED, NULL, 'q' },
  { "font-name", ARG_REQUIRED, NULL, 'F' },
  { "font-size", ARG_REQUIRED, NULL, 'f' },
  { "grid-style", ARG_REQUIRED, NULL, 'g' },
  { "height-of-plot", ARG_REQUIRED, NULL, 'h' },
  { "input-format", ARG_REQUIRED, NULL, 'I' },
  { "line-mode", ARG_REQUIRED, NULL, 'm' },
  { "line-width", ARG_REQUIRED, NULL, 'W' },
  { "right-shift", ARG_REQUIRED, NULL, 'r' },
  { "save-screen", ARG_NONE, NULL, 's' },
  { "symbol", ARG_OPTIONAL, NULL, 'S' }, /* 0 or 1 or 2 */
  { "tick-size", ARG_REQUIRED, NULL, 'k' },
  { "toggle-auto-bump", ARG_NONE, NULL, 'B' },
  { "toggle-axis-end", ARG_REQUIRED, NULL, 'E' },
  { "toggle-frame-on-top", ARG_NONE, NULL, 'H' },
  { "toggle-log-axis", ARG_REQUIRED, NULL, 'l' },
  { "toggle-no-ticks", ARG_REQUIRED, NULL, 'N' },
  { "toggle-rotate-y-label", ARG_NONE, NULL, 'Q' },
  { "toggle-round-to-next-tick", ARG_REQUIRED, NULL, 'R' },
  { "toggle-transpose-axes", ARG_NONE, NULL, 't' },
  { "toggle-use-color", ARG_NONE, NULL, 'C' },
  { "top-label", ARG_REQUIRED, NULL, 'L' },
  { "upward-shift", ARG_REQUIRED, NULL, 'u' },
  { "width-of-plot", ARG_REQUIRED, NULL, 'w' },
  { "x-label", ARG_REQUIRED, NULL, 'X' },
  { "x-limits", ARG_OPTIONAL, NULL, 'x' }, /* 0, 1, 2, or 3 */
  { "y-label", ARG_REQUIRED, NULL, 'Y' },
  { "y-limits", ARG_OPTIONAL, NULL, 'y' }, /* 0, 1, 2, or 3 */
  /* Long options with no equivalent short option alias */
  { "bg-color", ARG_REQUIRED, NULL, 'q' << 8 },
  { "bitmap-size", ARG_REQUIRED, NULL, 'B' << 8 },
  { "blankout", ARG_REQUIRED, NULL, 'b' << 8 },
  { "emulate-color", ARG_REQUIRED, NULL, 'e' << 8 },
  { "frame-line-width", ARG_REQUIRED, NULL, 'W' << 8 },
  { "frame-color", ARG_REQUIRED, NULL, 'C' << 8 },
  { "max-line-length", ARG_REQUIRED, NULL, 'M' << 8 },
  { "pen-colors", ARG_REQUIRED, NULL, 'p' << 8 },
  { "reposition", ARG_REQUIRED, NULL, 'R' << 8 }, /* 3 */
  { "rotation", ARG_REQUIRED, NULL, 'r' << 8 },
  { "symbol-font-name", ARG_REQUIRED, NULL, 'G' << 8 },
  { "title-font-name", ARG_REQUIRED, NULL, 'Z' << 8 },
  { "title-font-size", ARG_REQUIRED, NULL, 'F' << 8 },
  { "page-size", ARG_REQUIRED, NULL, 'P' << 8 },
  /* Options relevant only to raw graph (refers to plot(5) output) */
  { "portable-output", ARG_NONE, NULL, 'O' },
  /* Documentation options */
  { "help-fonts", ARG_NONE, NULL, 'f' << 8 },
  { "list-fonts", ARG_NONE, NULL, 'l' << 8 },
  { "version", ARG_NONE, NULL, 'V' << 8 },
  { "help", ARG_NONE, NULL, 'h' << 8 },
  { NULL, 0, 0, 0 }
};

const int hidden_options[] = { (int)('T' << 8), 0 };
const char *progname = "graph"; /* name of this program */
const char *written = "Written by Robert S. Maier.";
const char *copyright = "Copyright (C) 2009 Free Software Foundation, Inc.";
const char *usage_appendage
    = " [FILE]...\n\nWith no FILE, or when FILE is -, read standard input.\n";

ARG_LIST arg_list = {
  .errcnt = 0,
  .using_getopt = true,
  .continue_parse = true,
  .show_version = false,
  .show_usage = false,
  .show_fonts = false,
  .do_list_fonts = false,
  .filter = false,
  .new_symbol = false,
  .new_symbol_size = false,
  .new_symbol_font_name = false,
  .new_linemode = false,
  .new_plot_line_width = false,
  .new_fill_fraction = false,
  .new_use_color = false,
  .first_file_of_graph = true,
  .first_graph_of_multigraph = true,
  .data_file = NULL,
  .reader = NULL,
  .input_type = T_ASCII,
  .auto_bump = true,
  .auto_abscissa = false,
  .x_start = 0,
  .delta_x = 1,
  .linemode_index = 1,
  .plot_line_width = -0,
  .symbol_index = 0,
  .symbol_size = .03,
  .fill_fraction = -1,
  .use_color = false,
  .transpose_axes = false,
  .multigrapher = NULL,
  .output_format = "meta",
  .bg_color = NULL,
  .bitmap_size = NULL,
  .emulate_color = NULL,
  .max_line_length = NULL,
  .meta_portable = NULL,
  .page_size = NULL,
  .rotation_angle = NULL,
  .save_screen = false,
  .grid_spec = AXES_AND_BOX,
  .no_rotate_y_label = false,
  .frame_color = "black",
  .clip_mode = 1,
  .log_axis = 0,
  .round_to_next_tick = 0,
  .switch_axis_end = 0,
  .omit_ticks = 0,
  .frame_line_width = -0,
  .margin_below = .2,
  .margin_left = .2,
  .plot_height = .6,
  .plot_width = .6,
  .tick_size = .02,
  .font_size = 0.0525,
  .title_font_size = 0.07,
  .blankout_fraction = 1.3,
  .font_name = NULL,
  .title_font_name = NULL,
  .symbol_font_name = "ZapfDingbats",
  .x_label = NULL,
  .y_label = NULL,
  .top_label = NULL,
  .min_x = 0.0,
  .min_y = 0.0,
  .max_x = 0.0,
  .max_y = 0.0,
  .spacing_x = 0.0,
  .spacing_y = 0.0,
  .spec_min_x = false,
  .spec_min_y = false,
  .spec_max_x = false,
  .spec_max_y = false,
  .spec_spacing_x = false,
  .spec_spacing_y = false,

  .final_log_axis = 0,
  .final_round_to_next_tick = 0,
  .final_min_x = 0.0,
  .final_max_x = 0.0,
  .final_spacing_x = 0.0,
  .final_min_y = 0.0,
  .final_max_y = 0.0,
  .final_spacing_y = 0.0,
  .final_spec_min_x = false,
  .final_spec_min_y = false,
  .final_spec_max_x = false,
  .final_spec_max_y = false,
  .final_spec_spacing_x = false,
  .final_spec_spacing_y = false,
  .final_transpose_axes = false,
  .p = NULL,
  .points_length = 1024,
  .no_of_points = 0,
  .reposition_trans_x = 0.0,
  .reposition_trans_y = 0.0,
  .reposition_scale = 1.0,
  .frame_on_top = false,

};

int
get_args (int argc, char *argv[])
{
  while (arg_list.continue_parse)
    {
      if (arg_list.using_getopt)
        /* end of options not reached yet */
        {
          arg_list.option
              = getopt_long (argc, argv,
                             /* initial hyphen requests no reordering */
                             optstring, long_options, &arg_list.opt_index);
          if (arg_list.option == EOF) /* end of options */
            {
              arg_list.using_getopt = false;
              continue; /* back to top of while loop */
            }
          if (arg_list.option == 1) /* filename embedded among options */
            {
              if (strcmp (optarg, "-") == 0)
                arg_list.data_file = stdin; /* interpret "-" as stdin */
              else
                open_file_for_reading (optarg, &arg_list.data_file);
            }
        }
      else
        /* end of options reached, processing filenames manually */
        {
          if (optind >= argc) /* all files processed */
            {
              if (arg_list.first_graph_of_multigraph
                  && arg_list.first_file_of_graph)
                /* no file appeared on command line, read stdin instead */
                {
                  arg_list.data_file = stdin;
                  arg_list.option = 1; /* code for pseudo-option */
                }
              else
                break; /* all files done, break out of while loop */
            }
          else /* have files yet to process */
            {
              if (strcmp (argv[optind], "-") == 0)
                arg_list.data_file = stdin;
              else
                open_file_for_reading (argv[optind], &arg_list.data_file);
              optarg = argv[optind]; /* keep track of name of opened file */
              optind++;
              arg_list.option = 1; /* code for pseudo-option */
            }
        }

      /* Parse an option flag, which may be a genuine option flag obtained
                 from getopt, or a fake (a `1', indicating that a filename has
         been seen by getopt, or that filename has been seen on the command
         line after all genuine options have been processed, or that stdin
                 must be read because no filenames have been seen). */

      switch (arg_list.option)
        {
          /* ----------- options with no argument --------------*/

        case 's': /* Don't erase display before plot, ARG NONE */
          arg_list.save_screen = true;
          break;
        case 't': /* Toggle transposition of axes, ARG NONE */
          arg_list.transpose_axes
              = (arg_list.transpose_axes == true ? false : true);
          break;
        case 'B': /* Toggle linemode auto-bumping, ARG NONE */
          arg_list.auto_bump = (arg_list.auto_bump == true ? false : true);
          break;
        case 'C': /* Toggle color/monochrome, ARG NONE */
          arg_list.new_use_color = true;
          arg_list.use_color = (arg_list.use_color == true ? false : true);
          break;
        case 'H': /* Toggle frame-on-top, ARG NONE */
          arg_list.frame_on_top
              = (arg_list.frame_on_top == true ? false : true);
          break;
        case 'O': /* portable format, ARG NONE */
          arg_list.meta_portable = "yes";
          break;
        case 'e' << 8: /* emulate color, ARG NONE */
          arg_list.emulate_color = xstrdup (optarg);
          break;
        case 'V' << 8: /* Version, ARG NONE		*/
          arg_list.show_version = true;
          arg_list.continue_parse = false;
          break;
        case 'h' << 8: /* Help, ARG NONE		*/
          arg_list.show_usage = true;
          arg_list.continue_parse = false;
          break;
        case 'f' << 8: /* Help on fonts, ARG NONE	*/
          arg_list.show_fonts = true;
          arg_list.continue_parse = false;
          break;
        case 'l' << 8: /* List fonts, ARG NONE		*/
          arg_list.do_list_fonts = true;
          arg_list.continue_parse = false;
          break;
        case 'Q': /* Toggle rotation of y-label, ARG NONE */
          arg_list.no_rotate_y_label
              = (arg_list.no_rotate_y_label == true ? false : true);
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
              arg_list.input_type = T_ASCII;
              break;
            case 'f':
            case 'F':
              /* Binary single precision, records and fields within records
                     are contiguous, and datasets are separated by a FLT_MAX.
                     Record length = 2. */
              arg_list.input_type = T_SINGLE;
              break;
            case 'd':
            case 'D':
              /* Binary double precision, records and fields within records
                     are contiguous, and datasets are separated by a DBL_MAX.
                     Record length = 2. */
              arg_list.input_type = T_DOUBLE;
              break;
            case 'i':
            case 'I':
              /* Binary integer, records and fields within records are
                     contiguous, and datasets are separated by an occurrence of
                     INT_MAX.  Record length = 2. */
              arg_list.input_type = T_INTEGER;
              break;
            case 'e':
            case 'E':
              /* Same as T_ASCII, but record length = 3. */
              arg_list.input_type = T_ASCII_ERRORBAR;
              break;
            case 'g':
            case 'G':
              /* Sui generis. */
              arg_list.input_type = T_GNUPLOT; /* gnuplot `table' format */
              break;
            default:
              fprintf (stderr,
                       "%s: error: `%s' is an unrecognized data option\n",
                       progname, optarg);
              arg_list.errcnt++;
            }
          break;
        case 'f': /* Font size, ARG REQUIRED	*/
          if (sscanf (optarg, "%lf", &arg_list.local_font_size) <= 0)
            {
              fprintf (stderr,
                       "%s: error: the font size should be a number, but it "
                       "was `%s'\n",
                       progname, optarg);
              arg_list.errcnt++;
            }
          else
            {
              if (arg_list.local_font_size >= 1.0)
                fprintf (stderr,
                         "%s: the too-large font size `%f' is disregarded (it "
                         "should be less than 1.0)\n",
                         progname, arg_list.local_font_size);
              else if (arg_list.local_font_size < 0.0)
                fprintf (stderr,
                         "%s: the negative font size `%f' is disregarded\n",
                         progname, arg_list.local_font_size);
              else
                arg_list.font_size = arg_list.local_font_size;
            }
          break;
        case 'g': /* Grid style, ARG REQUIRED	*/
          if (sscanf (optarg, "%d", &arg_list.local_grid_style) <= 0)
            {
              fprintf (stderr,
                       "%s: error: the grid style should be a (small) "
                       "integer, but it was `%s'\n",
                       progname, optarg);
              arg_list.errcnt++;
              break;
            }
          switch (arg_list.local_grid_style)
            /* the subset ordering is: 0 < 1 < 2 < 3; 4 is different */
            {
            case 0:
              /* no frame at all; just the plot */
              arg_list.grid_spec = NO_AXES;
              break;
            case 1:
              /* box, ticks, gridlines, labels */
              arg_list.grid_spec = AXES;
              break;
            case 2:
              /* box, ticks, no gridlines, labels */
              arg_list.grid_spec = AXES_AND_BOX;
              break;
            case 3:
              /* `half-box', partial ticks, no gridlines, labels */
              arg_list.grid_spec = AXES_AND_BOX_AND_GRID;
              break;
            case 4:
              /* no box, no gridlines; specially positioned axes, labels */
              arg_list.grid_spec = AXES_AT_ORIGIN;
              break;
            default:
              fprintf (
                  stderr,
                  "%s: error: the grid style number `%s' is out of bounds\n",
                  progname, optarg);
              arg_list.errcnt++;
            }
          break;
        case 'h': /* Height of plot, ARG REQUIRED	*/
          if (sscanf (optarg, "%lf", &arg_list.plot_height) <= 0)
            {
              fprintf (stderr,
                       "%s: error: the plot height should be a number, but it "
                       "was `%s'\n",
                       progname, optarg);
              arg_list.errcnt++;
            }
          break;
        case 'K': /* Clip mode, ARG REQUIRED */
          if ((sscanf (optarg, "%d", &arg_list.local_clip_mode) <= 0)
              || arg_list.local_clip_mode < 0 || arg_list.local_clip_mode > 2)
            fprintf (stderr,
                     "%s: the bad clip mode `%s' is disregarded (it should be "
                     "0, 1, or 2)\n",
                     progname, optarg);
          else
            arg_list.clip_mode = arg_list.local_clip_mode;
          break;
        case 'l': /* Toggle log/linear axis, ARG REQUIRED */
          switch (*optarg)
            {
            case 'x':
            case 'X':
              arg_list.log_axis ^= X_AXIS;
              break;
            case 'y':
            case 'Y':
              arg_list.log_axis ^= Y_AXIS;
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
              arg_list.omit_ticks ^= X_AXIS;
              break;
            case 'y':
            case 'Y':
              arg_list.omit_ticks ^= Y_AXIS;
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
          arg_list.new_linemode = true;
          if (sscanf (optarg, "%d", &arg_list.linemode_index) <= 0)
            {
              fprintf (stderr,
                       "%s: error: the linemode should be a (small) integer, "
                       "but it was `%s'\n",
                       progname, optarg);
              arg_list.errcnt++;
            }
          break;
        case 'q': /* Fill fraction, ARG REQUIRED	*/
          if (sscanf (optarg, "%lf", &arg_list.local_fill_fraction) <= 0)
            {
              fprintf (stderr,
                       "%s: error: the fill fraction should be a number, but "
                       "it was `%s'\n",
                       progname, optarg);
              arg_list.errcnt++;
            }
          else
            {
              if (arg_list.local_fill_fraction > 1.0)
                fprintf (stderr,
                         "%s: the region fill fraction `%f' was disregarded "
                         "(it should be less than or equal to 1.0)\n",
                         progname, arg_list.local_fill_fraction);
              else
                {
                  arg_list.fill_fraction = arg_list.local_fill_fraction;
                  arg_list.new_fill_fraction = true;
                }
            }
          break;
        case 'r': /* Right shift, ARG REQUIRED */
          if (sscanf (optarg, "%lf", &arg_list.margin_left) <= 0)
            {
              fprintf (stderr,
                       "%s: error: the rightward displacement for the plot "
                       "should be a number, but it was `%s'\n",
                       progname, optarg);
              arg_list.errcnt++;
            }
          break;
        case 'u': /* Upward shift, ARG REQUIRED */
          if (sscanf (optarg, "%lf", &arg_list.margin_below) <= 0)
            {
              fprintf (stderr,
                       "%s: error: the upward displacement for the plot "
                       "should be a number, but it was `%s'\n",
                       progname, optarg);
              arg_list.errcnt++;
            }
          break;
        case 'w': /* Width of plot, ARG REQUIRED 	*/
          if (sscanf (optarg, "%lf", &arg_list.plot_width) <= 0)
            {
              fprintf (stderr,
                       "%s: error: the plot width should be a number, but it "
                       "was `%s'\n",
                       progname, optarg);
              arg_list.errcnt++;
            }
          break;
        case 'T': /* Output format, ARG REQUIRED      */
        case 'T' << 8:
          arg_list.output_format = xstrdup (optarg);
          break;
        case 'F': /* Font name, ARG REQUIRED      */
          arg_list.font_name = xstrdup (optarg);
          break;
        case 'r' << 8: /* Rotation angle, ARG REQUIRED      */
          arg_list.rotation_angle = xstrdup (optarg);
          break;
        case 'Z' << 8: /* Title Font name, ARG REQUIRED      */
          arg_list.title_font_name = xstrdup (optarg);
          break;
        case 'G' << 8: /* Symbol Font name, ARG REQUIRED      */
          arg_list.symbol_font_name = xstrdup (optarg);
          arg_list.new_symbol_font_name = true;
          break;
        case 'R': /* Toggle rounding to next tick, ARG REQUIRED*/
          switch (*optarg)
            {
            case 'x':
            case 'X':
              arg_list.round_to_next_tick ^= X_AXIS;
              break;
            case 'y':
            case 'Y':
              arg_list.round_to_next_tick ^= Y_AXIS;
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
          arg_list.top_label = xstrdup (optarg);
          break;
        case 'k': /* Tick size, ARG REQUIRED	*/
          if (sscanf (optarg, "%lf", &arg_list.tick_size) <= 0)
            {
              fprintf (stderr,
                       "%s: error: the tick size should be a number, but it "
                       "was `%s'\n",
                       progname, optarg);
              arg_list.errcnt++;
            }
          break;
        case 'W': /* Line width, ARG REQUIRED	*/
          if (sscanf (optarg, "%lf", &arg_list.local_plot_line_width) <= 0)
            {
              fprintf (stderr,
                       "%s: error: the line thickness for the plot should be "
                       "a number, but it was `%s'\n",
                       progname, optarg);
              arg_list.errcnt++;
            }
          if (arg_list.local_plot_line_width < 0.0)
            fprintf (
                stderr,
                "%s: the negative plot line thickness `%f' is disregarded\n",
                progname, arg_list.local_plot_line_width);
          else
            {
              arg_list.plot_line_width = arg_list.local_plot_line_width;
              arg_list.new_plot_line_width = true;
            }
          break;
        case 'X': /* X axis title, ARG REQUIRED	*/
          arg_list.x_label = xstrdup (optarg);
          break;
        case 'Y': /* Y axis title, ARG REQUIRED	*/
          arg_list.y_label = xstrdup (optarg);
          break;
        case 'E': /* Toggle switching of axis to other end,
ARG REQUIRED */
          switch (*optarg)
            {
            case 'x':
            case 'X':
              arg_list.switch_axis_end ^= Y_AXIS;
              break;
            case 'y':
            case 'Y':
              arg_list.switch_axis_end ^= X_AXIS;
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
          if (sscanf (optarg, "%lf", &arg_list.blankout_fraction) <= 0)
            {
              fprintf (stderr,
                       "%s: error: the fractional blankout should be a "
                       "number, but it was `%s'\n",
                       progname, optarg);
              arg_list.errcnt++;
            }
          break;
        case 'B' << 8: /* Bitmap size, ARG REQUIRED	*/
          arg_list.bitmap_size = xstrdup (optarg);
          break;
        case 'F' << 8: /* Title font size, ARG REQUIRED	*/
          if (sscanf (optarg, "%lf", &arg_list.local_title_font_size) <= 0)
            {
              fprintf (stderr,
                       "%s: error: the font size for the title should be a "
                       "number, but it was `%s'\n",
                       progname, optarg);
              arg_list.errcnt++;
            }
          else if (arg_list.local_title_font_size >= 1.0)
            fprintf (stderr,
                     "%s: the too-large title font size `%f' is disregarded "
                     "(it should be less than 1.0)\n",
                     progname, arg_list.local_title_font_size);
          else if (arg_list.local_title_font_size < 0.0)
            fprintf (stderr,
                     "%s: the negative title font size `%f' is disregarded\n",
                     progname, arg_list.local_title_font_size);
          if (arg_list.local_title_font_size == 0.0)
            fprintf (
                stderr,
                "%s: the request for a zero title font size is disregarded\n",
                progname);
          else
            arg_list.title_font_size = arg_list.local_title_font_size;
          break;
        case 'W' << 8: /* Frame line width, ARG REQUIRED	*/
          if (sscanf (optarg, "%lf", &arg_list.local_frame_line_width) <= 0)
            {
              fprintf (stderr,
                       "%s: error: the line thickness for the frame should be "
                       "a number, but it was `%s'\n",
                       progname, optarg);
              arg_list.errcnt++;
            }
          if (arg_list.local_frame_line_width < 0.0)
            fprintf (
                stderr,
                "%s: the negative frame line thickness `%f' is disregarded\n",
                progname, arg_list.local_frame_line_width);
          else
            arg_list.frame_line_width = arg_list.local_frame_line_width;
          break;
        case 'M' << 8: /* Max line length, ARG REQUIRED	*/
          arg_list.max_line_length = xstrdup (optarg);
          break;
        case 'P' << 8: /* Page size, ARG REQUIRED	*/
          arg_list.page_size = xstrdup (optarg);
          break;
        case 'p' << 8: /* Pen color string, ARG REQUIRED      */
          if (parse_pen_string (optarg) == false)
            {
              fprintf (stderr,
                       "%s: the unparseable pen string `%s' is disregarded\n",
                       progname, optarg);
            }
          break;
        case 'q' << 8: /* Background color, ARG REQUIRED      */
          arg_list.bg_color = xstrdup (optarg);
          break;
        case 'C' << 8: /* Frame color, ARG REQUIRED      */
          arg_list.frame_color = xstrdup (optarg);
          break;

          /*------ options with zero or more arguments ---------*/

        case 'a': /* Auto-abscissa, ARG OPTIONAL [0,1,2] */
          arg_list.auto_abscissa = true;
          if (optind >= argc)
            break;
          if (sscanf (argv[optind], "%lf", &arg_list.local_delta_x) <= 0)
            break;
          optind++; /* tell getopt we recognized delta_x */
          if (arg_list.local_delta_x == 0.0)
            /* "-a 0" turns off auto-abscissa for next file */
            {
              arg_list.auto_abscissa = false;
              break;
            }
          arg_list.delta_x = arg_list.local_delta_x;
          if (optind >= argc)
            break;
          if (sscanf (argv[optind], "%lf", &arg_list.local_x_start) <= 0)
            break;
          arg_list.x_start = arg_list.local_x_start;
          optind++; /* tell getopt we recognized x_start */
          break;
        case 'x': /* X limits, ARG OPTIONAL [0,1,2,3] */
          arg_list.matched = 0;
          if (optind >= argc
              || ((strcmp (argv[optind], "-") != 0)
                  && (arg_list.matched
                      = sscanf (argv[optind], "%lf", &arg_list.local_min_x))
                         <= 0))
            {
              arg_list.spec_min_x = arg_list.spec_max_x
                  = arg_list.spec_spacing_x = false;
              break;
            }
          if (arg_list.matched > 0)
            {
              arg_list.spec_min_x = true;
              arg_list.min_x = arg_list.local_min_x;
            }
          else
            arg_list.spec_min_x = false;
          optind++; /* tell getopt we recognized min_x */

          arg_list.matched = 0;
          if (optind >= argc
              || ((strcmp (argv[optind], "-") != 0)
                  && (arg_list.matched
                      = sscanf (argv[optind], "%lf", &arg_list.local_max_x))
                         <= 0))
            {
              arg_list.spec_max_x = arg_list.spec_spacing_x = false;
              break;
            }
          if (arg_list.matched > 0)
            {
              arg_list.spec_max_x = true;
              arg_list.max_x = arg_list.local_max_x;
            }
          else
            arg_list.spec_max_x = false;
          optind++; /* tell getopt we recognized max_x */

          arg_list.matched = 0;
          if (optind >= argc
              || ((strcmp (argv[optind], "-") != 0)
                  && (arg_list.matched = sscanf (argv[optind], "%lf",
                                                 &arg_list.local_spacing_x))
                         <= 0))
            {
              arg_list.spec_spacing_x = false;
              break;
            }
          if (arg_list.matched > 0)
            {
              arg_list.spec_spacing_x = true;
              arg_list.spacing_x = arg_list.local_spacing_x;
            }
          else
            arg_list.spec_spacing_x = false;
          optind++; /* tell getopt we recognized spacing_x */
          break;

        case 'y': /* Y limits, ARG OPTIONAL [0,1,2,3] */
          arg_list.matched = 0;
          if (optind >= argc
              || ((strcmp (argv[optind], "-") != 0)
                  && (arg_list.matched
                      = sscanf (argv[optind], "%lf", &arg_list.local_min_y))
                         <= 0))
            {
              arg_list.spec_min_y = arg_list.spec_max_y
                  = arg_list.spec_spacing_y = false;
              break;
            }
          if (arg_list.matched > 0)
            {
              arg_list.spec_min_y = true;
              arg_list.min_y = arg_list.local_min_y;
            }
          else
            arg_list.spec_min_y = false;
          optind++; /* tell getopt we recognized min_y */

          arg_list.matched = 0;
          if (optind >= argc
              || ((strcmp (argv[optind], "-") != 0)
                  && (arg_list.matched
                      = sscanf (argv[optind], "%lf", &arg_list.local_max_y))
                         <= 0))
            {
              arg_list.spec_max_y = arg_list.spec_spacing_y = false;
              break;
            }
          if (arg_list.matched > 0)
            {
              arg_list.spec_max_y = true;
              arg_list.max_y = arg_list.local_max_y;
            }
          else
            arg_list.spec_max_y = false;
          optind++; /* tell getopt we recognized max_y */

          arg_list.matched = 0;
          if (optind >= argc
              || ((strcmp (argv[optind], "-") != 0)
                  && (arg_list.matched = sscanf (argv[optind], "%lf",
                                                 &arg_list.local_spacing_y))
                         <= 0))
            {
              arg_list.spec_spacing_y = false;
              break;
            }
          if (arg_list.matched > 0)
            {
              arg_list.spec_spacing_y = true;
              arg_list.spacing_y = arg_list.local_spacing_y;
            }
          else
            arg_list.spec_spacing_y = false;
          optind++; /* tell getopt we recognized spacing_y */
          break;

        case 'S': /* Symbol, ARG OPTIONAL	[0,1,2]		*/
          arg_list.new_symbol = true;
          arg_list.symbol_index
              = 1; /* symbol # 1 is switched to by -S alone */
          if (optind >= argc)
            break;
          if (sscanf (argv[optind], "%d", &arg_list.local_symbol_index) <= 0)
            break;
          if (arg_list.local_symbol_index < 0
              || arg_list.local_symbol_index > 255)
            fprintf (stderr,
                     "%s: the symbol type `%d' is disregarded (it should be "
                     "in the range 0..255)\n",
                     progname, arg_list.local_symbol_index);
          else
            arg_list.symbol_index = arg_list.local_symbol_index;
          optind++; /* tell getopt we recognized symbol_index */
          if (optind >= argc)
            break;
          if (sscanf (argv[optind], "%lf", &arg_list.local_symbol_size) <= 0)
            break;
          if (arg_list.local_symbol_size < 0.0)
            fprintf (stderr,
                     "%s: the negative symbol size `%f' is disregarded\n",
                     progname, arg_list.local_symbol_size);
          else if (arg_list.local_symbol_size == 0.0)
            fprintf (stderr,
                     "%s: the request for a zero symbol size is disregarded\n",
                     progname);
          else
            {
              arg_list.symbol_size = arg_list.local_symbol_size;
              arg_list.new_symbol_size = true;
            }
          optind++; /* tell getopt we recognized symbol_size */
          break;

          /* ---------- options with one or more arguments ---------- */

        case 'R' << 8: /* End graph and reposition, ARG REQUIRED [3]*/
          arg_list.old_reposition_trans_x = arg_list.reposition_trans_x;
          arg_list.old_reposition_trans_y = arg_list.reposition_trans_y;
          arg_list.old_reposition_scale = arg_list.reposition_scale;

          if (sscanf (optarg, "%lf", &arg_list.reposition_trans_x) <= 0)
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
          if (sscanf (argv[optind], "%lf", &arg_list.reposition_trans_y) <= 0)
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
          if (sscanf (argv[optind], "%lf", &arg_list.reposition_scale) <= 0)
            {
              fprintf (stderr,
                       "%s: error: the reposition scale factor should be a "
                       "number, but it was `%s'\n",
                       progname, optarg);
              return EXIT_FAILURE;
            }
          if (arg_list.reposition_scale == 0.0)
            {
              fprintf (stderr,
                       "%s: error: the reposition scale factor should not be "
                       "zero\n",
                       progname);
              return EXIT_FAILURE;
            }
          optind++; /* tell getopt we recognized trans_x */

          if (!arg_list.first_file_of_graph)
            /* a graph is in progress (at least one file has been read), so
               it must be ended before we begin the next one */
            {
              if (!arg_list.filter)
                /* We haven't been acting as a real-time filter for the
                   duration of this graph, so the graph isn't already drawn
                   on the display.  Instead, we have a points array and we
                   need to plot it, after computing bounds. */
                {
                  /* fill in any of min_? and max_? that user didn't
                         specify (the prefix "final_" means these arguments
                         were finalized at the time the first file of the plot
                         was processed) */
                  array_bounds (
                      arg_list.p, arg_list.no_of_points,
                      arg_list.final_transpose_axes, arg_list.clip_mode,
                      &arg_list.final_min_x, &arg_list.final_min_y,
                      &arg_list.final_max_x, &arg_list.final_max_y,
                      arg_list.final_spec_min_x, arg_list.final_spec_min_y,
                      arg_list.final_spec_max_x, arg_list.final_spec_max_y);

                  if (arg_list.first_graph_of_multigraph)
                    /* haven't created multigrapher yet, do so now */
                    {
                      if ((arg_list.multigrapher = new_multigrapher (
                               arg_list.output_format, arg_list.bg_color,
                               arg_list.bitmap_size, arg_list.emulate_color,
                               arg_list.max_line_length,
                               arg_list.meta_portable, arg_list.page_size,
                               arg_list.rotation_angle, arg_list.save_screen))
                          == NULL)
                        {
                          fprintf (stderr,
                                   "%s: error: the graphing device could not "
                                   "be opened\n",
                                   progname);
                          return EXIT_FAILURE;
                        }
                    }

                  /* begin graph: push new libplot drawing state onto stack
                         of states; also concatenate the current transformation
                         matrix with a matrix formed from the repositioning
                         parameters (this will be in effect for duration of the
                         graph) */
                  begin_graph (arg_list.multigrapher,
                               arg_list.old_reposition_scale,
                               arg_list.old_reposition_trans_x,
                               arg_list.old_reposition_trans_y);

                  /* font selection, saves typing */
                  if ((arg_list.title_font_name == NULL)
                      && (arg_list.font_name != NULL))
                    arg_list.title_font_name = arg_list.font_name;

                  /* initialize, using (in part) finalized arguments */
                  set_graph_parameters (
                      arg_list.multigrapher, arg_list.frame_line_width,
                      arg_list.frame_color, arg_list.top_label,
                      arg_list.title_font_name,
                      arg_list.title_font_size, /* for title */
                      arg_list.tick_size, arg_list.grid_spec,
                      arg_list.final_min_x, arg_list.final_max_x,
                      arg_list.final_spacing_x, arg_list.final_min_y,
                      arg_list.final_max_y, arg_list.final_spacing_y,
                      arg_list.final_spec_spacing_x,
                      arg_list.final_spec_spacing_y, arg_list.plot_width,
                      arg_list.plot_height, arg_list.margin_below,
                      arg_list.margin_left, arg_list.font_name,
                      arg_list.font_size, /* for abs. label */
                      arg_list.x_label, arg_list.font_name,
                      arg_list.font_size, /* for ord. label */
                      arg_list.y_label, arg_list.no_rotate_y_label,
                      /* these args are portmanteaux */
                      arg_list.final_log_axis,
                      arg_list.final_round_to_next_tick,
                      arg_list.switch_axis_end, arg_list.omit_ticks,
                      /* more args */
                      arg_list.clip_mode, arg_list.blankout_fraction,
                      arg_list.final_transpose_axes);

                  /* draw the graph frame (grid, ticks, etc.); draw a
                         `canvas' (a background opaque white rectangle) only if
                         this isn't the first graph */
                  draw_frame_of_graph (
                      arg_list.multigrapher,
                      (arg_list.first_graph_of_multigraph ? false : true));

                  /* plot the laboriously read-in array */
                  plot_point_array (arg_list.multigrapher, arg_list.p,
                                    arg_list.no_of_points);

                  /* free points array */
                  free (arg_list.p);
                  arg_list.no_of_points = 0;
                  arg_list.first_file_of_graph = false;

                } /* end of not-filter case */

              /* draw graph frame on top of graph, if user requested it */
              if (arg_list.frame_on_top)
                {
                  end_polyline_and_flush (arg_list.multigrapher);
                  draw_frame_of_graph (arg_list.multigrapher, false);
                }

              /* end graph: pop the graph-specific libplot drawing state off
     the stack of drawing states */
              end_graph (arg_list.multigrapher);

              /* on to next graph */
              arg_list.first_graph_of_multigraph = false;
              arg_list.first_file_of_graph = true;

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
          if (arg_list.first_file_of_graph)
            {
              /* For plots with a logarithmic axis, compute logs of axis
                     limits, since coordinates along the axis, as obtained from
                     the reader, are stored in logarithmic form. */
              if (arg_list.log_axis & X_AXIS)
                {
                  if (arg_list.spec_min_x)
                    {
                      if (arg_list.min_x > 0.0)
                        arg_list.min_x = log10 (arg_list.min_x);
                      else
                        {
                          fprintf (stderr,
                                   "%s: error: the limit %g on a logarithmic "
                                   "axis is nonpositive\n",
                                   progname, arg_list.min_x);
                          return EXIT_FAILURE;
                        }
                    }
                  if (arg_list.spec_max_x)
                    {
                      if (arg_list.max_x > 0.0)
                        arg_list.max_x = log10 (arg_list.max_x);
                      else
                        {
                          fprintf (stderr,
                                   "%s: error: the limit %g on a logarithmic "
                                   "axis is nonpositive\n",
                                   progname, arg_list.max_x);
                          return EXIT_FAILURE;
                        }
                    }
                }

              if (arg_list.log_axis & Y_AXIS)
                {
                  if (arg_list.spec_min_y)
                    {
                      if (arg_list.min_y > 0.0)
                        arg_list.min_y = log10 (arg_list.min_y);
                      else
                        {
                          fprintf (stderr,
                                   "%s: error: the limit %g on a logarithmic "
                                   "axis is nonpositive\n",
                                   progname, arg_list.min_y);
                          return EXIT_FAILURE;
                        }
                    }
                  if (arg_list.spec_max_y)
                    {
                      if (arg_list.max_y > 0.0)
                        arg_list.max_y = log10 (arg_list.max_y);
                      else
                        {
                          fprintf (stderr,
                                   "%s: error: the limit %g on a logarithmic "
                                   "axis is nonpositive\n",
                                   progname, arg_list.max_y);
                          return EXIT_FAILURE;
                        }
                    }
                }

              /* We now finalize the following parameters (arguments to
                     set_graph_parameters()), even though we won't call
                     set_graph_parameters() for a while yet, if it turns out we
                     need to act as a real-time filter. */

              /* portmanteaux */
              arg_list.final_log_axis = arg_list.log_axis;
              arg_list.final_round_to_next_tick = arg_list.round_to_next_tick;

              /* bool */
              arg_list.final_transpose_axes = arg_list.transpose_axes;

              /* x-axis specific */
              arg_list.final_min_x = arg_list.min_x;
              arg_list.final_max_x = arg_list.max_x;
              arg_list.final_spacing_x = arg_list.spacing_x;
              arg_list.final_spec_min_x = arg_list.spec_min_x;
              arg_list.final_spec_max_x = arg_list.spec_max_x;
              arg_list.final_spec_spacing_x = arg_list.spec_spacing_x;

              /* y-axis specific */
              arg_list.final_min_y = arg_list.min_y;
              arg_list.final_max_y = arg_list.max_y;
              arg_list.final_spec_min_y = arg_list.spec_min_y;
              arg_list.final_spec_max_y = arg_list.spec_max_y;
              arg_list.final_spacing_y = arg_list.spacing_y;
              arg_list.final_spec_spacing_y = arg_list.spec_spacing_y;

              /* If user didn't specify either the lower limit or the upper
                     limit for an axis, by default we'll round the axis limits
                     to the nearest tick, after computing them.  (If either
                     limit was specified by the user, to request rounding the
                     user must specify the -R option as well.) */
              if (!arg_list.final_spec_min_x && !arg_list.final_spec_max_x)
                arg_list.final_round_to_next_tick |= X_AXIS;
              if (!arg_list.final_spec_min_y && !arg_list.final_spec_max_y)
                arg_list.final_round_to_next_tick |= Y_AXIS;

              /* The case when x_min, x_max, y_min, y_max are all specified
                     by the luser is special: we set the `filter' flag for the
                     duration of this plot, to indicate that we can function as
                     a real-time filter, calling read_and_plot_file() on each
                     file, rather than calling read_file() on each one
                     separately to create an array of points, and then calling
                     plot_point_array(). */
              arg_list.filter
                  = ((arg_list.final_spec_min_x && arg_list.final_spec_max_x
                      && arg_list.final_spec_min_y
                      && arg_list.final_spec_max_y)
                         ? true
                         : false);

            } /* end of first-file-of-plot case */

          if (arg_list.filter)
            /* filter flag is set, will call read_and_plot() on this file */
            {
              if (arg_list.first_file_of_graph)
                {
                  if (arg_list.first_graph_of_multigraph)
                    /* need to create the multigrapher */
                    {
                      if ((arg_list.multigrapher = new_multigrapher (
                               arg_list.output_format, arg_list.bg_color,
                               arg_list.bitmap_size, arg_list.emulate_color,
                               arg_list.max_line_length,
                               arg_list.meta_portable, arg_list.page_size,
                               arg_list.rotation_angle, arg_list.save_screen))
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
                  begin_graph (arg_list.multigrapher,
                               arg_list.reposition_scale,
                               arg_list.reposition_trans_x,
                               arg_list.reposition_trans_y);

                  /* font selection, saves typing */
                  if ((arg_list.title_font_name == NULL)
                      && (arg_list.font_name != NULL))
                    arg_list.title_font_name = arg_list.font_name;

                  /* following will be in effect for the entire plot */
                  set_graph_parameters (
                      arg_list.multigrapher, arg_list.frame_line_width,
                      arg_list.frame_color, arg_list.top_label,
                      arg_list.title_font_name,
                      arg_list.title_font_size, /* for title */
                      arg_list.tick_size, arg_list.grid_spec,
                      arg_list.final_min_x, arg_list.final_max_x,
                      arg_list.final_spacing_x, arg_list.final_min_y,
                      arg_list.final_max_y, arg_list.final_spacing_y,
                      arg_list.final_spec_spacing_x,
                      arg_list.final_spec_spacing_y, arg_list.plot_width,
                      arg_list.plot_height, arg_list.margin_below,
                      arg_list.margin_left, arg_list.font_name,
                      arg_list.font_size, /* on abscissa */
                      arg_list.x_label, arg_list.font_name,
                      arg_list.font_size, /* on ordinate */
                      arg_list.y_label, arg_list.no_rotate_y_label,
                      /* these args are portmanteaux */
                      arg_list.final_log_axis,
                      arg_list.final_round_to_next_tick,
                      arg_list.switch_axis_end, arg_list.omit_ticks,
                      /* more args */
                      arg_list.clip_mode, arg_list.blankout_fraction,
                      arg_list.final_transpose_axes);

                  /* draw the graph frame (grid, ticks, etc.); draw a
                         `canvas' (a background opaque white rectangle) only if
                         this isn't the first graph */
                  draw_frame_of_graph (
                      arg_list.multigrapher,
                      arg_list.first_graph_of_multigraph ? false : true);

                  arg_list.reader = new_reader (
                      arg_list.data_file, arg_list.input_type,
                      arg_list.auto_abscissa, arg_list.delta_x,
                      arg_list.x_start,
                      /* following three are graph-specific */
                      arg_list.final_transpose_axes, arg_list.final_log_axis,
                      arg_list.auto_bump,
                      /* following args are file-specific
                             (they set dataset attributes) */
                      arg_list.symbol_index, arg_list.symbol_size,
                      arg_list.symbol_font_name, arg_list.linemode_index,
                      arg_list.plot_line_width, arg_list.fill_fraction,
                      arg_list.use_color);
                  arg_list.new_symbol = arg_list.new_symbol_size
                      = arg_list.new_symbol_font_name = false;
                  arg_list.new_linemode = arg_list.new_plot_line_width = false;
                  arg_list.new_fill_fraction = arg_list.new_use_color = false;
                }
              else
                /* not first file of plot; do some things anyway */
                {
                  /* set reader parameters that may change when we move
                         from file to file within a plot */
                  alter_reader_parameters (
                      arg_list.reader, arg_list.data_file, arg_list.input_type,
                      arg_list.auto_abscissa, arg_list.delta_x,
                      arg_list.x_start,
                      /* following args set dataset
                             attributes */
                      arg_list.symbol_index, arg_list.symbol_size,
                      arg_list.symbol_font_name, arg_list.linemode_index,
                      arg_list.plot_line_width, arg_list.fill_fraction,
                      arg_list.use_color,
                      /* following bools make up a mask*/
                      arg_list.new_symbol, arg_list.new_symbol_size,
                      arg_list.new_symbol_font_name, arg_list.new_linemode,
                      arg_list.new_plot_line_width, arg_list.new_fill_fraction,
                      arg_list.new_use_color);

                  arg_list.new_symbol = arg_list.new_symbol_size
                      = arg_list.new_symbol_font_name = false;
                  arg_list.new_linemode = arg_list.new_plot_line_width = false;
                  arg_list.new_fill_fraction = arg_list.new_use_color = false;
                }

              /* call read_and_plot_file() on the file; each dataset in the
                     file yields a polyline */
              read_and_plot_file (arg_list.reader, arg_list.multigrapher);

            } /* end of filter case */

          else
            /* filter flag is set, will read and plot this file separately */

            /* Luser didn't specify enough information for us to act as a
               filter, so we do things the hard way: we call read_file() on
               each file to create a points array, and at the end of the
               plot we'll call plot_point_array() on the array.  For now,
               we don't even call set_graph_parameters(). */
            {
              if (arg_list.first_file_of_graph) /* some additional
                                                   initializations */
                {
                  arg_list.p = (Point *)xmalloc (arg_list.points_length
                                                 * sizeof (Point));

                  arg_list.reader = new_reader (
                      arg_list.data_file, arg_list.input_type,
                      arg_list.auto_abscissa, arg_list.delta_x,
                      arg_list.x_start,
                      /* following are graph-specific */
                      arg_list.final_transpose_axes, arg_list.final_log_axis,
                      arg_list.auto_bump,
                      /* following args are file-specific
                             (they set dataset attributes) */
                      arg_list.symbol_index, arg_list.symbol_size,
                      arg_list.symbol_font_name, arg_list.linemode_index,
                      arg_list.plot_line_width, arg_list.fill_fraction,
                      arg_list.use_color);
                  arg_list.new_symbol = arg_list.new_symbol_size
                      = arg_list.new_symbol_font_name = false;
                  arg_list.new_linemode = arg_list.new_plot_line_width = false;
                  arg_list.new_fill_fraction = arg_list.new_use_color = false;
                }
              else /* not first file of plot, but do some things anyway */
                {
                  /* set reader parameters that may change when we move
                         from file to file within a plot */
                  alter_reader_parameters (
                      arg_list.reader, arg_list.data_file, arg_list.input_type,
                      arg_list.auto_abscissa, arg_list.delta_x,
                      arg_list.x_start,
                      /* following args set dataset
                             attributes */
                      arg_list.symbol_index, arg_list.symbol_size,
                      arg_list.symbol_font_name, arg_list.linemode_index,
                      arg_list.plot_line_width, arg_list.fill_fraction,
                      arg_list.use_color,
                      /* following bools make up a mask*/
                      arg_list.new_symbol, arg_list.new_symbol_size,
                      arg_list.new_symbol_font_name, arg_list.new_linemode,
                      arg_list.new_plot_line_width, arg_list.new_fill_fraction,
                      arg_list.new_use_color);

                  arg_list.new_symbol = arg_list.new_symbol_size
                      = arg_list.new_symbol_font_name = false;
                  arg_list.new_linemode = arg_list.new_plot_line_width = false;
                  arg_list.new_fill_fraction = arg_list.new_use_color = false;
                }

              /* add points to points array by calling read_file() on file */
              read_file (arg_list.reader, &arg_list.p, &arg_list.points_length,
                         &arg_list.no_of_points);

            } /* end of not-filter case */

          /* close file */
          if (arg_list.data_file != stdin)
            close_file (optarg, arg_list.data_file);

          arg_list.first_file_of_graph = false;
          break; /* end of `case 1' in switch() [i.e., filename seen] */

          /*---------------- End of options ----------------*/

        default: /* Default, unknown option */
          arg_list.errcnt++;
          arg_list.continue_parse = false;
          break;
        } /* end of switch() */

      if (arg_list.errcnt > 0)
        arg_list.continue_parse = false;
    } /* end of while loop */
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
