#include <pebble.h>
#include "settings.h"

Settings globalSettings;

void Settings_init() {
  // load the default colors
  globalSettings.timeBgColor      = GColorBlack;
  globalSettings.sidebarTextColor = GColorWhite;

    globalSettings.timeColor      = GColorWhite;
    globalSettings.sidebarColor   = GColorBlack;

    globalSettings.iconFillColor = GColorBlack;
    globalSettings.iconStrokeColor = globalSettings.sidebarTextColor; // exciting
}
