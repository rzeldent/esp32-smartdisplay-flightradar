#include <calibrate.h>

#include <lvgl.h>

/*
// Defines for calibration
#define CALIBRATION_CROSS_LENGTH 10

void btn_click_action_calibration(lv_event_t *event)
{
  if (event)
  {
    // Get the hardware coordinates from the driver here.
    lv_indev_t *indev = lv_indev_get_act();
    lv_indev_get_point(indev, &calibration_point);
    log_i("Calibration touch at %d, %d\n", calibration_point.x, calibration_point.y);
  }
}

void smartdisplay_touch_calibrate()
{
  log_i("Starting calibration");

  // Disable calibration adjustment
  touch_calibration_data.valid = false;

  // Create screen points
  const lv_point_t screen_pt[] = {
      {.x = CALIBRATION_CROSS_LENGTH, .y = CALIBRATION_CROSS_LENGTH},               // X=~0, Y=~0
      {.x = LCD_WIDTH - CALIBRATION_CROSS_LENGTH, .y = LCD_HEIGHT / 2},             // X=~Max, Y=~Max/2
      {.x = CALIBRATION_CROSS_LENGTH, .y = LCD_HEIGHT - CALIBRATION_CROSS_LENGTH}}; // X=~0, Y=~Max

  // Results for touch points
  lv_point_t touch_pt[sizeof(screen_pt) / sizeof(lv_point_t)];

  // Save the old screen
  lv_obj_t *oldscreen = lv_scr_act();

  // Create a calibration screen, full size!
  lv_obj_t *screen_calibration = lv_obj_create(NULL);
  lv_obj_clear_flag(screen_calibration, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_text_align(screen_calibration, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_size(screen_calibration, LV_HOR_RES, LV_VER_RES);
  // Make the screen one big button
  lv_obj_t *calibration_button_area = lv_btn_create(screen_calibration);
  // Use the complete screen
  lv_obj_set_size(calibration_button_area, LV_HOR_RES, LV_VER_RES);
  // Set some text
  lv_obj_t *calibration_scrren_text = lv_label_create(screen_calibration);
  lv_obj_set_size(calibration_scrren_text, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_set_align(calibration_scrren_text, LV_ALIGN_CENTER);
  lv_label_set_text(calibration_scrren_text, "CALIBRATION\n\nUse the stylus to click on the crosses\nthat appear as precise as possible.");

  // lv_obj_remove_style(calibration_button_area, NULL, LV_PART_MAIN | LV_STATE_DEFAULT);

  // lv_obj_set_style_opa(big_btn, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT); // Opacity zero
  lv_obj_add_event_cb(calibration_button_area, btn_click_action_calibration, LV_EVENT_CLICKED, NULL);
  lv_obj_set_layout(calibration_button_area, 0); // Disable layout of children. The first registered layout starts at 1

  lv_scr_load(screen_calibration);

  lv_point_t *pt = (lv_point_t *)&screen_pt;
  for (int p = 0; p < sizeof(screen_pt) / sizeof(lv_point_t); p++, pt++)
  {
    // Clear the screen
    lv_obj_clean(calibration_button_area);
    delay(1000);
    log_i("Calibrate screen point %d: (%d, %d)", p, pt->x, pt->y);

    // Cross
    lv_point_t line_points[] = {
        {pt->x - CALIBRATION_CROSS_LENGTH, pt->y},
        {pt->x + CALIBRATION_CROSS_LENGTH, pt->y},
        {pt->x, pt->y},
        {pt->x, pt->y - CALIBRATION_CROSS_LENGTH},
        {pt->x, pt->y + CALIBRATION_CROSS_LENGTH}};

    // Create style
    lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 1);
    lv_style_set_line_color(&style_line, lv_color_black());

    lv_obj_t *cross = lv_line_create(calibration_button_area);
    lv_line_set_points(cross, line_points, sizeof(line_points) / sizeof(lv_point_t));
    lv_obj_add_style(cross, &style_line, 0);
    lv_obj_align(cross, LV_ALIGN_TOP_LEFT, 0, 0);

    calibration_point = (lv_point_t){-1, -1};
    while (calibration_point.y == -1)
      lv_timer_handler();

    touch_pt[p] = calibration_point;
    lv_obj_del(cross);
    lv_timer_handler();

    log_i("Result %d: Screen (%d,%d), Touch (%d, %d)", p, pt->x, pt->y, touch_pt[p].x, touch_pt[p].y);
  }

  // Calculate the transform parameters
  smartdisplay_compute_touch_calibration(screen_pt, touch_pt);
}
*/