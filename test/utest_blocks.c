#include <utest.h>
#include <chunky.h>
#include "../src/chunky_ctx.h" /* peek at the state */
#include <string.h>

struct tform_comp {
        uint32_t data[10];
};

struct bounds_comp {
        uint32_t data[6];
};

struct renderable_comp {
        uint32_t data[2];
};

struct block {
        struct chunky_ctx *ctx;

        uint64_t tform_compid;
        uint64_t bounds_compid;
        uint64_t renderable_compid;
};

UTEST_F_SETUP(block) {

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
                        .bytes = sizeof(struct tform_comp)
                },
                {
                        .name = "bounds",
                        .bytes = sizeof(struct bounds_comp)
                },
                {
                        .name = "renderable",
                        .bytes = sizeof(struct renderable_comp)
                }
        };

        uint64_t components[3] = {0};

        int ok = chunky_components_create(
                utest_fixture->ctx,
                desc,
                3,
                components);

        ASSERT_TRUE(components[0] > 0);
        utest_fixture->tform_compid = components[0];

        ASSERT_TRUE(components[1] > 0);
        utest_fixture->bounds_compid = components[1];

        ASSERT_TRUE(components[2] > 0);
        utest_fixture->renderable_compid = components[2];

        ASSERT_TRUE(ok > 0);
}

UTEST_F_TEARDOWN(block) {
        /* Pull down the Chunky context
         */

        struct chunky_ctx *ch_ctx = utest_fixture->ctx;

        int ok = chunky_destroy(&ch_ctx);

        ASSERT_TRUE(ok > 0);
        ASSERT_TRUE(ch_ctx == 0);
}

UTEST_F(block, access_component_data) {
        size_t count = 0;

        /* Create an entity
         * With a some components, we should be able to find the chunk of this
         * entity, by matching the components.
         */

        uintptr_t ent = chunky_entity_create(
                utest_fixture->ctx, 
                utest_fixture->tform_compid | utest_fixture->bounds_compid);

        int ok = chunky_find_blocks(
                utest_fixture->ctx,
                utest_fixture->tform_compid | utest_fixture->bounds_compid,
                NULL,
                &count);

        /* We should be able to get the chunk header that this array landed in.
         */

        ASSERT_TRUE(count == 1);

        struct chunky_block_header *headers[1] = {0};

        ok = chunky_find_blocks(
                utest_fixture->ctx,
                utest_fixture->tform_compid | utest_fixture->bounds_compid,
                headers,
                &count);

        /* Now we have the chunk, we should be able to access and write to a
         * components data.
         */

        for(int i = 0; i < count; ++i) {
                void *tform = NULL;
                tform = chunky_block_data(
                    utest_fixture->ctx,
                    headers[0],
                    utest_fixture->tform_compid);

                ASSERT_TRUE(tform != NULL);
        };
}

UTEST_F(block, access_entity_data_full) {
        size_t count = 0;

        #define ENT_COUNT 32

        uintptr_t ents[ENT_COUNT];

        /* Create an entity
         * With a some components, we should be able to find the chunk of this
         * entity, by matching the components.
         */

        for(int i = 0; i < ENT_COUNT; ++i) {
                ents[i] = chunky_entity_create(
                        utest_fixture->ctx, 
                        utest_fixture->tform_compid | utest_fixture->bounds_compid);
        }

        int ok = chunky_find_blocks(
                utest_fixture->ctx,
                utest_fixture->tform_compid | utest_fixture->bounds_compid,
                NULL,
                &count);

        /* We should be able to get the chunk header that this data landed in.
         * Test is only geared towards landing in a single block.
         */

        ASSERT_TRUE(count == 1);

        struct chunky_block_header *headers[1] = {0};

        ok = chunky_find_blocks(
                utest_fixture->ctx,
                utest_fixture->tform_compid | utest_fixture->bounds_compid,
                headers,
                &count);

        /* Now we have the chunk, we should be able to access the entities
         * We compare the entities we get back from the block, with the ones
         * we got when we created the entity.
         */

        uintptr_t * entities = NULL;
        entities = chunky_block_entities(
                utest_fixture->ctx,
                headers[0]);

        for(int i = 0; i < ENT_COUNT; ++i) {
                ASSERT_TRUE(entities[i] == ents[i]);
        }

        #undef ENT_COUNT
}

UTEST_F(block, mem_corruption_test) {
        size_t count = 0;

        #define ENT_COUNT 32

        uintptr_t ents[ENT_COUNT];

        /* Create an entity
         * With a some components, we should be able to find the chunk of this
         * entity, by matching the components.
         */

        for(int i = 0; i < ENT_COUNT; ++i) {
                ents[i] = chunky_entity_create(
                        utest_fixture->ctx, 
                        utest_fixture->tform_compid | utest_fixture->bounds_compid);
        }

        int ok = chunky_find_blocks(
                utest_fixture->ctx,
                utest_fixture->tform_compid | utest_fixture->bounds_compid,
                NULL,
                &count);

        /* We should be able to get the chunk header that this data landed in.
         * Test is only geared towards landing in a single block.
         */

        ASSERT_TRUE(count == 1);

        struct chunky_block_header *headers[1] = {0};

        ok = chunky_find_blocks(
                utest_fixture->ctx,
                utest_fixture->tform_compid | utest_fixture->bounds_compid,
                headers,
                &count);

        /* We are going to insert a bunch of data into each entity's components
         * Then we are going to check to see if the data is still correct.
         */

        struct tform_comp *tform = chunky_block_data(
                utest_fixture->ctx,
                headers[0],
                utest_fixture->tform_compid);
        ASSERT_TRUE(tform != NULL);

        struct bounds_comp *bounds = chunky_block_data(
                utest_fixture->ctx,
                headers[0],utest_fixture->bounds_compid);
        ASSERT_TRUE(bounds != NULL);

        // struct renderable_comp *rdr = chunky_block_data(
        //         utest_fixture->ctx,
        //         headers[0],utest_fixture->renderable_compid);
        // ASSERT_TRUE(rdr != NULL);

        for(int i = 0; i < headers[0]->count; ++i) {
                struct tform_comp tc = {
                        .data = {1,1,1,1,1,1,1,1,1,1}
                };
                tform[i] = tc;

                struct bounds_comp bc = {
                        .data = {2,2,2,2,2,2}
                };
                bounds[i] = bc;

                // struct renderable_comp rc = {
                //         .data = {3,3}
                // };
                // rdr[i] = rc;
        }

        /* Now we have filled the data we should be able to check to see if the
         * various arrays are still intact.
         */

        uintptr_t * entities = NULL;
        entities = chunky_block_entities(
                utest_fixture->ctx,
                headers[0]);

        for(int i = 0; i < ENT_COUNT; ++i) {
                ASSERT_TRUE(entities[i] == ents[i]);

                ASSERT_TRUE(tform[i].data[0] == 1);
                ASSERT_TRUE(tform[i].data[1] == 1);
                ASSERT_TRUE(tform[i].data[2] == 1);
                ASSERT_TRUE(tform[i].data[3] == 1);
                ASSERT_TRUE(tform[i].data[4] == 1);
                ASSERT_TRUE(tform[i].data[5] == 1);
                ASSERT_TRUE(tform[i].data[6] == 1);
                ASSERT_TRUE(tform[i].data[7] == 1);
                ASSERT_TRUE(tform[i].data[8] == 1);
                ASSERT_TRUE(tform[i].data[9] == 1);

                ASSERT_TRUE(bounds[i].data[0] == 2);
                ASSERT_TRUE(bounds[i].data[1] == 2);
                ASSERT_TRUE(bounds[i].data[2] == 2);
                ASSERT_TRUE(bounds[i].data[3] == 2);
                ASSERT_TRUE(bounds[i].data[4] == 2);
                ASSERT_TRUE(bounds[i].data[5] == 2);
        }

        #undef ENT_COUNT
}

UTEST_F(block, get_empty_blocks) {
        size_t count = 0;

        /* Create an entity this should not appear in a chunk
         */

        uintptr_t ent = chunky_entity_create(utest_fixture->ctx, 0);

        /* Find chunks with these components
         * We should get nothing, because the above entity has no components.
         */

        int ok = chunky_find_blocks(
                utest_fixture->ctx,
                utest_fixture->tform_compid | utest_fixture->bounds_compid,
                NULL,
                &count);

        ASSERT_TRUE(ok > 0);
        ASSERT_TRUE(count == 0);

        /* Pass an array for chunk headers
         * This is just to ensure this works, it should contain nothing.
         */

        struct chunky_block_header *headers[3] = {0};

        ok = chunky_find_blocks(
                utest_fixture->ctx,
                utest_fixture->tform_compid | utest_fixture->bounds_compid,
                headers,
                &count);

        ASSERT_TRUE(ok > 0);
        ASSERT_TRUE(count == 0);
}

UTEST_F(block, get_block) {
        size_t count = 0;

        /* Create an entity
         * With a some components, we should be able to find the chunk of this
         * entity, by matching the components.
         */

        uintptr_t ent = chunky_entity_create(
                utest_fixture->ctx, 
                utest_fixture->tform_compid | utest_fixture->bounds_compid);

        int ok = chunky_find_blocks(
                utest_fixture->ctx,
                utest_fixture->tform_compid | utest_fixture->bounds_compid,
                NULL,
                &count);

        ASSERT_TRUE(ok > 0);
        ASSERT_TRUE(count == 1);

        /* We should be able to get the chunk header that this array landed in.
         */

        struct chunky_block_header *headers[3] = {0};

        ok = chunky_find_blocks(
                utest_fixture->ctx,
                utest_fixture->tform_compid | utest_fixture->bounds_compid,
                headers,
                &count);

        ASSERT_TRUE(ok > 0);
        ASSERT_TRUE(count == 1);
}

UTEST_F(block, get_block_with_subset) {
        size_t count = 0;

        /* Create an entity
         * With a some components, we should be able to find the chunk of this
         * entity with sub components, eg a subset of the components the entity
         * actually has.
         */

        uintptr_t ent = chunky_entity_create(
                utest_fixture->ctx, 
                utest_fixture->tform_compid | utest_fixture->bounds_compid | utest_fixture->renderable_compid);

        int ok = chunky_find_blocks(
                utest_fixture->ctx,
                utest_fixture->tform_compid | utest_fixture->bounds_compid,
                NULL,
                &count);

        ASSERT_TRUE(ok > 0);
        ASSERT_TRUE(count == 1);

        /* We should be able to get the chunk header that this array landed in.
         */

        struct chunky_block_header *headers[3] = {0};

        ok = chunky_find_blocks(
                utest_fixture->ctx,
                utest_fixture->tform_compid | utest_fixture->bounds_compid,
                headers,
                &count);

        ASSERT_TRUE(ok > 0);
        ASSERT_TRUE(count == 1);
}

UTEST_F(block, get_no_block_from_wrong_types) {
        size_t count = 0;

        /* Create an entity
         * With a some components, we shouldn't find this entity when searching
         * for chunks with different components.
         */

        uintptr_t ent = chunky_entity_create(
                utest_fixture->ctx, 
                utest_fixture->tform_compid | utest_fixture->bounds_compid);

        int ok = chunky_find_blocks(
                utest_fixture->ctx,
                utest_fixture->renderable_compid,
                NULL,
                &count);

        ASSERT_TRUE(ok > 0);
        ASSERT_TRUE(count == 0);
}
