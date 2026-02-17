#include <furi.h>
#include <gui/modules/popup.h>
#include <gui/view_dispatcher.h>

#include "train_scene.h"
#include "scenes.h"
#include "../game_decoder.h"
#include "../../constants.h"
#include "../../flipper_structs.h"

static uint32_t train_ticks_count;

static void scene_train_refresh_view(Popup *popup, struct ApplicationContext *context) {
    popup_reset(popup);
    popup_set_icon(popup,
                   0, 2,
                   decode_image(context->game_state));
    popup_set_header(popup,
                     "Training",
                     68, 5,
                     AlignLeft, AlignTop);
    popup_set_text(popup,
                   "ByteBuddy\nis training!",
                   70, 20,
                   AlignLeft, AlignTop);
}

void scene_train_on_enter(void *ctx) {
    struct ApplicationContext *context = (struct ApplicationContext *)ctx;
    Popup *pp = context->popup_module;
    FURI_LOG_T(LOG_TAG, "scene_train_on_enter");

    scene_train_refresh_view(pp, context);

    train_ticks_count = 0;

    view_dispatcher_switch_to_view(context->view_dispatcher, scene_train);
}

bool scene_train_on_event(void *ctx, SceneManagerEvent event) {
    struct ApplicationContext *context = (struct ApplicationContext *)ctx;

    switch(event.type) {
        case SceneManagerEventTypeTick:
            FURI_LOG_T(LOG_TAG, "scene_train_on_event: received tick");
            if (train_ticks_count >= ANIMATION_TICKS_DURATION) {
                scene_manager_previous_scene(context->scene_manager);
            } else {
                train_ticks_count++;
                scene_train_refresh_view(context->popup_module, context);
            }
            return true;
        case SceneManagerEventTypeBack:
            FURI_LOG_T(LOG_TAG, "scene_train_on_event: pressed back button");
            return true;
        default:
            FURI_LOG_T(LOG_TAG, "scene_train_on_event: unhandled event");
            break;
    }
    return false;
}

void scene_train_on_exit(void *ctx) {
    struct ApplicationContext *context = (struct ApplicationContext *)ctx;
    FURI_LOG_T(LOG_TAG, "scene_train_on_exit");
    popup_reset(context->popup_module);
}
