#include <pebble.h>
#include <math.h>
#include "settings.h"
#include "languages.h"
#include "util.h"
#include "health.h"
#include "sidebar_widgets.h"

int SidebarWidgets_xOffset;

// sidebar icons
GDrawCommandImage* dateImage;
GDrawCommandImage* batteryImage;
GDrawCommandImage* batteryChargeImage;

// fonts
GFont smSidebarFont;
GFont mdSidebarFont;
GFont lgSidebarFont;
GFont currentSidebarFont;
GFont batteryFont;

char currentDayName[8];
char currentDayNum[5];
char currentMonth[8];

// the widgets
int BatteryMeter_getHeight();
void BatteryMeter_draw(GContext* ctx, int yPosition);

int DateWidget_getHeight();
void DateWidget_draw(GContext* ctx, int yPosition);
SidebarWidget batteryMeterWidget;
SidebarWidget dateWidget;
SidebarWidget healthWidget;
SidebarWidget heartRateWidget;

  GDrawCommandImage* sleepImage;
  GDrawCommandImage* stepsImage;
  GDrawCommandImage* heartImage;

  int Health_getHeight();
  void Health_draw(GContext* ctx, int yPosition);
  void Sleep_draw(GContext* ctx, int yPosition);
  void Steps_draw(GContext* ctx, int yPosition);

  int HeartRate_getHeight();
  void HeartRate_draw(GContext* ctx, int yPosition);

void SidebarWidgets_init() {
  // load fonts
  smSidebarFont = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
  mdSidebarFont = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  lgSidebarFont = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);

  // load the sidebar graphics
  dateImage = gdraw_command_image_create_with_resource(RESOURCE_ID_DATE_BG);
  batteryImage = gdraw_command_image_create_with_resource(RESOURCE_ID_BATTERY_BG);
  batteryChargeImage = gdraw_command_image_create_with_resource(RESOURCE_ID_BATTERY_CHARGE);

    sleepImage = gdraw_command_image_create_with_resource(RESOURCE_ID_HEALTH_SLEEP);
    stepsImage = gdraw_command_image_create_with_resource(RESOURCE_ID_HEALTH_STEPS);
    heartImage = gdraw_command_image_create_with_resource(RESOURCE_ID_HEALTH_HEART);

  // set up widgets' function pointers correctly
  batteryMeterWidget.getHeight = BatteryMeter_getHeight;
  batteryMeterWidget.draw      = BatteryMeter_draw;

  dateWidget.getHeight = DateWidget_getHeight;
  dateWidget.draw      = DateWidget_draw;

    healthWidget.getHeight = Health_getHeight;
    healthWidget.draw = Health_draw;
    
    heartRateWidget.getHeight = HeartRate_getHeight;
    heartRateWidget.draw = HeartRate_draw;

}

void SidebarWidgets_deinit() {
  gdraw_command_image_destroy(dateImage);
  gdraw_command_image_destroy(batteryImage);
  gdraw_command_image_destroy(batteryChargeImage);

    gdraw_command_image_destroy(stepsImage);
    gdraw_command_image_destroy(sleepImage);
}

void SidebarWidgets_updateFonts() {
    currentSidebarFont = mdSidebarFont;
    batteryFont = smSidebarFont;
}

// c can't do true modulus on negative numbers, apparently
// from http://stackoverflow.com/questions/11720656/modulo-operation-with-negative-numbers
int mod(int a, int b) {
    int r = a % b;
    return r < 0 ? r + b : r;
}

void SidebarWidgets_updateTime(struct tm* timeInfo) {
  // set all the date strings
  strftime(currentDayNum,  3, "%e", timeInfo);

  strncpy(currentDayName, dayNames[timeInfo->tm_wday], sizeof(currentDayName));
  strncpy(currentMonth, monthNames[timeInfo->tm_mon], sizeof(currentMonth));

  // remove padding on date num, if needed
  if(currentDayNum[0] == ' ') {
    currentDayNum[0] = currentDayNum[1];
    currentDayNum[1] = '\0';
  }

}


/********** functions for the battery meter widget **********/

int BatteryMeter_getHeight() {
  BatteryChargeState chargeState = battery_state_service_peek();

  if(chargeState.is_charging) {
    return 14; // graphic only height
  } else {
    return 27; // heights with text
  }
}

void BatteryMeter_draw(GContext* ctx, int yPosition) {

  BatteryChargeState chargeState = battery_state_service_peek();
  uint8_t battery_percent = (chargeState.charge_percent > 0) ? chargeState.charge_percent : 5;

  graphics_context_set_text_color(ctx, globalSettings.sidebarTextColor);

  char batteryString[6];
  int batteryPositionY = yPosition - 5; // correct for vertical empty space on battery icon

  if (batteryImage) {
    gdraw_command_image_recolor(batteryImage, globalSettings.iconFillColor, globalSettings.iconStrokeColor);
    gdraw_command_image_draw(ctx, batteryImage, GPoint(3 + SidebarWidgets_xOffset, batteryPositionY));
  }

  if(chargeState.is_charging) {
    if(batteryChargeImage) {
      // the charge "bolt" icon uses inverted colors
      gdraw_command_image_recolor(batteryChargeImage, globalSettings.iconStrokeColor, globalSettings.iconFillColor);
      gdraw_command_image_draw(ctx, batteryChargeImage, GPoint(3 + SidebarWidgets_xOffset, batteryPositionY));
    }
  } else {

    int width = roundf(18 * battery_percent / 100.0f);

    graphics_context_set_fill_color(ctx, globalSettings.iconStrokeColor);


    graphics_fill_rect(ctx, GRect(6 + SidebarWidgets_xOffset, 8 + batteryPositionY, width, 8), 0, GCornerNone);
  }

  // never show battery % while charging, because of this issue:
  // https://github.com/freakified/TimeStylePebble/issues/11
  if(!chargeState.is_charging) {
      snprintf(batteryString, sizeof(batteryString), "%d", battery_percent);

      graphics_draw_text(ctx,
                         batteryString,
                         batteryFont,
                         GRect(-4 + SidebarWidgets_xOffset, 14 + batteryPositionY, 38, 20),
                         GTextOverflowModeFill,
                         GTextAlignmentCenter,
                         NULL);
  }
}

/********** current date widget **********/

int DateWidget_getHeight() {
    return 58;
}

void DateWidget_draw(GContext* ctx, int yPosition) {
  graphics_context_set_text_color(ctx, globalSettings.sidebarTextColor);

  // compensate for extra space that appears on the top of the date widget
  yPosition -= 7;

  // first draw the day name
  graphics_draw_text(ctx,
                     currentDayName,
                     currentSidebarFont,
                     GRect(-5 + SidebarWidgets_xOffset, yPosition, 40, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);

  // next, draw the date background
  // (an image in normal mode, a rectangle in large font mode)
    if(dateImage) {
      gdraw_command_image_recolor(dateImage, globalSettings.iconFillColor, globalSettings.iconStrokeColor);
      gdraw_command_image_draw(ctx, dateImage, GPoint(3 + SidebarWidgets_xOffset, yPosition + 23));
    }    

  // next, draw the date number
  graphics_context_set_text_color(ctx, globalSettings.iconStrokeColor);

  int yOffset = 26;
  graphics_draw_text(ctx,
                     currentDayNum,
                     currentSidebarFont,
                     GRect(-5 + SidebarWidgets_xOffset, yPosition + yOffset, 40, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);


   // switch back to normal color for the rest
  graphics_context_set_text_color(ctx, globalSettings.sidebarTextColor);

    graphics_draw_text(ctx,
                       currentMonth,
                       currentSidebarFont,
                       GRect(-5 + SidebarWidgets_xOffset, yPosition + 47, 40, 20),
                       GTextOverflowModeFill,
                       GTextAlignmentCenter,
                       NULL);


}

/***** Health Widget *****/

int Health_getHeight() {
  if(Health_isUserSleeping()) {
    return 44;
  } else {
    return 32;
  }
}

void Health_draw(GContext* ctx, int yPosition) {
  // check if we're showing the sleep data or step data
  if(Health_isUserSleeping()) {
    Sleep_draw(ctx, yPosition);
  } else {
    Steps_draw(ctx, yPosition);
  }
}

void Sleep_draw(GContext* ctx, int yPosition) {
  if(sleepImage) {
    gdraw_command_image_recolor(sleepImage, globalSettings.iconFillColor, globalSettings.iconStrokeColor);
    gdraw_command_image_draw(ctx, sleepImage, GPoint(3 + SidebarWidgets_xOffset, yPosition - 7));
  }

  // get sleep in seconds
  HealthValue sleep_seconds = Health_getSleepSeconds();

  // convert to hours/minutes
  int sleep_minutes = sleep_seconds / 60;
  int sleep_hours   = sleep_minutes / 60;

  // find minutes remainder
  sleep_minutes %= 60;

  char sleep_text[4];

  snprintf(sleep_text, sizeof(sleep_text), "%ih", sleep_hours);

  graphics_context_set_text_color(ctx, globalSettings.sidebarTextColor);
  graphics_draw_text(ctx,
                     sleep_text,
                     mdSidebarFont,
                     GRect(-2 + SidebarWidgets_xOffset, yPosition + 14, 34, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);

  snprintf(sleep_text, sizeof(sleep_text), "%im", sleep_minutes);

  graphics_draw_text(ctx,
                     sleep_text,
                     smSidebarFont,
                     GRect(-2 + SidebarWidgets_xOffset, yPosition + 30, 34, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);

}

void Steps_draw(GContext* ctx, int yPosition) {

  if(stepsImage) {
    gdraw_command_image_recolor(stepsImage, globalSettings.iconFillColor, globalSettings.iconStrokeColor);
    gdraw_command_image_draw(ctx, stepsImage, GPoint(3 + SidebarWidgets_xOffset, yPosition - 7));
  }
GFont stepsSidebarFont = mdSidebarFont;
  char steps_text[8];

    HealthValue steps = Health_getSteps();
  // format step string
     int yOffset = 13;
     if (steps < 999) {
	stepsSidebarFont = lgSidebarFont;
        yOffset = 10;
     }
      if (steps > 9999) {
        yOffset = 15;
	stepsSidebarFont = smSidebarFont;
      }

      snprintf(steps_text, sizeof(steps_text), "%li", steps);
  graphics_context_set_text_color(ctx, globalSettings.sidebarTextColor);

  graphics_draw_text(ctx,
                     steps_text,
                     stepsSidebarFont,
                     GRect(-2 + SidebarWidgets_xOffset, yPosition + yOffset, 35, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);
}

int HeartRate_getHeight() {
    return 40;
}

void HeartRate_draw(GContext* ctx, int yPosition) {
  if(heartImage) {
    gdraw_command_image_recolor(heartImage, globalSettings.iconFillColor, globalSettings.iconStrokeColor);
    gdraw_command_image_draw(ctx, heartImage, GPoint(3 + SidebarWidgets_xOffset, yPosition));
  }


  HealthValue heart_rate = Health_getHeartRate();
  char heart_rate_text[8];

  snprintf(heart_rate_text, sizeof(heart_rate_text), "%li", heart_rate);

  graphics_context_set_text_color(ctx, globalSettings.sidebarTextColor);
  graphics_draw_text(ctx,
                     heart_rate_text,
                     lgSidebarFont,
                     GRect(-4 + SidebarWidgets_xOffset, yPosition + 18, 38, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);
}

