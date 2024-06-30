#include "arg.h"
#include "arg_parse.h"
#include "file.h"
#include "libcommon.h"
#include "graph_plotter.h"
#include "sys-defines.h"

int
parse_args (int argc, char *argv[])
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

      if (parse_flags (&arg_list, argc, argv) == EXIT_FAILURE)
        return EXIT_FAILURE;

      if (arg_list.errcnt > 0)
        arg_list.continue_parse = false;
    } /* end of while loop */
  return 2;
}

int
print_arg_info ()
{
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
  return 2;
}
