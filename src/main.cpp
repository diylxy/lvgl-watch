#include <lvgl.h>
#include <lv_examples.h>
#include <SPI.h>
#include <Arduino_GFX.h>
#include <Arduino_ESP32SPI_DMA.h>
#include <Arduino_GC9A01.h>
#include <esp_task_wdt.h>
Arduino_DataBus *bus = new Arduino_ESP32SPI_DMA(12, 5, 18, 23, -1, VSPI);
Arduino_GFX *gfx = new Arduino_GC9A01(bus, 33, 0 /* rotation */, true /* IPS */);

static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  gfx->draw16bitRGBBitmap(area->x1, area->y1, &color_p->full, w, h);
  lv_disp_flush_ready(disp);
}

bool my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data)
{
  data->state = LV_INDEV_STATE_REL;
  return false; /*Return `false` because we are not buffering and no more data to read*/
}



void setup()
{
  lv_init();
  
  gfx->begin(80000000); /* TFT init */

  gfx->fillScreen(GREEN);
  lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

  /*Initialize the display*/
  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = 240;
  disp_drv.ver_res = 240;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.buffer = &disp_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the (dummy) input device driver*/
  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  lv_demo_widgets();
}

void loop()
{
  lv_task_handler(); /* let the GUI do its work */
  delay(1);
}
