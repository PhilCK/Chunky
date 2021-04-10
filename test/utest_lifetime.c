#include <utest.h>
#include <chunky.h>

UTEST(lifetime, simple) {
        struct chunky_ctx *ch_ctx = 0;
        ch_ctx = chunky_create();

        ASSERT_TRUE(ch_ctx != 0);

        int ok = chunky_destroy(&ch_ctx);

        ASSERT_TRUE(ok > 0);
        ASSERT_TRUE(ch_ctx == 0);
}
