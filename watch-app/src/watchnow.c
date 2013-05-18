#include <stdint.h>
#include <string.h>

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "resource_ids.auto.h"

#define BITMAP_BUFFER_BYTES 1024

// C12AA2C8-71BB-44DA-96AA-B364A39DF885
#define MY_UUID {0xc1, 0x2a, 0xa2, 0xc8, 0x71, 0xbb, 0x44, 0xda, 0x96, 0xaa, 0xb3, 0x64, 0xa3, 0x9d, 0xf8, 0x85}
PBL_APP_INFO(MY_UUID, "WatchNow", "@NeoNacho", 0x1, 0x0, DEFAULT_MENU_ICON, APP_INFO_STANDARD_APP);

static struct WatchNowData {
  Window window;
  TextLayer text_layer;
  char message[255];
  AppSync sync;
  uint8_t sync_buffer[32];
} s_data;

enum {
  NOW_MESSAGE_KEY = 0x0,  // TUPLE_CSTRING
};

// TODO: Error handling
static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  (void) dict_error;
  (void) app_message_error;
  (void) context;
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
  (void) old_tuple;

  switch (key) {
  case NOW_MESSAGE_KEY:
    strncpy(s_data.message, new_tuple->value->cstring, 255);
    layer_mark_dirty(&s_data.text_layer.layer);
    break;
  default:
    return;
  }
}

static void watchnow_app_init(AppContextRef c) {
  (void) c;

  resource_init_current_app(&WATCHNOW_APP_RESOURCES);

  Window* window = &s_data.window;
  window_init(window, "WatchNow");
  window_set_background_color(window, GColorBlack);
  window_set_fullscreen(window, true);

  text_layer_init(&s_data.text_layer, GRect(0, 100, 144, 68));
  text_layer_set_text_color(&s_data.text_layer, GColorWhite);
  text_layer_set_background_color(&s_data.text_layer, GColorClear);
  text_layer_set_font(&s_data.text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(&s_data.text_layer, GTextAlignmentCenter);
  text_layer_set_text(&s_data.text_layer, s_data.message);
  layer_add_child(&window->layer, &s_data.text_layer.layer);

  Tuplet initial_values[] = {
    TupletCString(NOW_MESSAGE_KEY, "None\u00B0C"),
  };
  app_sync_init(&s_data.sync, s_data.sync_buffer, sizeof(s_data.sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
                sync_tuple_changed_callback, sync_error_callback, NULL);

  window_stack_push(window, true);
}

 static void watchnow_app_deinit(AppContextRef c) {
   app_sync_deinit(&s_data.sync);
 }

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &watchnow_app_init,
    .deinit_handler = &watchnow_app_deinit,
    .messaging_info = {
      .buffer_sizes = {
        .inbound = 64,
        .outbound = 16,
      }
    }
  };
  app_event_loop(params, &handlers);
}
