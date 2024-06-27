#pragma once
#include "extern.h"
#include "getopt.h"

#define ARG_NONE 0
#define ARG_REQUIRED 1
#define ARG_OPTIONAL 2

extern const char *optstring; /* initial hyphen requests no reordering
                               */

extern struct option long_options[];

/* null-terminated list of options, such as obsolete-but-still-maintained
   options or undocumented options, which we don't show to the user */
extern const int hidden_options[];

extern const char *progname;
extern const char *written;
extern const char *copyright;

extern const char *usage_appendage;

typedef struct
{
  int option;
  int opt_index;
  int errcnt; /* errors encountered in getopt parsing */
  int matched;
  bool using_getopt;   /* true until end of command-line options */
  bool continue_parse; /* reset e.g. when --help or --version seen */
  bool show_version;   /* show version message? */
  bool show_usage;     /* show usage message? */
  bool show_fonts;     /* supply help on fonts? */
  bool do_list_fonts;  /* show a list of fonts? */
  bool filter;         /* will we act as a filter? */
  bool new_symbol;
  bool new_symbol_size;
  bool new_symbol_font_name;
  bool new_linemode;
  bool new_plot_line_width;
  bool new_fill_fraction;
  bool new_use_color;
  bool first_file_of_graph;
  bool first_graph_of_multigraph;
  FILE *data_file;

  /* Variables related to the point reader */

  Reader *reader;
  data_type input_type; /* by default we read ascii data */
  bool auto_bump;       /* auto-bump linemode between polylines? */
  bool auto_abscissa;   /* generate abscissa values automatically? */
  double x_start;       /* start and increment, for auto-abscissa */
  double delta_x;
  /* polyline attributes */
  int linemode_index;     /* linemode for polylines, 1=solid, etc. */
  double plot_line_width; /* polyline width (as frac. of display width),
               negative means default provided by libplot) */
  int symbol_index;       /* 0=none, 1=dot, 2=plus, 3=asterisk, etc. */
  double symbol_size;     /* symbol size (frac. of plotting box size) */
  double fill_fraction;   /* negative means regions aren't filled */
  bool use_color;         /* color / monochrome */

  /* Variables related to both the point reader and the point plotter */

  bool transpose_axes; /* true means -x applies to y axis, etc. */

  /* Variables related to the multigrapher, i.e. point plotter */

  Multigrapher *multigrapher;

  /* command-line parameters (constant over multigrapher operation) */
  const char *output_format; /* libplot output format */
  const char *bg_color;      /* color of background, if non-NULL */
  const char *bitmap_size;
  const char *emulate_color;
  const char *max_line_length;
  const char *meta_portable;
  const char *page_size;
  const char *rotation_angle;
  bool save_screen; /* save screen, i.e. no erase before plot? */

  /* graph-specific parameters (may change from graph to graph) */

  grid_type grid_spec;     /* frame type for current graph */
  bool no_rotate_y_label;  /* used for pre-X11R6 servers */
  const char *frame_color; /* color of frame (and graph, if no -C)*/
  int clip_mode;           /* clipping mode (cf. gnuplot) */
  /* following variables are portmanteau: x and y are included as
     bitfields*/
  int log_axis;           /* log axes or linear axes? */
  int round_to_next_tick; /* round axis limits to nearest tick? */
  int switch_axis_end;    /* axis at top/right instead of bottom/left? */
  int omit_ticks;         /* omit ticks and tick labels from an axis? */

  /* graph dimensions, expressed as fractions of the width of the libplot
     graphics display [by convention square]; <0.0 means use libplot
     default */
  double frame_line_width; /* width of lines in the graph frame */

  /* dimensions of graphing area, expressed as fractions of the width of
     the libplot graphics display [by convention square] */
  double margin_below; /* margin below the plot */
  double margin_left;  /* margin left of the plot */
  double plot_height;  /* height of the plot */
  double plot_width;   /* width of the plot */

  /* dimensions, expressed as fractions of the size of the plotting area */
  double tick_size;         /* size of tick marks (< 0.0 allowed) */
  double font_size;         /* fontsize */
  double title_font_size;   /* title fontsize */
  double blankout_fraction; /* this fraction of size of plotting box
  is erased before the plot is drawn */

  /* text-related */
  const char *font_name;        /* font name, NULL -> device default */
  const char *title_font_name;  /* title font name, NULL -> default */
  const char *symbol_font_name; /* symbol font name, NULL -> default */
  const char *x_label;          /* label for the x axis, NULL -> no label */
  const char *y_label;          /* label for the y axis, NULL -> no label */
  const char *top_label;        /* title above the plot, NULL -> no title */

  /* user-specified limits on the axes */
  double min_x, min_y, max_x, max_y;
  double spacing_x, spacing_y;

  /* flags indicating which axis limits the user has specified */
  bool spec_min_x, spec_min_y;
  bool spec_max_x, spec_max_y;
  bool spec_spacing_x, spec_spacing_y;

  /* misc. local variables used in getopt parsing, counterparts to the above */
  double local_x_start, local_delta_x;
  int local_grid_style;
  int local_symbol_index;
  int local_clip_mode;
  double local_symbol_size, local_font_size, local_title_font_size;
  double local_frame_line_width, local_plot_line_width;
  double local_min_x, local_min_y;
  double local_max_x, local_max_y;
  double local_spacing_x, local_spacing_y;
  double local_fill_fraction;

  /* `finalized' arguments to set_graph_parameters() (computed at the time
     the first file of a graph is seen, and continuing in effect over the
     duration of the graph) */
  int final_log_axis;
  int final_round_to_next_tick;
  double final_min_x, final_max_x, final_spacing_x;
  double final_min_y, final_max_y, final_spacing_y;
  bool final_spec_min_x, final_spec_min_y;
  bool final_spec_max_x, final_spec_max_y;
  bool final_spec_spacing_x, final_spec_spacing_y;
  bool final_transpose_axes;

  /* for storage of data points (if we're not acting as a filter) */
  Point *p;          /* points array */
  int points_length; /* length of the points array, in points */
  int no_of_points;  /* number of points stored in it */

  /* support for multigraphing */
  double reposition_trans_x, reposition_trans_y;
  double reposition_scale;
  double old_reposition_trans_x, old_reposition_trans_y;
  double old_reposition_scale;

  /* sui generis */
  bool frame_on_top;

} ARG_LIST;

extern ARG_LIST arg_list;

int get_args (int argc, char *argv[]);
bool parse_pen_string (const char *pen_s);
