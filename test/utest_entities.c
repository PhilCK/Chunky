#include <utest.h>
#include <chunky.h>
#include <string.h>

struct entity_test_fixture {
        struct chunky_ctx *ctx;
};

UTEST_F_SETUP(entity_test_fixture) {

        /* Create a chunky ctx
         */

        struct chunky_ctx *ch_ctx = 0;
        ch_ctx = chunky_create();

        ASSERT_TRUE(ch_ctx != 0);
        utest_fixture->ctx = ch_ctx;
}

UTEST_F_TEARDOWN(entity_test_fixture) {
        struct chunky_ctx *ch_ctx = utest_fixture->ctx;

        int ok = chunky_destroy(&ch_ctx);

        ASSERT_TRUE(ok > 0);
        ASSERT_TRUE(ch_ctx == 0);
}

UTEST_F(entity_test_fixture, create_destroy_entity) {

        uintptr_t ent = chunky_entity_create(utest_fixture->ctx, 0);

        ASSERT_TRUE(ent > 0);

        int ok = chunky_entity_destroy(utest_fixture->ctx, ent);

        ASSERT_TRUE(ok > 0);
}
