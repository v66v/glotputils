#include "file.h"
#include "arg.h"
#include "libcommon.h"

void
open_file_for_reading (char *filename, FILE **input)
{
  FILE *data_file;

  data_file = fopen (filename, "r");
  if (data_file == NULL)
    {
      fprintf (stderr, "%s: %s: %s\n", progname, filename, strerror (errno));
      exit (EXIT_FAILURE);
    }
  else
    *input = data_file;
}

void
close_file (char *filename, FILE *stream)
{
  if (fclose (stream) < 0)
    fprintf (stderr, "%s: the input file `%s' could not be closed\n", progname,
             filename);
}
