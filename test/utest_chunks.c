#include <utest.h>
#include <chunky.h>
#include "../src/chunky_ctx.h" /* peek at the state */
#include <string.h>

struct chunk_test_fixture {
        struct chunky_ctx *ctx;

        uint64_t tform_compid;
        uint64_t bounds_compid;
        uint64_t renderable_compid;
};

UTEST_F_SETUP(chunk_test_fixture) {

        /* Create a chunky ctx.
         */

        struct chunky_ctx *ch_ctx = 0;
        ch_ctx = chunky_create();

        ASSERT_TRUE(ch_ctx != 0);
        utest_fixture->ctx = ch_ctx;

        /* Register some basic components.
         */

        struct chunky_component_desc desc[3] = {
                {
                        .name = "transform",
                        .bytes = sizeof(float) * 10
                },
                {
                        .name = "bounds",
                        .bytes = sizeof(float) * 6
                },
                {
                        .name = "renderable",
                        .bytes = sizeof(uintptr_t) * 2
                }
        };

        uint64_t components[3] = {0};

        int ok = chunky_components_create(
                utest_fixture->ctx,
                desc,
                3,
                components);

        utest_fixture->tform_compid = components[0];
        utest_fixture->bounds_compid = components[1];
        utest_fixture->renderable_compid = components[2];

        ASSERT_TRUE(ok > 0);
}

UTEST_F_TEARDOWN(chunk_test_fixture) {
        /* Pull down the Chunky context
         */

        struct chunky_ctx *ch_ctx = utest_fixture->ctx;

        int ok = chunky_destroy(&ch_ctx);

        ASSERT_TRUE(ok > 0);
        ASSERT_TRUE(ch_ctx == 0);
}

UTEST_F(chunk_test_fixture, get_empty_chunks) {
        size_t count = 0;

        /* Create an entity this should not appear in a chunk
         */
        
        uintptr_t ent = chunky_entity_create(utest_fixture->ctx, 0);

        /* Find chunks with these components
         * We should get nothing, because the above entity has no components.
         */

        int ok = chunky_find_chunks(
                utest_fixture->ctx,
                utest_fixture->tform_compid | utest_fixture->bounds_compid,
                NULL,
                &count);

        ASSERT_TRUE(ok > 0);
        ASSERT_TRUE(count == 0);

        /* Pass an array for chunk headers
         * This is just to ensure this works, it should contain nothing.
         */

        struct chunky_chunk_header *headers[3] = {0};

        ok = chunky_find_chunks(
                utest_fixture->ctx,
                utest_fixture->tform_compid | utest_fixture->bounds_compid,
                headers,
                &count);

        ASSERT_TRUE(ok > 0);
        ASSERT_TRUE(count == 0);
}

UTEST_F(chunk_test_fixture, get_chunk) {
        size_t count = 0;

        /* Create an entity
         * With a some components, we should be able to find the chunk of this
         * entity, by matching the components.
         */

        uintptr_t ent = chunky_entity_create(
                utest_fixture->ctx, 
                utest_fixture->tform_compid | utest_fixture->bounds_compid);

        int ok = chunky_find_chunks(
                utest_fixture->ctx,
                utest_fixture->tform_compid | utest_fixture->bounds_compid,
                NULL,
                &count);

        ASSERT_TRUE(ok > 0);
        ASSERT_TRUE(count == 1);

        /* We should be able to get the chunk header that this array landed in.
         */

        struct chunky_chunk_header *headers[3] = {0};

        ok = chunky_find_chunks(
                utest_fixture->ctx,
                utest_fixture->tform_compid | utest_fixture->bounds_compid,
                headers,
                &count);

        ASSERT_TRUE(ok > 0);
        ASSERT_TRUE(count == 1);
}

UTEST_F(chunk_test_fixture, get_chunk_with_subset) {
        size_t count = 0;

        /* Create an entity
         * With a some components, we should be able to find the chunk of this
         * entity with sub components, eg a subset of the components the entity
         * actually has.
         */

        uintptr_t ent = chunky_entity_create(
                utest_fixture->ctx, 
                utest_fixture->tform_compid | utest_fixture->bounds_compid | utest_fixture->renderable_compid);

        int ok = chunky_find_chunks(
                utest_fixture->ctx,
                utest_fixture->tform_compid | utest_fixture->bounds_compid,
                NULL,
                &count);

        ASSERT_TRUE(ok > 0);
        ASSERT_TRUE(count == 1);

        /* We should be able to get the chunk header that this array landed in.
         */

        struct chunky_chunk_header *headers[3] = {0};

        ok = chunky_find_chunks(
                utest_fixture->ctx,
                utest_fixture->tform_compid | utest_fixture->bounds_compid,
                headers,
                &count);

        ASSERT_TRUE(ok > 0);
        ASSERT_TRUE(count == 1);
}

UTEST_F(chunk_test_fixture, get_no_chunk_from_wrong_types) {
        size_t count = 0;

        /* Create an entity
         * With a some components, we shouldn't find this entity when searching
         * for chunks with different components.
         */

        uintptr_t ent = chunky_entity_create(
                utest_fixture->ctx, 
                utest_fixture->tform_compid | utest_fixture->bounds_compid);

        int ok = chunky_find_chunks(
                utest_fixture->ctx,
                utest_fixture->renderable_compid,
                NULL,
                &count);

        ASSERT_TRUE(ok > 0);
        ASSERT_TRUE(count == 0);
}
