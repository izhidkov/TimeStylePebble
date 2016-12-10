#pragma once
#include <pebble.h>

extern int SidebarWidgets_xOffset;

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
void SidebarWidgets_updateFonts();
void SidebarWidgets_updateTime(struct tm* timeInfo);
