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
#include "plotter.h"
#include "sys-defines.h"

int
main (int argc, char *argv[])
{
  if ((parse_args (argc, argv) == EXIT_FAILURE))
    return EXIT_FAILURE;

  if (show_args () == EXIT_FAILURE)
    return EXIT_FAILURE;

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
          if (plot_graph_no_filter (&arg_list) == EXIT_FAILURE)
            return EXIT_FAILURE;
        } /* end of not-filter case */

      /* draw graph frame on top of graph, if user requested it */
      if (arg_list.frame_on_top)
        {
          end_polyline_and_flush (arg_list.multigrapher);
          draw_frame_of_graph (arg_list.multigrapher, false);
        }

      if (arg_list.legend_plot)
        {
          draw_legend_of_graph (arg_list.multigrapher);
        }
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
