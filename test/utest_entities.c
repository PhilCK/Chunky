#include <utest.h>
#include <chunky.h>
#include <string.h>
#include "../src/chunky_ctx.h"

struct entity {
        struct chunky_ctx *ctx;
};

UTEST_F_SETUP(entity) {

        /* Create a chunky ctx
         */

        struct chunky_ctx *ch_ctx = 0;
        ch_ctx = chunky_create();

        ASSERT_TRUE(ch_ctx != 0);
        utest_fixture->ctx = ch_ctx;
}

UTEST_F_TEARDOWN(entity) {
        struct chunky_ctx *ch_ctx = utest_fixture->ctx;

        int ok = chunky_destroy(&ch_ctx);

        ASSERT_TRUE(ok > 0);
        ASSERT_TRUE(ch_ctx == 0);
}

UTEST_F(entity, create_destroy_entity) {

        /* Simple create an entity then destroy it.
         */

        uintptr_t ent = chunky_entity_create(utest_fixture->ctx, 0);
        ASSERT_TRUE(ent > 0);

        int ok = chunky_entity_destroy(utest_fixture->ctx, ent);
        ASSERT_TRUE(ok > 0);
}

UTEST_F(entity, create_alot_of_entities) {

        /* Feel free to check this by increasing to check it explods
         * (CHUNKY_MAX_ENTITIES + 1)
         */

        for(int i = 0; i < CHUNKY_MAX_ENTITIES; ++i) {
                (void)chunky_entity_create(utest_fixture->ctx, 0);
        }

        /* Basically we are just checking it doesn't blow up
         */

        ASSERT_TRUE(1);
}

