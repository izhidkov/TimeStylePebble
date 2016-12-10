#pragma once
#include <pebble.h>

typedef struct {
  // color settings
  GColor timeColor;
  GColor timeBgColor;
  GColor sidebarColor;
  GColor sidebarTextColor;
  GColor iconFillColor;
  GColor iconStrokeColor;
} Settings;

extern Settings globalSettings;


void Settings_init();
