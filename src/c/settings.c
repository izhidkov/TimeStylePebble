#include <pebble.h>
#include "settings.h"

Settings globalSettings;

void Settings_init() {
  // load the default colors
  globalSettings.timeBgColor      = GColorBlack;
  globalSettings.sidebarTextColor = GColorWhite;

    globalSettings.timeColor      = GColorWhite;
    globalSettings.sidebarColor   = GColorBlack;

  // set the default widgets
  globalSettings.widgets[0] = DATE;
  globalSettings.widgets[1] = HEARTRATE;
  globalSettings.widgets[2] = HEALTH;

    globalSettings.iconFillColor = GColorBlack;
    globalSettings.iconStrokeColor = globalSettings.sidebarTextColor; // exciting
}
