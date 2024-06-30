#include "arg_common.h"

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
  { "legend-position", ARG_REQUIRED, NULL, 'L' << 8 },
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
const char *progname = "graph";
const char *written = "Written by Robert S. Maier., v66v";
const char *copyright = "Copyright (C) 2024 by v66v";
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
  .legend_plot = false,
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
