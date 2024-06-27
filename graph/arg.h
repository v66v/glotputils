#pragma once
#include "extern.h"
#include "getopt.h"


#define ARG_NONE 0
#define ARG_REQUIRED 1
#define ARG_OPTIONAL 2

const char *optstring = "-BCHOQVstE:F:f:g:h:k:K:I:l:L:m:N:q:R:r:T:u:w:W:X:Y:a:"
                        ":x::y::S::"; /* initial hyphen requests no reordering
                                       */

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

/* null-terminated list of options, such as obsolete-but-still-maintained
   options or undocumented options, which we don't show to the user */
const int hidden_options[] = { (int)('T' << 8), 0 };

const char *progname = "graph"; /* name of this program */
const char *written = "Written by Robert S. Maier.";
const char *copyright = "Copyright (C) 2009 Free Software Foundation, Inc.";

const char *usage_appendage = " [FILE]...\n\
With no FILE, or when FILE is -, read standard input.\n";

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

int get_args (int argc, char *argv[]);
bool parse_pen_string (const char *pen_s);
