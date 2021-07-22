#include "A_config.h"
#include <lvgl.h>
#include <SPI.h>
#include <Arduino_GFX.h>
#include <Arduino_ESP32SPI_DMA.h>
#include <Arduino_GC9A01.h>

Arduino_DataBus *bus = new Arduino_ESP32SPI_DMA(12, 5, 18, 23, -1, VSPI);
Arduino_TFT *gfx = new Arduino_GC9A01(bus, 33, 0 /* rotation */, true /* IPS */);
#define DISPLAY_MAX_X 240
static lv_disp_draw_buf_t disp_buf;
static lv_color_t buf[DISPLAY_MAX_X * 40];
static lv_color_t buf2[DISPLAY_MAX_X * 40];

uint8_t currentKey = LV_KEY_ENTER;
uint8_t currentKeyPressed = 0;

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  gfx->startWrite();
  gfx->writeAddrWindow(area->x1, area->y1, w, h);
  gfx->writePixels(&color_p->full, w*h);
  gfx->endWrite();
  lv_disp_flush_ready(disp);
}


void updateButton()
{
  static uint8_t laststate[3] = {0,0,1};
  if(digitalRead(WATCH_BUTTON_UP) != laststate[0])
  {
    if(laststate[0] == 0 && currentKeyPressed == 0) 
    {
      Serial.println("KeyUp->Pressed");
      currentKey = LV_KEY_LEFT;
      currentKeyPressed = 1;
      laststate[0] = 1;
    }
    else if(laststate[0])
    {
      currentKeyPressed = 0;
      laststate[0] = 0;
    }
  }
  if(digitalRead(WATCH_BUTTON_DOWN) != laststate[1])
  {
    if(laststate[1] == 0 && currentKeyPressed == 0) 
    {
      Serial.println("KeyDown->Pressed");
      currentKey = LV_KEY_RIGHT;
      currentKeyPressed = 1;
      laststate[1] = 1;
    }
    else if(laststate[1])
    {
      currentKeyPressed = 0;
      laststate[1] = 0;
    }
  }
  if(digitalRead(WATCH_BUTTON_ENTER) != laststate[2])
  {
    if(laststate[2] && currentKeyPressed == 0) 
    {
      Serial.println("KeyEnter->Pressed");
      currentKey = LV_KEY_ENTER;
      currentKeyPressed = 1;
      laststate[2] = 0;
    }
    else if(laststate[2] == 0)
    {
      currentKeyPressed = 0;
      laststate[2] = 1;
    }
  }
}
void encoder_with_keys_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data)
{
  data->key = currentKey;            /*Get the last pressed or released key*/
                                     /* use LV_KEY_ENTER for encoder press */
  if(currentKeyPressed) 
  {
    data->state = LV_INDEV_STATE_PRESSED;
  }
  else {
      data->state = LV_INDEV_STATE_RELEASED;
  }
}
void timer_test(lv_timer_t *tm)
{
  static uint8_t i = 0;
  lv_obj_scroll_to_view(lv_obj_get_child((lv_obj_t *)tm->user_data, i), LV_ANIM_ON);
  ++i;
  if(i == 20)i = 0;
}

static void scroll_event_cb(lv_event_t * e)
{
    lv_obj_t * cont = lv_event_get_target(e);

    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);
    lv_coord_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;

    lv_coord_t r = lv_obj_get_height(cont) * 7 / 10;
    uint32_t i;
    uint32_t child_cnt = lv_obj_get_child_cnt(cont);
    for(i = 0; i < child_cnt; i++) {
        lv_obj_t * child = lv_obj_get_child(cont, i);
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);

        lv_coord_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;

        lv_coord_t diff_y = child_y_center - cont_y_center;
        diff_y = LV_ABS(diff_y);

        /*Get the x of diff_y on a circle.*/
        lv_coord_t x;
        /*If diff_y is out of the circle use the last point of the circle (the radius)*/
        if(diff_y >= r) {
            x = r;
        } else {
            /*Use Pythagoras theorem to get x from radius and y*/
            lv_coord_t x_sqr = r * r - diff_y * diff_y;
            lv_sqrt_res_t res;
            lv_sqrt(x_sqr, &res, 0x8000);   /*Use lvgl's built in sqrt root function*/
            x = r - res.i;
        }

        /*Translate the item by the calculated X coordinate*/
        lv_obj_set_style_translate_x(child, x, 0);

        /*Use some opacity with larger translations*/
        lv_opa_t opa = lv_map(x, 0, r, LV_OPA_TRANSP, LV_OPA_COVER);
        lv_obj_set_style_opa(child, LV_OPA_COVER - opa, 0);
    }
}

/**
 * Translate the object as they scroll
 */
void lv_example_scroll_6(void)
{
    lv_obj_t * cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, 180, 180);
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_event_cb(cont, scroll_event_cb, LV_EVENT_SCROLL, NULL);
    lv_obj_set_style_radius(cont, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_clip_corner(cont, true, 0);
    lv_obj_set_scroll_dir(cont, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);

    uint32_t i;
    for(i = 0; i < 20; i++) {
        lv_obj_t * btn = lv_btn_create(cont);
        lv_obj_set_width(btn, lv_pct(100));

        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text_fmt(label, "Button %d", i);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
    }

    /*Update the buttons position manually for first*/
    lv_event_send(cont, LV_EVENT_SCROLL, NULL);

    /*Be sure the fist button is in the middle*/
    lv_obj_scroll_to_view(lv_obj_get_child(cont, 0), LV_ANIM_OFF);
    lv_timer_create(timer_test, 1000, cont);
}

void setup()
{
  pinMode(WATCH_BUTTON_UP,INPUT);
  pinMode(WATCH_BUTTON_DOWN,INPUT);
  pinMode(WATCH_BUTTON_ENTER,INPUT_PULLUP);
  pinMode(VIBRATE_MOTOR, OUTPUT);

  digitalWrite(VIBRATE_MOTOR, 1);
  delay(20);
  digitalWrite(VIBRATE_MOTOR, 0);
  Serial.begin(115200);
  lv_init();
  
  gfx->begin(80000000); /* TFT init */

  gfx->fillScreen(GREEN);
  lv_disp_draw_buf_init(&disp_buf, buf, buf2, DISPLAY_MAX_X * 40);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = 240;
  disp_drv.ver_res = 240;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &disp_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_ENCODER;
  indev_drv.read_cb = encoder_with_keys_read;
  lv_indev_drv_register(&indev_drv);

  lv_example_scroll_6();
}

void loop()
{
  lv_timer_handler();
  updateButton();
  //delay(1);
}