#include <pebble.h>
#include <ctype.h>
#include <math.h>
#include "settings.h"
#include "languages.h"
#include "sidebar.h"

#define V_PADDING_DEFAULT 8
#define V_PADDING_COMPACT 4

// "private" functions
// layer update callbacks
void updateRectSidebar(Layer *l, GContext* ctx);


Layer* sidebarLayer;


void Sidebar_init(Window* window) {
  // init the sidebar layer
  GRect bounds;
  GRect scr_rect = layer_get_bounds(window_get_root_layer(window));
bounds = GRect(scr_rect.size.w - ACTION_BAR_WIDTH, 0, ACTION_BAR_WIDTH, scr_rect.size.h);

  // init the widgets
  SidebarWidgets_init();

  sidebarLayer = layer_create(bounds);
  layer_add_child(window_get_root_layer(window), sidebarLayer);

    layer_set_update_proc(sidebarLayer, updateRectSidebar);
}

void Sidebar_deinit() {
  layer_destroy(sidebarLayer);

  SidebarWidgets_deinit();
}

void Sidebar_redraw() {

  // redraw the layer
  layer_mark_dirty(sidebarLayer);

}

void Sidebar_updateTime(struct tm* timeInfo) {
  SidebarWidgets_updateTime(timeInfo);

  // redraw the sidebar in case it changed in any way
  Sidebar_redraw();
}

bool isAutoBatteryShown() {
    BatteryChargeState chargeState = battery_state_service_peek();

      if(chargeState.charge_percent <= 10 || chargeState.is_charging) {
        return true;
      }

  return false;
}


void updateRectSidebar(Layer *l, GContext* ctx) {
  GRect bounds = layer_get_unobstructed_bounds(l);

  // this ends up being zero on every rectangular platform besides emery
  SidebarWidgets_xOffset = (ACTION_BAR_WIDTH - 30) / 2;

  SidebarWidgets_updateFonts();

  graphics_context_set_fill_color(ctx, globalSettings.sidebarColor);
  graphics_fill_rect(ctx, layer_get_bounds(l), 0, GCornerNone);

  graphics_context_set_text_color(ctx, globalSettings.sidebarTextColor);

  bool showAutoBattery = isAutoBatteryShown();

  // do we need to replace a widget?
  // if so, determine which widget should be replaced
  SidebarWidget midWidget = heartRateWidget;
  if(showAutoBattery) {

      midWidget = batteryMeterWidget;
  }


  // calculate the three widget positions
  int topWidgetPos = V_PADDING_DEFAULT;
  int lowerWidgetPos = bounds.size.h - V_PADDING_DEFAULT - healthWidget.getHeight();

  // vertically center the middle widget using MATH
  int middleWidgetPos = ((lowerWidgetPos - midWidget.getHeight()) + (topWidgetPos + dateWidget.getHeight())) / 2;

  // draw the widgets
  dateWidget.draw(ctx, topWidgetPos);
    midWidget.draw(ctx, middleWidgetPos);
  healthWidget.draw(ctx, lowerWidgetPos);
}
