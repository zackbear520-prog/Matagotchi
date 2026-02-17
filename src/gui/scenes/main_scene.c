#include <furi.h>
#include "../custom_modules/button_panel.h"
#include <gui/view_dispatcher.h>

#include "main_scene.h"
#include "scenes.h"
#include "matagotchi_icons.h"
#include "../game_decoder.h"
#include "../../flipper_structs.h"
#include "../../constants.h"

enum ButtonIndex {
    settings_button,
    info_button,
    candy_button,
    pill_button,
    train_button
};

static void main_button_pressed_callback(void *ctx, uint32_t index) {
    struct ApplicationContext *context = (struct ApplicationContext *)ctx;
    FURI_LOG_T(LOG_TAG, "main_button_pressed_callback with index %lu", index);
    view_dispatcher_send_custom_event(context->view_dispatcher, index);
}

static void scene_main_refresh_view(ButtonPanel *button_panel, struct ApplicationContext *context) {
    uint16_t x, y;
    button_panel_get_selection(button_panel, &x, &y);
    button_panel_reset(button_panel);

    button_panel_reserve(button_panel, 2, 3);
    button_panel_add_icon(button_panel, 0, 2, decode_image(context->game_state));
    button_panel_add_item(button_panel,
                          settings_button,
                          0, 0,
                          70, 6,
                          &I_settings_icon_20x20,
                          &I_settings_icon_hover_20x20,
                          main_button_pressed_callback,
                          context);
    button_panel_add_item(button_panel,
                          info_button,
                          1, 0,
                          102, 6,
                          &I_info_icon_20x20,
                          &I_info_icon_hover_20x20,
                          main_button_pressed_callback,
                          context);
    button_panel_add_item(button_panel,
                          candy_button,
                          0, 1,
                          70, 38,
                          &I_candy_icon_20x20,
                          &I_candy_icon_hover_20x20,
                          main_button_pressed_callback,
                          context);
    button_panel_add_item(button_panel,
                          pill_button,
                          1, 1,
                          102, 38,
                          &I_pill_icon_20x20,
                          &I_pill_icon_hover_20x20,
                          main_button_pressed_callback,
                          context);
    button_panel_add_item(button_panel,
                          train_button,
                          1, 2,
                          102, 54,
                          &I_candy_icon_20x20,
                          &I_candy_icon_hover_20x20,
                          main_button_pressed_callback,
                          context);

    if (x > 1) x = 1;
    if (y > 2) y = 2;
    button_panel_set_selection(button_panel, x, y);
}

void scene_main_on_enter(void *ctx) {
    struct ApplicationContext *context = (struct ApplicationContext *)ctx;
    FURI_LOG_T(LOG_TAG, "scene_main_on_enter");

    scene_main_refresh_view(context->button_module, context);

    view_dispatcher_switch_to_view(context->view_dispatcher, scene_main);
}

bool scene_main_on_event(void *ctx, SceneManagerEvent event) {
    struct ApplicationContext *context = (struct ApplicationContext *)ctx;

    switch(event.type) {
        case SceneManagerEventTypeBack:
            FURI_LOG_T(LOG_TAG, "scene_main_on_event: pressed back button");
            scene_manager_stop(context->scene_manager);
            view_dispatcher_stop(context->view_dispatcher);
            return true;
        case SceneManagerEventTypeTick:
            FURI_LOG_T(LOG_TAG, "scene_main_on_event: received tick");
            scene_main_refresh_view(context->button_module, context);
            return true;
        case SceneManagerEventTypeCustom:
            FURI_LOG_T(LOG_TAG, "scene_main_on_event: received button press %lu", event.event);
            uint32_t index = event.event;

            if (index == settings_button) {
                scene_manager_next_scene(context->scene_manager, scene_settings);
            } else if (index == info_button) {
                scene_manager_next_scene(context->scene_manager, scene_status);
            } else if (index == candy_button) {
                struct ThreadsMessage threads_message = {.type = PROCESS_CANDY};
                furi_message_queue_put(context->threads_message_queue, &threads_message, FuriWaitForever);
            } else if (index == pill_button) {
                struct ThreadsMessage threads_message = {.type = PROCESS_PILL};
                furi_message_queue_put(context->threads_message_queue, &threads_message, FuriWaitForever);
            } else if (index == train_button) {
                struct ThreadsMessage threads_message = {.type = PROCESS_TRAIN};
                furi_message_queue_put(context->threads_message_queue, &threads_message, FuriWaitForever);
            }
            return true;
        default:
            FURI_LOG_T(LOG_TAG, "scene_main_on_event: unhandled event");
            break;
    }
    return false;
}

void scene_main_on_exit(void *ctx) {
    struct ApplicationContext *context = (struct ApplicationContext *)ctx;
    FURI_LOG_T(LOG_TAG, "scene_main_on_exit");
    button_panel_reset(context->button_module);
}
