#include <pebble.h>
#include "health.h"

static HealthEventCallback s_health_event_callback;

static bool s_sleeping;
static HealthValue s_sleep_seconds;
static HealthValue s_steps;
static HealthValue s_heart_rate;

static bool is_health_metric_accessible(HealthMetric metric) {
    time_t start = time_start_of_today();
    time_t end = time(NULL);

    HealthServiceAccessibilityMask mask = health_service_metric_accessible(metric, start, end);
    return mask & HealthServiceAccessibilityMaskAvailable;
}

static inline HealthValue get_health_value_sum_today(HealthMetric metric) {
    return is_health_metric_accessible(metric) ? health_service_sum_today(metric) : 0;
}

static void health_event_handler(HealthEventType event, void *context) {
    if (event == HealthEventSignificantUpdate) {
        health_event_handler(HealthEventSleepUpdate, NULL);
        health_event_handler(HealthEventMovementUpdate, NULL);
        health_event_handler(HealthEventHeartRateUpdate, NULL);
    } else if (event == HealthEventSleepUpdate) {
        HealthActivityMask mask = health_service_peek_current_activities();
        s_sleeping = (mask & HealthActivitySleep) || (mask & HealthActivityRestfulSleep);
        s_sleep_seconds = get_health_value_sum_today(HealthMetricSleepSeconds);
    } else if (event == HealthEventMovementUpdate) {
        HealthActivityMask mask = health_service_peek_current_activities();
        s_sleeping = (mask & HealthActivitySleep) || (mask & HealthActivityRestfulSleep);
        s_steps = get_health_value_sum_today(HealthMetricStepCount);
    } else if (event == HealthEventHeartRateUpdate) {
        HealthServiceAccessibilityMask hr = health_service_metric_accessible(HealthMetricHeartRateBPM, time(NULL), time(NULL));
        if (hr & HealthServiceAccessibilityMaskAvailable) {
            s_heart_rate = health_service_peek_current_value(HealthMetricHeartRateBPM);
        }
    }

    if (s_health_event_callback) s_health_event_callback();
}

bool Health_init(HealthEventCallback callback) {
    bool b = health_service_events_subscribe(health_event_handler, NULL);
    if (b) {
        health_event_handler(HealthEventSignificantUpdate, NULL);
        s_health_event_callback = callback;
    }
    return b;
}

void Health_deinit(void) {
    health_service_events_unsubscribe();
}

bool Health_isUserSleeping(void) {
    return s_sleeping;
}

HealthValue Health_getSleepSeconds(void) {
    return s_sleep_seconds;
}

HealthValue Health_getSteps(void) {
    return s_steps;
}

HealthValue Health_getHeartRate(void) {
    return s_heart_rate;
}
