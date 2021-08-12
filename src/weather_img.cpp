#include "A_config.h"

#define WEATHER_NAME_CNT 44
struct weathernametable
{
    uint16_t num;
    const lv_img_dsc_t *desc;
};
LV_IMG_DECLARE(img_0);
LV_IMG_DECLARE(img_1);
LV_IMG_DECLARE(img_2);
LV_IMG_DECLARE(img_3);
LV_IMG_DECLARE(img_4);
LV_IMG_DECLARE(img_5);
LV_IMG_DECLARE(img_6);
LV_IMG_DECLARE(img_7);
LV_IMG_DECLARE(img_8);
LV_IMG_DECLARE(img_9);
LV_IMG_DECLARE(img_10);
LV_IMG_DECLARE(img_11);
LV_IMG_DECLARE(img_12);
LV_IMG_DECLARE(img_13);
LV_IMG_DECLARE(img_14);
LV_IMG_DECLARE(img_15);
LV_IMG_DECLARE(img_16);
LV_IMG_DECLARE(img_17);
LV_IMG_DECLARE(img_18);
LV_IMG_DECLARE(img_19);
LV_IMG_DECLARE(img_20);
LV_IMG_DECLARE(img_21);
LV_IMG_DECLARE(img_22);
LV_IMG_DECLARE(img_23);
LV_IMG_DECLARE(img_24);
LV_IMG_DECLARE(img_25);
LV_IMG_DECLARE(img_26);
LV_IMG_DECLARE(img_27);
LV_IMG_DECLARE(img_28);
LV_IMG_DECLARE(img_29);
LV_IMG_DECLARE(img_30);
LV_IMG_DECLARE(img_31);
LV_IMG_DECLARE(img_32);
LV_IMG_DECLARE(img_39);
LV_IMG_DECLARE(img_49);
LV_IMG_DECLARE(img_53);
LV_IMG_DECLARE(img_54);
LV_IMG_DECLARE(img_55);
LV_IMG_DECLARE(img_56);
LV_IMG_DECLARE(img_57);
LV_IMG_DECLARE(img_58);
LV_IMG_DECLARE(img_99);
LV_IMG_DECLARE(img_301);
LV_IMG_DECLARE(img_302);
const struct weathernametable weather_num_to_img_table[] = {
    {0, &img_0},
    {1, &img_1},
    {2, &img_2},
    {3, &img_3},
    {4, &img_4},
    {5, &img_5},
    {6, &img_6},
    {7, &img_7},
    {8, &img_8},
    {9, &img_9},
    {10, &img_10},
    {11, &img_11},
    {12, &img_12},
    {13, &img_13},
    {14, &img_14},
    {15, &img_15},
    {16, &img_16},
    {17, &img_17},
    {18, &img_18},
    {19, &img_19},
    {20, &img_20},
    {21, &img_21},
    {22, &img_22},
    {23, &img_23},
    {24, &img_24},
    {25, &img_25},
    {26, &img_26},
    {27, &img_27},
    {28, &img_28},
    {29, &img_29},
    {30, &img_30},
    {31, &img_31},
    {32, &img_32},
    {39, &img_39},
    {49, &img_49},
    {53, &img_53},
    {54, &img_54},
    {55, &img_55},
    {56, &img_56},
    {57, &img_57},
    {58, &img_58},
    {99, &img_99},
    {301, &img_301},
    {302, &img_302},
};
const lv_img_dsc_t *Weather::weatherNumtoImage(uint16_t num)
{
    for(uint8_t i = 0; i < WEATHER_NAME_CNT; ++i)
    {
        if(weather_num_to_img_table[i].num == num)
            return weather_num_to_img_table[i].desc;
    }
    return NULL;
}