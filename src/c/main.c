#include <pebble.h>
 
enum {
  KEY_READING = 0x0,
  KEY_READING_1YR = 0x1,
  KEY_DATE_MODE = 0x2
};

#define PKEY_DATE_MODE 0
#define DATE_MODE_A 0
#define DATE_MODE_B 1
#define DATE_MODE_C 2
#define DATE_MODE_D 3
#define DATE_MODE_E 4
#define DATE_MODE_F 5
  
static Window *s_main_window;
static TextLayer *s_date1_layer;
static TextLayer *s_date2_layer;
static TextLayer *s_time_layer;
static TextLayer *s_reading_layer;
static TextLayer *s_reading_1yr_layer;

static GFont s_date_font;
static GFont s_time_font;
static GFont s_reading_font;
static GFont s_reading_1yr_font;

static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  unsigned short hour;
  static char buffer[6];  \
  static char date1_buffer[10];
  static char date2_buffer[19];
  static char* month_array[] = {
      "January",
      "February",
      "March",
      "April",
      "May",
      "June",
      "July",
      "August",
      "September",
      "October",
      "November",
      "December"
  };
  static char* day_array[] = {
      "Sunday",
      "Monday",
      "Tuesday",
      "Wednesday",
      "Thursday",
      "Friday",
      "Saturday"
   };
 
  switch(persist_read_int(PKEY_DATE_MODE)) {
  case DATE_MODE_A:
    snprintf(date1_buffer,sizeof(date1_buffer), "%s", day_array[tick_time->tm_wday]);
    snprintf(date2_buffer,sizeof(date2_buffer), "%s %d", month_array[tick_time->tm_mon], tick_time->tm_mday);    
    break;
  case DATE_MODE_B:
    snprintf(date1_buffer,sizeof(date1_buffer), "%d", 1900+tick_time->tm_year);
    snprintf(date2_buffer,sizeof(date2_buffer), "%s %d", month_array[tick_time->tm_mon], tick_time->tm_mday);
    break; 
  case DATE_MODE_C:
    snprintf(date1_buffer,sizeof(date1_buffer), "%s", day_array[tick_time->tm_wday]);
    snprintf(date2_buffer,sizeof(date2_buffer), "%d %s", tick_time->tm_mday, month_array[tick_time->tm_mon]);
    break;
  case DATE_MODE_D:
    snprintf(date1_buffer,sizeof(date1_buffer), "%d", 1900+tick_time->tm_year);
    snprintf(date2_buffer,sizeof(date2_buffer), "%d %s", tick_time->tm_mday, month_array[tick_time->tm_mon]);
    break;
  case DATE_MODE_E:
    snprintf(date1_buffer,sizeof(date1_buffer), "%s", day_array[tick_time->tm_wday]);
    snprintf(date2_buffer,sizeof(date2_buffer), "%d/%d/%d", tick_time->tm_mon+1, tick_time->tm_mday, 1900+tick_time->tm_year);  
    break;
  case DATE_MODE_F:
    snprintf(date1_buffer,sizeof(date1_buffer), "%s", day_array[tick_time->tm_wday]);
    snprintf(date2_buffer,sizeof(date2_buffer), "%d/%d/%d", tick_time->tm_mday, tick_time->tm_mon+1, 1900+tick_time->tm_year);
    break;
  default:
    snprintf(date1_buffer,sizeof(date1_buffer), "%d", 1900+tick_time->tm_year);
    snprintf(date2_buffer,sizeof(date2_buffer), "%s %d", month_array[tick_time->tm_mon], tick_time->tm_mday);
    break;
  }
   
  if(clock_is_24h_style()) 
    hour = tick_time->tm_hour;
  else {
    if(tick_time->tm_hour>12)
      hour = tick_time->tm_hour-12;
    else if(tick_time->tm_hour==0)
      hour = 12;
    else
      hour = tick_time->tm_hour;
  }
  snprintf(buffer, sizeof(buffer), "%d:%02d",hour,tick_time->tm_min);  

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
  text_layer_set_text(s_date1_layer, date1_buffer);
  text_layer_set_text(s_date2_layer, date2_buffer);
}

static void main_window_load(Window *window) {
  //Create GBitmap, then set to created BitmapLayer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND_CO2);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));

  s_date_font = fonts_get_system_font(FONT_KEY_GOTHIC_28);
  s_time_font = fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49);
  //s_time_font = fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS);
  //s_time_font = fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS);
  s_reading_font = fonts_get_system_font(FONT_KEY_GOTHIC_28);
  s_reading_1yr_font = fonts_get_system_font(FONT_KEY_GOTHIC_28);
  
  // Create date1 TextLayer
  s_date1_layer = text_layer_create(GRect(0, -3, 144, 32));
  text_layer_set_background_color(s_date1_layer, GColorClear);
  text_layer_set_text_color(s_date1_layer, GColorWhite);
  text_layer_set_text_alignment(s_date1_layer, GTextAlignmentCenter);
  text_layer_set_text(s_date1_layer, "Loading");
  text_layer_set_font(s_date1_layer, s_date_font);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date1_layer));
  
  // Create date2 TextLayer
  s_date2_layer = text_layer_create(GRect(0, 24, 144, 32));
  text_layer_set_background_color(s_date2_layer, GColorClear);
  text_layer_set_text_color(s_date2_layer, GColorWhite);
  text_layer_set_text_alignment(s_date2_layer, GTextAlignmentCenter);
  text_layer_set_text(s_date2_layer, "Loading");
  text_layer_set_font(s_date2_layer, s_date_font);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date2_layer));
 
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 52, 144, 58));
  //s_time_layer = text_layer_create(GRect(0, 56, 144, 58));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, s_time_font);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  
  // Create reading Layer
  s_reading_layer = text_layer_create(GRect(0, 106, 144, 32));
  text_layer_set_background_color(s_reading_layer, GColorClear);
  text_layer_set_text_color(s_reading_layer, GColorWhite);
  text_layer_set_text_alignment(s_reading_layer, GTextAlignmentCenter);
  text_layer_set_text(s_reading_layer, "Loading...");
  text_layer_set_font(s_reading_layer, s_reading_font);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_reading_layer));

  // Create reading 1yr Layer
  s_reading_1yr_layer = text_layer_create(GRect(0, 133, 144, 32));
  text_layer_set_background_color(s_reading_1yr_layer, GColorClear);
  text_layer_set_text_color(s_reading_1yr_layer, GColorWhite);
  text_layer_set_text_alignment(s_reading_1yr_layer, GTextAlignmentCenter);
  text_layer_set_text(s_reading_1yr_layer, "Loading...");
  text_layer_set_font(s_reading_1yr_layer, s_reading_1yr_font);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_reading_1yr_layer));
   
  // Make sure the time is displayed from the start
  update_time();
}

static void main_window_unload(Window *window) {
  //fonts_unload_custom_font(s_time_font);
  //fonts_unload_custom_font(s_reading_font);
  //fonts_unload_custom_font(s_reading_1yr_font);
  //fonts_unload_custom_font(s_reading_10yr_font);
  
  //Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);

  //Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
  
  // Destroy TextLayer
  text_layer_destroy(s_date1_layer);
  text_layer_destroy(s_date2_layer);
  text_layer_destroy(s_time_layer);
  
  // Destroy reading elements
  text_layer_destroy(s_reading_layer);
  text_layer_destroy(s_reading_1yr_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  // Get reading update every 30 minutes
  if(tick_time->tm_min % 30 == 0) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();
  }
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char reading_buffer[7];
  static char reading_1yr_buffer[7];
  
  // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_READING:
      snprintf(reading_buffer, sizeof(reading_buffer), "%s", t->value->cstring);
      text_layer_set_text(s_reading_layer, reading_buffer);
      break;
    case KEY_READING_1YR:
      snprintf(reading_1yr_buffer, sizeof(reading_1yr_buffer), "%s", t->value->cstring);
      text_layer_set_text(s_reading_1yr_layer, reading_1yr_buffer);
      break;
    case KEY_DATE_MODE:
      if(strncmp(t->value->cstring,"full_us_day___",14)==0)
        persist_write_int(PKEY_DATE_MODE, DATE_MODE_A);
      else if(strncmp(t->value->cstring,"full_us_year__",14)==0)
        persist_write_int(PKEY_DATE_MODE, DATE_MODE_B);
      else if(strncmp(t->value->cstring,"full_euro_day_",14)==0)
        persist_write_int(PKEY_DATE_MODE, DATE_MODE_C);
      else if(strncmp(t->value->cstring,"full_euro_year",14)==0)
        persist_write_int(PKEY_DATE_MODE, DATE_MODE_D);
      else if(strncmp(t->value->cstring,"abbrev_us_____",14)==0)
        persist_write_int(PKEY_DATE_MODE, DATE_MODE_E);
      else if(strncmp(t->value->cstring,"abbrev_euro___",14)==0)
        persist_write_int(PKEY_DATE_MODE, DATE_MODE_F);
      else
        persist_write_int(PKEY_DATE_MODE, DATE_MODE_B);
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }
  update_time();
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
