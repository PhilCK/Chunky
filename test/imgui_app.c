//------------------------------------------------------------------------------
//  Simple C99 cimgui+sokol starter project for Win32, Linux and macOS.
//------------------------------------------------------------------------------
#include "chunky.h"
#include "chunky_ctx.h"
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_time.h"
#include "sokol_glue.h"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#include "sokol_imgui.h"
#include <string.h>

/* -------------------------------------------------------------------------- */
/* Test App Data and Types 
 */

struct tform {
        float position[3];
        float scale[3];
        float rotation[3];
};

struct bounds {
        float scale[3];
};

struct renderable {
        int mesh;
        int index;
        int count;
        int material;

        int some_big_padding[128]; /* This is to reduce the capacity in a chunk */
};

enum {
        COMP_TFORM,
        COMP_BOUNDS,
        COMP_RDRABLE,

        COMP_COUNT,
};

uint64_t type_ids[COMP_COUNT];

struct chunky_ctx *ch_ctx = 0;
uintptr_t entities[8] = {0};
int ent_count = 0;
int show_ch = -1;
uintptr_t show_ent = 0;

/* -------------------------------------------------------------------------- */

static struct {
    uint64_t laptime;
    sg_pass_action pass_action;

    /* Chunky */
} state;

static void init(void) {
    sg_setup(&(sg_desc){
        .context = sapp_sgcontext()
    });
    stm_setup();
    simgui_setup(&(simgui_desc_t){ 0 });

    /*
    // initial clear color
    state.pass_action = (sg_pass_action) {
        .colors[0] = { .action = SG_ACTION_CLEAR, .value = { 0.0f, 0.5f, 1.0f, 1.0 } }
    };
    */

    ch_ctx = chunky_create();

    struct chunky_component_desc desc[COMP_COUNT] = {
        [0] = {
            .name = "tform",
            .bytes = sizeof(struct tform),
        },
        [1] = {
            .name = "bounds",
            .bytes = sizeof(struct bounds),
        },
        [2] = {
            .name = "renderable",
            .bytes = sizeof(struct renderable),
        },
    };

    int ok = chunky_components_create(
        ch_ctx,
        desc,
        COMP_COUNT,
        type_ids);
}


/* -------------------------------------------------------------------------- */
/* Test App Frame 
 */

void igTransform(const char *name, struct tform *t) {
        if(t && igCollapsingHeaderTreeNodeFlags(name, 0)) {
                igDragFloat3("Position", t->position, 0.1f, 0.1f, 0.1f, 0, 0);
                igDragFloat3("Scale", t->scale, 0.1f, 0.1f, 0.1f, 0, 0);
                igDragFloat3("Rotation", t->rotation, 0.1f, 0.1f, 0.1f, 0, 0);
        }
}

void igBounds(const char *name, struct bounds *b) {
        if(b && igCollapsingHeaderTreeNodeFlags(name, 0)) {
                igDragFloat3("Scale", b->scale, 0.1f, 0.1f, 0.1f, 0, 0); 
        }
}

void igRenderable(const char *name, struct renderable *r) {
        if(r && igCollapsingHeaderTreeNodeFlags(name, 0)) {
                igDragInt("Mesh", &r->mesh, 0.1f, 0, 1000, 0, 0);
                igDragInt("Index", &r->index, 0.1f, 0, 1000, 0, 0);
                igDragInt("Count", &r->count, 0.1f, 0, 1000, 0, 0);
                igDragInt("Material", &r->material, 0.1f, 0, 1000, 0, 0);
        }
}

static void frame(void) {
        const int width = sapp_width();
        const int height = sapp_height();
        const double delta_time = stm_sec(
                stm_round_to_common_refresh_rate(stm_laptime(&state.laptime))
        );

        simgui_new_frame(width, height, delta_time);

        int column_width = width / 4;

        /*=== UI CODE STARTS HERE ===*/

        ImGuiWindowFlags flags =
                ImGuiWindowFlags_NoResize | 
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoCollapse;

        /* Entity List
         * Show a list of entities where the user can create and select and
         * entity.
         */

        igSetNextWindowPos((ImVec2){0,0}, ImGuiCond_Always, (ImVec2){0,0});
        igSetNextWindowSize((ImVec2){column_width, height}, ImGuiCond_Always);

        igBegin("Entity List", 0, flags); 

        if(ent_count < 8) {
                static uint64_t types = 0; 

                bool enable_tform = types & type_ids[COMP_TFORM];
                if(igCheckbox("Transform", &enable_tform)) {
                        types ^= type_ids[COMP_TFORM];
                }

                bool enable_bounds = types & type_ids[COMP_BOUNDS];
                if(igCheckbox("Bounds", &enable_bounds)) {
                        types ^= type_ids[COMP_BOUNDS];
                }

                bool enable_rdr = types & type_ids[COMP_RDRABLE];
                if(igCheckbox("Renderable", &enable_rdr)) {
                        types ^= type_ids[COMP_RDRABLE];
                }

                if(igButton("Create", (ImVec2){100, 25})) {
                        uintptr_t ent = chunky_entity_create(ch_ctx, types);
                        entities[ent_count] = ent;
                        ent_count += 1;
                }
        } else {
                igText("Maxed Out Ents"); 
        }

        igSeparator();

        for(int i = 0; i < ent_count; ++i) {
                igText("Ent %ld", entities[i]);

                char button[32] = {0};
                snprintf(button, sizeof(button), "Show##%d", i);

                if(igButton(button, (ImVec2){100, 25})) {
                        show_ent = entities[i];
                }

                igSeparator();
        }

        igEnd();

        /* Entity Details */
        /* List some items about the entity
         * and allow its components to be edited
         */

        igSetNextWindowPos((ImVec2){column_width * 1,0}, ImGuiCond_Always, (ImVec2){0,0});
        igSetNextWindowSize((ImVec2){column_width, height}, ImGuiCond_Always);

        igBegin("Entity Details", 0, flags);
        if(show_ent) {

                struct chunky_entity *ent = (struct chunky_entity*)show_ent;

                igText("Entity %ld", show_ent);
                igSeparator();

                uintptr_t tform_d = chunky_entity_component_get(
                        ch_ctx,
                        show_ent,
                        type_ids[COMP_TFORM]);

                igTransform("Transform", (struct tform*)tform_d);

                uintptr_t bounds_d = chunky_entity_component_get(
                        ch_ctx,
                        show_ent,
                        type_ids[COMP_BOUNDS]);

                igBounds("Bounds", (struct bounds*)bounds_d);

                uintptr_t rdr_d = chunky_entity_component_get(
                        ch_ctx,
                        show_ent,
                        type_ids[COMP_RDRABLE]);

                igRenderable("Renderable", (struct renderable*)rdr_d);
        } else {
                igText("Select An Entity");
        }

        igEnd();

        /* Block List
         * Look at the blocks and give some info on them.
         */

        igSetNextWindowPos((ImVec2){column_width * 2,0}, ImGuiCond_Always, (ImVec2){0,0});
        igSetNextWindowSize((ImVec2){column_width, height}, ImGuiCond_Always);

        igBegin("Block List", 0, flags); 

        for(int i = 0; i < CHUNKY_MAX_BLOCKS; ++i) {
        
                int child = show_ch;

                igText("Block %d", i);
                bool on = false;

                on = ch_ctx->info[i].layout & type_ids[COMP_TFORM];
                igCheckbox("Transform", &on);

                on = ch_ctx->info[i].layout & type_ids[COMP_BOUNDS];
                igCheckbox("Bounds", &on);

                on = ch_ctx->info[i].layout & type_ids[COMP_RDRABLE];
                igCheckbox("Renderable", &on);

                int count = ch_ctx->block[i].header.count;
                igDragInt("Count", &count, 0, 0, 0, 0, 0); 

                int cap = ch_ctx->block[i].header.capacity;
                igDragInt("Capacity", &cap, 0, 0, 0, 0, 0); 

                char button[32] = {0};
                snprintf(button, sizeof(button), "Show##%d", i);

                if(igButton(button, (ImVec2){100, 25})) {
                        show_ch = i;
                }

                igSeparator();
        }

        igEnd();

        /* Show A Block 
         * The details of the chunk, the data inside it etc.
         */

        igSetNextWindowPos((ImVec2){column_width * 3,0}, ImGuiCond_Always, (ImVec2){0,0});
        igSetNextWindowSize((ImVec2){column_width, height}, ImGuiCond_Always);

        igBegin("Block Details", 0, flags); 
    
        if(show_ch >= 0 && show_ch < CHUNKY_MAX_BLOCKS) {
                int i = show_ch;

                igText("Block %d", show_ch);

                int count = ch_ctx->block[i].header.count;
                igDragInt("Count", &count, 0, 0, 0, 0, 0); 

                int cap = ch_ctx->block[i].header.capacity;
                igDragInt("Capacity", &cap, 0, 0, 0, 0, 0);

                igSeparator();

                static int data_idx = 0;

                char items[] = "Entities\0Transforms\0Bounds\0Renderable\0";

                igComboStr("Data", &data_idx, items, 10);

                uintptr_t *entities = 0;
                entities = (uintptr_t*)ch_ctx->block[i].header.entities;

                uintptr_t *comps = 0;
                comps = ch_ctx->block[i].header.components;

                int tform_idx = id2idx(type_ids[COMP_TFORM]);
                struct tform *tforms = 0;
                tforms = (struct tform*)comps[tform_idx];

                int bounds_idx = id2idx(type_ids[COMP_BOUNDS]);
                struct bounds *bounds = 0;
                bounds = (struct bounds*)comps[bounds_idx];

                int rdr_idx = id2idx(type_ids[COMP_RDRABLE]);
                struct renderable *rdrs = 0;
                rdrs = (struct renderable*)comps[rdr_idx];

                if(data_idx ==0) {
                        for(int j = 0; j < ch_ctx->block[i].header.count; ++j) {
                                char name[32] = {0};
                                snprintf(name, sizeof(name), "%d. %lu", j, entities[j]);
                                igButton(name, (ImVec2){300, 20});
                        }
                } else if(data_idx == 1) {
                        for(int j = 0; j < ch_ctx->block[i].header.count; ++j) {
                                char name[32] = {0};
                                snprintf(name, sizeof(name), "%d. %lu", j, entities[j]);
                                igTransform(name, &tforms[j]);
                        }
                } else if(data_idx == 2) {
                        for(int j = 0; j < ch_ctx->block[i].header.count; ++j) {
                                char name[32] = {0};
                                snprintf(name, sizeof(name), "%d. %lu", j, entities[j]);
                                igBounds(name, &bounds[j]);
                        }
                } else if(data_idx == 3) {
                        for(int j = 0; j < ch_ctx->block[i].header.count; ++j) {
                                char name[32] = {0};
                                snprintf(name, sizeof(name), "%d. %lu", j, entities[j]);
                                igRenderable(name, &rdrs[j]);
                        }
                } else {
                        igText("Noooo - something went wrong");
                }
        } else {
                igText("Select a Block from list");
        }

        igEnd();

        /*=== UI CODE ENDS HERE ===*/

        sg_begin_default_pass(&state.pass_action, width, height);
        simgui_render();
        sg_end_pass();
        sg_commit();
}

static void cleanup(void) {
    simgui_shutdown();
    sg_shutdown();
}

static void event(const sapp_event* ev) {
    simgui_handle_event(ev);
}

sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = event,
        .window_title = "Block Viewer",
        .width = 1200,
        .height = 600,
    };
}
