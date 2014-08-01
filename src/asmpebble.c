#include "pebble.h"
#include "schedule.h"

#define PROGRAM_LAYERS 3

Window *window;
TextLayer *text_date_layer;
TextLayer *text_time_layer;
Layer *line_layer;
TextLayer *text_program_layers[PROGRAM_LAYERS];
TextLayer *text_start_layers[PROGRAM_LAYERS];
static char *start_text[] = { "00:01", "00:02", "00:03" };

void line_layer_update_callback(Layer *layer, GContext* ctx) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

void update_programs(time_t now) {
  // Find active programs
  struct program *program;
  int layer = 0;
  struct tm *tm;

  for (program = programs; program->title && layer < PROGRAM_LAYERS; program++) {
    if (program->start <= now && now <= program->end && program->end-program->start < 7200) {
      tm = localtime(&program->start);
      strftime(start_text[layer], 6, "%R", tm);
      text_layer_set_text(text_start_layers[layer], start_text[layer]);
      text_layer_set_text(text_program_layers[layer], program->title);
      layer++;
    } else {
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Skip: %d <= %d <= %d", program->start, (int)now, program->end);
    }
  }
  for (program = programs; program->title && layer < PROGRAM_LAYERS; program++) {
    if (program->start > now) {
      tm = localtime(&program->start);
      strftime(start_text[layer], 6, "%R", tm);
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Next: %d/%d %d:%d", (int)program->start, (int)now, tm->tm_hour, tm->tm_min);
      text_layer_set_text(text_start_layers[layer], start_text[layer]);
      text_layer_set_text(text_program_layers[layer], program->title);
      layer++;
    }
  }
}

void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  // Need to be static because they're used by the system later.
  static char time_text[] = "00:00";
  static char date_text[] = "Xxxxxxxxx 00";

  char *time_format;
  time_t now = time(NULL);

  if (!tick_time) {
    tick_time = localtime(&now);
  }

  // TODO: Only update the date when it's changed.
  strftime(date_text, sizeof(date_text), "%B %e", tick_time);
  text_layer_set_text(text_date_layer, date_text);


  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  strftime(time_text, sizeof(time_text), time_format, tick_time);

  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }

  text_layer_set_text(text_time_layer, time_text);

  update_programs(now);
}

void handle_deinit(void) {
  tick_timer_service_unsubscribe();
}

void handle_init(void) {
  int i;

  window = window_create();
  window_stack_push(window, true /* Animated */);
  window_set_background_color(window, GColorBlack);

  Layer *window_layer = window_get_root_layer(window);

  text_time_layer = text_layer_create(GRect(7, 0, 144-14, 33));
  text_layer_set_text_color(text_time_layer, GColorWhite);
  text_layer_set_background_color(text_time_layer, GColorClear);
  text_layer_set_text_alignment(text_time_layer, GTextAlignmentRight);
  text_layer_set_font(text_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
  layer_add_child(window_layer, text_layer_get_layer(text_time_layer));

  text_date_layer = text_layer_create(GRect(9, 33, 144-18, 34));
  text_layer_set_text_color(text_date_layer, GColorWhite);
  text_layer_set_background_color(text_date_layer, GColorClear);
  text_layer_set_text_alignment(text_date_layer, GTextAlignmentRight);
  text_layer_set_font(text_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  layer_add_child(window_layer, text_layer_get_layer(text_date_layer));

  GRect line_frame = GRect(0, 67, 144, 2);
  line_layer = layer_create(line_frame);
  layer_set_update_proc(line_layer, line_layer_update_callback);
  layer_add_child(window_layer, line_layer);

  for (i = 0; i < PROGRAM_LAYERS; i++) {
    text_program_layers[i] = text_layer_create(GRect(34, 70 + i*32, 110, 32));
    text_layer_set_text_color(text_program_layers[i], GColorWhite);
    text_layer_set_background_color(text_program_layers[i], GColorClear);
    text_layer_set_font(text_program_layers[i], fonts_get_system_font(FONT_KEY_GOTHIC_14));
    layer_add_child(window_layer, text_layer_get_layer(text_program_layers[i]));

    text_start_layers[i] = text_layer_create(GRect(0, 70 + i*32, 34, 16));
    text_layer_set_text_color(text_start_layers[i], GColorWhite);
    text_layer_set_background_color(text_start_layers[i], GColorClear);
    text_layer_set_font(text_start_layers[i], fonts_get_system_font(FONT_KEY_GOTHIC_14));
    layer_add_child(window_layer, text_layer_get_layer(text_start_layers[i]));
  }

  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
  handle_minute_tick(NULL, MINUTE_UNIT);
}


int main(void) {
  handle_init();

  app_event_loop();

  handle_deinit();
}
