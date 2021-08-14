#include "A_config.h"
LV_IMG_DECLARE(bird_0);
LV_IMG_DECLARE(bird_1);
LV_IMG_DECLARE(bird_2);
LV_IMG_DECLARE(game_over);
LV_IMG_DECLARE(pic_pipe);
LV_IMG_DECLARE(tutorial);

//lvgl对象定义部分
static lv_obj_t *scr_flappybird;
static lv_obj_t *img_bird;
static lv_obj_t *img_notice;
static lv_timer_t *flappy_bird_timer;
static lv_obj_t *lblscore;
//end of lvgl对象定义
//////////////////////////////////////////////////////////////////////////////////////////////////
//flappybird变量定义部分
//gameover判定：
//检查bird新的y坐标是否在安全区内，且在屏幕内
//如果是则继续，否则显示gameover
//管道定义
//管道安全区域：图片y坐标+190-图片y坐标+290
//所以管道y范围: -166~-24，假定两端都留一个管口
//小鸟当前坐标判定：小鸟图片x坐标+24, 小鸟图片y坐标+24
//小鸟加速度=1px/frame
#define BIRD_GRAVITY 1
#define BIRD_INIT_SPEED -8
#define BIRD_X_POS 120
#define BIRD_INIT_Y 120
#define PIPE_SPEED_INIT -5
#define PIPE_SPEED_INC 10
#define PIPE_SPEED_MAX -10
typedef struct
{
    int16_t x;
    int16_t y;
    lv_obj_t *img_pipe;
} flappy_bird_pipe;
flappy_bird_pipe pipe_obj;

const lv_img_dsc_t *birds[3] =
    {
        &bird_0,
        &bird_1,
        &bird_2};
uint8_t current_image = 0;            //当前显示的图片指针
int16_t bird_speed = BIRD_INIT_SPEED; //当前小鸟速度，向下为正
int16_t bird_y = 120;
int16_t pipe_speed = PIPE_SPEED_INIT;
uint8_t pipe_speed_inc_counter = 0; //管道速度增加计数器，每隔PIPE_SPEED_INC帧管道速度自增（坐标系原因实际上是-1）
uint32_t score = 0;
bool running = false;
//end of flappy变量
//////////////////////////////////////////////////////////////////////////////////////////////////
//flappybird函数定义
/**
 * @brief 判断小鸟是否存活
 * @param y 小鸟y坐标
 * @return 小鸟是否存活，如果是则返回true，否则返回false
 */
bool isalive(int16_t y)
{
    int16_t pipe_safe_start = pipe_obj.y + 190;
    if (y < 0 || y >= 240)
        return false;
    if (BIRD_X_POS >= pipe_obj.x && BIRD_X_POS <= pipe_obj.x + 48)
    {
        if (y <= pipe_safe_start || y >= pipe_safe_start + 100)
        {
            return false;
        }
    }
    return true;
}
void newPipe()
{
    pipe_obj.y = random(-166, -74);
    pipe_obj.x = 240;
}
void restart()
{
    lv_obj_set_style_opa(img_notice, 0, 0);
    score = 0;
    bird_y = 120;
    lv_timer_resume(flappy_bird_timer);
    bird_speed = BIRD_INIT_SPEED;
    newPipe();
    pipe_speed = PIPE_SPEED_INIT;
    pipe_speed_inc_counter = 0;
    running = true;
    lv_img_set_angle(img_bird, 0);
}
int16_t calcangle(int16_t speed)
{
    double num = speed;
    double res;
    res = num/pipe_speed;
    return (int16_t)10*(-RAD_TO_DEG * atan(res));
}
//end of 函数定义
//////////////////////////////////////////////////////////////////////////////////////////////////
//定时器函数
//负责小鸟状态刷新
//输入在loop里面
void bird_update(lv_timer_t *timer)
{
    //计算
    bird_speed += BIRD_GRAVITY;
    bird_y += bird_speed;
    pipe_obj.x += pipe_speed;
    ++score;
    current_image++;
    if (current_image == 3)
    {
        current_image = 0;
    }
    if (pipe_obj.x <= -48)
    {
        newPipe();
    }
    ++pipe_speed_inc_counter;
    if(pipe_speed_inc_counter == PIPE_SPEED_INC)
    {
        pipe_speed_inc_counter = 0;
        if(pipe_speed > PIPE_SPEED_MAX)
        {
            //--pipe_speed;
        }
    }
    //显示更新
    lv_img_set_angle(img_bird, calcangle(bird_speed));
    lv_obj_set_pos(img_bird, BIRD_X_POS - 24, bird_y - 24);
    lv_img_set_src(img_bird, birds[current_image]);
    lv_obj_set_pos(pipe_obj.img_pipe, pipe_obj.x, pipe_obj.y);
    lv_label_set_text_fmt(lblscore, "%u", score);
    if (isalive(bird_y) == false)
    {
        lv_obj_set_style_opa(img_notice, 255, 0);
        lv_img_set_src(img_notice, &game_over);
        lv_timer_pause(flappy_bird_timer);
        hal.motorAdd(MOTOR_RUN, 10);
        running = false;
    }
}
//主函数，只管输入
static void wf_flappybird_loop()
{
    if (hal.btnEnter.isPressedRaw())
    {
        lv_timer_pause(flappy_bird_timer);
        menu_create();
        menu_add("重新开始");
        menu_add("退出");
        switch (menu_show())
        {
        case 1:
            restart();
            return;
        case 2:
            lv_timer_del(flappy_bird_timer);
            popWatchFace();
            return;
        default:
            break;
        }
        lv_timer_resume(flappy_bird_timer);
    }
    if (hal.btnUp.isPressedRaw())
    {
        bird_speed = BIRD_INIT_SPEED;
        if(running == false)
        {
            restart();
        }
    }
    vTaskDelay(10);
}

void wf_flappy_bird_load()
{
    EASY_LOAD_WATCHFACE(scr_flappybird);
    running = false;
    lv_obj_fade_in(scr_flappybird, 300, 0);
    lv_obj_clear_flag(scr_flappybird, LV_OBJ_FLAG_SCROLLABLE);

    img_bird = lv_img_create(scr_flappybird);
    lv_img_set_src(img_bird, birds[0]);
    lv_obj_set_pos(img_bird, 120 - 24, 120 - 24);
    lv_img_set_pivot(img_bird, 24, 24);
    pipe_obj.img_pipe = lv_img_create(scr_flappybird);
    lv_img_set_src(pipe_obj.img_pipe, &pic_pipe);
    lv_obj_set_pos(pipe_obj.img_pipe, -100, 180);

    lblscore = label("0", 0, 0);
    lv_obj_align(lblscore, LV_ALIGN_CENTER, 0, -80);

    img_notice = lv_img_create(scr_flappybird);
    lv_img_set_src(img_notice, &tutorial);
    lv_obj_center(img_notice);

    while (hal.btnUp.isPressedRaw() == false)
        vTaskDelay(100);

    flappy_bird_timer = lv_timer_create(bird_update, 40, NULL);
    lv_timer_pause(flappy_bird_timer);
    restart();
    hal.fLoop = wf_flappybird_loop;
    RELEASELV();
}