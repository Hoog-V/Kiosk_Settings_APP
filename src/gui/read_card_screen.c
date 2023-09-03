//
// Created by victor on 3-9-23.
//

#include "read_card_screen.h"

lv_obj_t* lv_read_card_screen = NULL;

LV_IMG_DECLARE(NFC_ICON)

static lv_style_t label_style;

void init_read_card_screen(void) {
    lv_read_card_screen = lv_obj_create(lv_scr_act());
    lv_coord_t scr_width = lv_obj_get_width(lv_scr_act());
    lv_coord_t scr_height = lv_obj_get_height(lv_scr_act());
    lv_obj_set_size(lv_read_card_screen, scr_width, scr_height);

    lv_obj_t * icon = lv_img_create(lv_read_card_screen);
    lv_img_set_src(icon, &NFC_ICON);
    lv_obj_align(icon, LV_ALIGN_CENTER, 0, 0);


    lv_style_init(&label_style);
    lv_style_set_text_font(&label_style, &lv_font_montserrat_48);

    lv_obj_t * label = lv_label_create(lv_read_card_screen);
    lv_obj_set_width(label, scr_width/2);
    lv_obj_add_style(label, &label_style, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);     /*Circular scroll*/
    lv_label_set_text(label, "Please check-in with your student card - Log in met uw school pas...");
    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, 0);
}
