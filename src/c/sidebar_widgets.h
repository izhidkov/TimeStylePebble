#pragma once
#include <pebble.h>

extern int SidebarWidgets_xOffset;

/*
 * The different types of sidebar widgets:
 * we'll give them numbers so that we can index them in settings
 */
typedef enum {
  EMPTY                     = 0,
  BATTERY_METER             = 2,
  DATE                      = 4,
  HEALTH                    = 10,
  HEARTRATE                 = 12
} SidebarWidgetType;

typedef struct {
  /*
   * Returns the pixel height of the widget, taking into account all current
   * settings that would affect this, such as font size
   */
  int (*getHeight)();

  /*
   * Draws the widget using the provided graphics context
   */
  void (*draw)(GContext* ctx, int yPosition);
} SidebarWidget;

void SidebarWidgets_init();
void SidebarWidgets_deinit();
SidebarWidget getSidebarWidgetByType(SidebarWidgetType type);
void SidebarWidgets_updateFonts();
void SidebarWidgets_updateTime(struct tm* timeInfo);
