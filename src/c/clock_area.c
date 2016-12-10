#include "clock_area.h"
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/ffont.h>
#include <pebble-fctx/fpath.h>
#include <pebble.h>
#include "settings.h"

char time_hours[3];
char time_minutes[3];

Layer* clock_area_layer;

FFont* leco;
GRect screen_rect;

void update_clock_area_layer(Layer* l, GContext* ctx) {
  // check layer bounds
  GRect bounds = layer_get_unobstructed_bounds(l);

  // initialize FCTX, the fancy 3rd party drawing library that all the cool kids
  // use
  FContext fctx;

  fctx_init_context(&fctx, ctx);
  fctx_set_color_bias(&fctx, 0);
  fctx_set_fill_color(&fctx, globalSettings.timeColor);

  // calculate font size
  int font_size = 4 * bounds.size.h / 7 + 6;

  // avenir + avenir bold metrics
  int v_padding = bounds.size.h / 20;
  int h_adjust = -4;
  int v_adjust = 0;

  h_adjust -= ACTION_BAR_WIDTH / 2 + 1;

  FPoint time_pos;
  fctx_begin_fill(&fctx);
  fctx_set_text_em_height(&fctx, leco, font_size);

  // draw hours
  time_pos.x = INT_TO_FIXED(bounds.size.w / 2 + h_adjust);
  time_pos.y = INT_TO_FIXED(v_padding + v_adjust);
  fctx_set_offset(&fctx, time_pos);
  fctx_draw_string(&fctx, time_hours, leco, GTextAlignmentCenter,
                   FTextAnchorTop);

  // draw minutes
  time_pos.y = INT_TO_FIXED(bounds.size.h - v_padding + v_adjust);
  fctx_set_offset(&fctx, time_pos);
  fctx_draw_string(&fctx, time_minutes, leco, GTextAlignmentCenter,
                   FTextAnchorBaseline);
  fctx_end_fill(&fctx);

  fctx_deinit_context(&fctx);
}

void ClockArea_init(Window* window) {
  // record the screen size, since we NEVER GET IT AGAIN
  screen_rect = layer_get_bounds(window_get_root_layer(window));

  GRect bounds;
  bounds = GRect(0, 0, screen_rect.size.w, screen_rect.size.h);

  // init the clock area layer
  clock_area_layer = layer_create(bounds);
  layer_add_child(window_get_root_layer(window), clock_area_layer);
  layer_set_update_proc(clock_area_layer, update_clock_area_layer);
  leco = ffont_create_from_resource(RESOURCE_ID_LECO_REGULAR_FFONT);
}

void ClockArea_deinit() {
  layer_destroy(clock_area_layer);

  ffont_destroy(leco);
}

void ClockArea_redraw() { layer_mark_dirty(clock_area_layer); }

void ClockArea_update_time(struct tm* time_info) {
  // hours
  strftime(time_hours, sizeof(time_hours), "%k", time_info);

  // minutes
  strftime(time_minutes, sizeof(time_minutes), "%M", time_info);

  ClockArea_redraw();
}
