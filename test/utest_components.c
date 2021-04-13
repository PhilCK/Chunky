#include <utest.h>
#include <chunky.h>
#include "../src/chunky_ctx.h" /* peek at the state */
#include <string.h>

struct component_test_fixture {
        struct chunky_ctx *ctx;
};

static int
is_pow2(uint64_t i) {
        return (i & (i - 1)) == 0;
}

UTEST_F_SETUP(component_test_fixture) {
        struct chunky_ctx *ch_ctx = 0;
        ch_ctx = chunky_create();

        ASSERT_TRUE(ch_ctx != 0);

        utest_fixture->ctx = ch_ctx;
}

UTEST_F_TEARDOWN(component_test_fixture) {
        struct chunky_ctx *ch_ctx = utest_fixture->ctx;

        int ok = chunky_destroy(&ch_ctx);

        ASSERT_TRUE(ok > 0);
        ASSERT_TRUE(ch_ctx == 0);
}

UTEST_F(component_test_fixture, single_reg) {
        ASSERT_TRUE(chunky_components_count(utest_fixture->ctx) == 0);

        struct chunky_component_desc desc = {
                .name = "transform",
                .bytes = sizeof(float) * 10
        };

        uint64_t id = 0;

        int ok = chunky_components_create(
                utest_fixture->ctx,
                &desc,
                1,
                &id);

        ASSERT_TRUE(ok > 0);
        ASSERT_TRUE(id > 0);
        ASSERT_TRUE(is_pow2(id));
        ASSERT_TRUE(chunky_components_count(utest_fixture->ctx) == 1);

        /* Check internal state is correct.
         */

        ASSERT_TRUE(utest_fixture->ctx->comps[0].bytes == desc.bytes);
        ASSERT_TRUE(strcmp(utest_fixture->ctx->comps[0].name, desc.name) == 0);
}

UTEST_F(component_test_fixture, multi_reg) {

        #define COMP_COUNT 3

        ASSERT_TRUE(chunky_components_count(utest_fixture->ctx) == 0);

        struct chunky_component_desc desc[COMP_COUNT] = {
                {
                        .name = "transform",
                        .bytes = sizeof(float) * 10
                },
                {
                        .name = "mesh",
                        .bytes = sizeof(float) * 3
                },
                {
                        .name = "bounds",
                        .bytes = sizeof(float) * 4
                },
        };

        uint64_t ids[COMP_COUNT] = {0};

        int ok = chunky_components_create(
                utest_fixture->ctx,
                desc,
                COMP_COUNT,
                ids);

        ASSERT_TRUE(ok > 0);
        ASSERT_TRUE(chunky_components_count(utest_fixture->ctx) == COMP_COUNT);

        for(int i = 0; i < COMP_COUNT; ++i) {
                ASSERT_TRUE(ids[i] == 1ULL << i);

                /* Check internal state is correct
                 */

                struct chunky_component_desc *d = &desc[i];
                struct chunky_component *c = &utest_fixture->ctx->comps[i];

                ASSERT_TRUE(strcmp(c->name, d->name) == 0);
                ASSERT_TRUE(c->bytes == d->bytes);
        }

        #undef COMP_COUNT
}

UTEST_F(component_test_fixture, max_reg) {

        #define COMP_COUNT 64

        ASSERT_TRUE(chunky_components_count(utest_fixture->ctx) == 0);

        struct chunky_component_desc desc[COMP_COUNT] = {0};

        /* create some junky data
         */

        const char *name = "max_reg";

        for(int i = 0; i < COMP_COUNT; ++i) {
                desc[i].bytes = i + 1;
                desc[i].name = name;
        }

        uint64_t ids[COMP_COUNT] = {0};

        int ok = chunky_components_create(
                utest_fixture->ctx,
                desc,
                COMP_COUNT,
                ids);

        ASSERT_TRUE(ok > 0);
        ASSERT_TRUE(chunky_components_count(utest_fixture->ctx) == COMP_COUNT);

        for(int i = 0; i < COMP_COUNT; ++i) {
                ASSERT_TRUE(ids[i] == 1ULL << i);
        }

        #undef COMP_COUNT
}
