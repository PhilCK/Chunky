#include <utest.h>
#include <chunky.h>
#include "../src/chunky_ctx.h" /* peek at the state */
#include <string.h>

struct recorder_test_fixture {
        struct chunky_ctx *ctx;
};

UTEST_F_SETUP(recorder_test_fixture) {
        struct chunky_ctx *ch_ctx = 0;
        ch_ctx = chunky_create();

        ASSERT_TRUE(ch_ctx != 0);

        utest_fixture->ctx = ch_ctx;
}

UTEST_F_TEARDOWN(recorder_test_fixture) {
        struct chunky_ctx *ch_ctx = utest_fixture->ctx;

        int ok = chunky_destroy(&ch_ctx);

        ASSERT_TRUE(ok > 0);
        ASSERT_TRUE(ch_ctx == 0);
}

UTEST_F(recorder_test_fixture, lifetime) {
        ASSERT_TRUE(1);
}
