#include <pebble.h>
#include "clock_area.h"
#include "settings.h"
#include "sidebar.h"
#include "util.h"
#include "health.h"

// windows and layers
static Window* mainWindow;
static Layer* windowLayer;


void update_clock();
void redrawScreen();
void tick_handler(struct tm *tick_time, TimeUnits units_changed);


void update_clock() {
  time_t rawTime;
  struct tm* timeInfo;

  time(&rawTime);
  timeInfo = localtime(&rawTime);

  ClockArea_update_time(timeInfo);
  Sidebar_updateTime(timeInfo);
}

/* forces everything on screen to be redrawn -- perfect for keeping track of settings! */
void redrawScreen() {

  window_set_background_color(mainWindow, globalSettings.timeBgColor);

  // maybe the language changed!
  update_clock();

  // update the sidebar
  Sidebar_redraw();

  ClockArea_redraw();
}

static void main_window_load(Window *window) {
  window_set_background_color(window, globalSettings.timeBgColor);

  // create the sidebar
  Sidebar_init(window);

  ClockArea_init(window);

  // Make sure the time is displayed from the start
  redrawScreen();
  update_clock();
}

static void main_window_unload(Window *window) {
  ClockArea_deinit();
  Sidebar_deinit();
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_clock();
}

// force the sidebar to redraw any time the battery state changes
void batteryStateChanged(BatteryChargeState charge_state) {
  Sidebar_redraw();
}

// fixes for disappearing elements after notifications
// (from http://codecorner.galanter.net/2016/01/08/solved-issue-with-pebble-framebuffer-after-notification-is-dismissed/)
static void app_focus_changing(bool focusing) {
  if (focusing) {
     layer_set_hidden(windowLayer, true);
  }
}

static void app_focus_changed(bool focused) {
  if (focused) {
     layer_set_hidden(windowLayer, false);
     layer_mark_dirty(windowLayer);
  }
}

static void init() {
  setlocale(LC_ALL, "");

  srand(time(NULL));

  // init settings
  Settings_init();

  // init health service
  if (!Health_init(redrawScreen)) {
      APP_LOG(APP_LOG_LEVEL_WARNING, "Could not init health service");
  }

  // Create main Window element and assign to pointer
  mainWindow = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(mainWindow, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(mainWindow, true);

  windowLayer = window_get_root_layer(mainWindow);

  // Register with TickTimerService
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);


  // register with battery service
  battery_state_service_subscribe(batteryStateChanged);

  // set up focus change handlers
  app_focus_service_subscribe_handlers((AppFocusHandlers){
    .did_focus = app_focus_changed,
    .will_focus = app_focus_changing
  });
}

static void deinit() {
  // Destroy Window
  window_destroy(mainWindow);
  Health_deinit();
  battery_state_service_unsubscribe();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
