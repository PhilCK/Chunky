#include <ubench.h>
#include <chunky.h>

struct components {
        struct chunky_ctx *ctx;
};

UBENCH_F_SETUP(components) {
        struct chunky_ctx *ch_ctx = 0;
        ch_ctx = chunky_create();
        ubench_fixture->ctx = ch_ctx; 
}

UBENCH_F_TEARDOWN(components) { 
        struct chunky_ctx *ch_ctx = ubench_fixture->ctx;

        (void)chunky_destroy(&ch_ctx);
}

// UBENCH_F(components, create) {
//         struct chunky_component_desc desc = {
//                 .name = "transform",
//                 .bytes = sizeof(float) * 10
//         };

//         uint64_t id = 0;

//         int ok = chunky_components_create(
//                 ubench_fixture->ctx,
//                 &desc,
//                 1,
//                 &id);
// }
