#pragma once
#include <pebble.h>
#include "sidebar_widgets.h"
extern SidebarWidget batteryMeterWidget;
extern SidebarWidget dateWidget;
extern SidebarWidget healthWidget;
extern SidebarWidget heartRateWidget;
// "public" functions
void Sidebar_init(Window* window);
void Sidebar_deinit();
void Sidebar_redraw();
void Sidebar_updateTime(struct tm* timeInfo);
