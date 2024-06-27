/* This file is part of the GNU plotutils package.  Copyright (C) 1989,
   1990, 1991, 1995, 1996, 1997, 1998, 1999, 2000, 2005, 2008, 2009, Free
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

/* This file contains the main routine, and a few support subroutines, for
   GNU graph. */

#include "arg.h"
#include "file.h"
#include "fontlist.h"
#include "libcommon.h"
#include "sys-defines.h"

int
main (int argc, char *argv[])
{
  get_args (argc, argv);

  if (arg_list.errcnt > 0)
    {
      fprintf (stderr, "Try `%s --help' for more information\n", progname);
      return EXIT_FAILURE;
    }
  if (arg_list.show_version)
    {
      display_version (progname, written, copyright);
      return EXIT_SUCCESS;
    }
  if (arg_list.do_list_fonts)
    {
      int success;

      success = list_fonts (arg_list.output_format, progname);
      if (success)
        return EXIT_SUCCESS;
      else
        return EXIT_FAILURE;
    }
  if (arg_list.show_fonts)
    {
      int success;

      success = display_fonts (arg_list.output_format, progname);
      if (success)
        return EXIT_SUCCESS;
      else
        return EXIT_FAILURE;
    }
  if (arg_list.show_usage)
    {
      display_usage (progname, hidden_options, usage_appendage, 2);
      return EXIT_SUCCESS;
    }

  /* End of command-line parse.  At this point, we need to terminate the
     graph currently in progress, if it's nonempty (i.e. if one or more
     files have been read). */

  if (arg_list.first_file_of_graph == false)
    {
      /* At least one file was read.  If we're acting as a real-time
         filter, then the graph is already drawn on the display and there's
         nothing for us to do.  Instead, we have a points array and we need
         to plot it, after computing bounds. */
      if (!arg_list.filter)
        {

          /* fill in any of min_? and max_? that user didn't specify (the
             prefix "final_" means these arguments were finalized at the
             time the first file of the plot was processed) */
          array_bounds (arg_list.p, arg_list.no_of_points,
                        arg_list.final_transpose_axes, arg_list.clip_mode,
                        &arg_list.final_min_x, &arg_list.final_min_y,
                        &arg_list.final_max_x, &arg_list.final_max_y,
                        arg_list.final_spec_min_x, arg_list.final_spec_min_y,
                        arg_list.final_spec_max_x, arg_list.final_spec_max_y);

          if (arg_list.first_graph_of_multigraph)
            /* still haven't created multigrapher, do so now */
            {
              if ((arg_list.multigrapher = new_multigrapher (
                       arg_list.output_format, arg_list.bg_color,
                       arg_list.bitmap_size, arg_list.emulate_color,
                       arg_list.max_line_length, arg_list.meta_portable,
                       arg_list.page_size, arg_list.rotation_angle,
                       arg_list.save_screen))
                  == NULL)
                {
                  fprintf (
                      stderr,
                      "%s: error: the graphing device could not be opened\n",
                      progname);
                  return EXIT_FAILURE;
                }
            }

          /* begin graph: push new libplot drawing state onto stack of
             states; also concatenate the current transformation matrix
             with a matrix formed from the repositioning parameters (this
             will take effect for the duration of the graph) */
          begin_graph (arg_list.multigrapher, arg_list.reposition_scale,
                       arg_list.reposition_trans_x,
                       arg_list.reposition_trans_y);

          /* font selection, saves typing */
          if ((arg_list.title_font_name == NULL)
              && (arg_list.font_name != NULL))
            arg_list.title_font_name = arg_list.font_name;

          set_graph_parameters (
              arg_list.multigrapher, arg_list.frame_line_width,
              arg_list.frame_color, arg_list.top_label,
              arg_list.title_font_name, arg_list.title_font_size, /*for title*/
              arg_list.tick_size, arg_list.grid_spec, arg_list.final_min_x,
              arg_list.final_max_x, arg_list.final_spacing_x,
              arg_list.final_min_y, arg_list.final_max_y,
              arg_list.final_spacing_y, arg_list.final_spec_spacing_x,
              arg_list.final_spec_spacing_y, arg_list.plot_width,
              arg_list.plot_height, arg_list.margin_below,
              arg_list.margin_left, arg_list.font_name,
              arg_list.font_size, /* for abscissa label */
              arg_list.x_label, arg_list.font_name,
              arg_list.font_size, /* for ordinate label */
              arg_list.y_label, arg_list.no_rotate_y_label,
              /* these args are portmanteaux */
              arg_list.final_log_axis, arg_list.final_round_to_next_tick,
              arg_list.switch_axis_end, arg_list.omit_ticks,
              /* more args */
              arg_list.clip_mode, arg_list.blankout_fraction,
              arg_list.final_transpose_axes);

          /* draw the graph frame (grid, ticks, etc.); draw a `canvas' (a
             background opaque white rectangle) only if this isn't the
             first graph */
          // TODO: add it here?
          draw_frame_of_graph (arg_list.multigrapher,
                               arg_list.first_graph_of_multigraph ? false
                                                                  : true);

          /* plot the laboriously read-in array */
          plot_point_array (arg_list.multigrapher, arg_list.p,
                            arg_list.no_of_points);

          /* free points array */
          free (arg_list.p);
          arg_list.no_of_points = 0;

        } /* end of not-filter case */

      /* draw graph frame on top of graph, if user requested it */
      if (arg_list.frame_on_top)
        {
          end_polyline_and_flush (arg_list.multigrapher);
          draw_frame_of_graph (arg_list.multigrapher, false);
        }

      draw_legend_of_graph (arg_list.multigrapher, false);
      /* end graph: pop drawing state off the stack of drawing states */
      end_graph (arg_list.multigrapher);

    } /* end of nonempty-graph case */

  /* finish up by deleting our multigrapher (one must have been created,
     since we always read at least stdin) */
  if (delete_multigrapher (arg_list.multigrapher) < 0)
    {
      fprintf (stderr, "%s: error: the graphing device could not be closed\n",
               progname);
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
