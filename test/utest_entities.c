#include <utest.h>
#include <chunky.h>
#include <string.h>
#include "../src/chunky_ctx.h"

struct entity {
        struct chunky_ctx *ctx;
        uint64_t comp_id[2];
};

struct tform {
        float position[3];
        float scale[3];
        float rotation[4];
};

struct bounds {
        float scale[3];
};

UTEST_F_SETUP(entity) {

        /* Create a chunky ctx
         */

        struct chunky_ctx *ch_ctx = 0;
        ch_ctx = chunky_create();

        ASSERT_TRUE(ch_ctx != 0);
        utest_fixture->ctx = ch_ctx;
        
        struct chunky_component_desc desc[2] = {
                [0] = {
                        .name = "tform",
                        .bytes = sizeof(struct tform),
                },
                [1] = {
                        .name = "bounds",
                        .bytes = sizeof(struct bounds),
                }
        };

        int ok = chunky_components_create(
                utest_fixture->ctx,
                desc,
                2,
                utest_fixture->comp_id);

        ASSERT_TRUE(ok != 0);
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

UTEST_F(entity, get_set_name) {

        /* Create an entity and set it's name.
         */

        uintptr_t ent = chunky_entity_create(utest_fixture->ctx, 0);

        /* Set the name to something unique
         */

        const char *name = "foobar";

        (void)chunky_entity_name_set(
                utest_fixture->ctx,
                ent,
                name);

        size_t len = 0;
        (void)chunky_entity_name_get(
                utest_fixture->ctx,
                ent,
                NULL,
                &len);

        ASSERT_TRUE(len <= CHUNKY_MAX_NAME_LEN);

        /* Get the name, it should be the same as what we set.
         */

        char get_name[CHUNKY_MAX_NAME_LEN] = {0};

        (void)chunky_entity_name_get(
                utest_fixture->ctx,
                ent,
                get_name,
                &len);

        ASSERT_TRUE(strcmp(get_name, name) == 0);
}

UTEST_F(entity, get_set_name_clipped) {

        /* Create an entity and set it's name.
         */

        uintptr_t ent = chunky_entity_create(utest_fixture->ctx, 0);

        /* Set the name to something really long
         */

        const char *name = "really_really_really_really_long_name";
        size_t original_len = strlen(name);

        ASSERT_TRUE(original_len > CHUNKY_MAX_NAME_LEN);

        (void)chunky_entity_name_set(
                utest_fixture->ctx,
                ent,
                name);

        size_t len = 0;
        (void)chunky_entity_name_get(
                utest_fixture->ctx,
                ent,
                NULL,
                &len);

        ASSERT_TRUE(len < original_len);

        /* Get the name, it should have its tail cut off.
         */

        char get_name[CHUNKY_MAX_NAME_LEN] = {0};

        (void)chunky_entity_name_get(
                utest_fixture->ctx,
                ent,
                get_name,
                &len);

        ASSERT_TRUE(strstr(name, get_name) != NULL);
}

UTEST_F(entity, get_set_component_data) {
        /* Create an entity 
         */

        uintptr_t ent = chunky_entity_create(
                utest_fixture->ctx,
                utest_fixture->comp_id[0] | utest_fixture->comp_id[1]);

        /* Get the component data
         */

        uintptr_t comp_data_1 = chunky_entity_component_get(
                utest_fixture->ctx,
                ent,
                utest_fixture->comp_id[0]);
 
        ASSERT_TRUE(comp_data_1 > 0);

        struct tform *tform_data = (struct tform*)comp_data_1;
        tform_data->position[0] = 123.f;
        tform_data->scale[0] = 234.f;
        tform_data->rotation[0] = 345.f;

        uintptr_t comp_data_2 = chunky_entity_component_get(
                utest_fixture->ctx,
                ent,
                utest_fixture->comp_id[1]);

        ASSERT_TRUE(comp_data_2 > 0);

        struct bounds *bounds_data = (struct bounds*)comp_data_2;
        bounds_data->scale[0] = 999.f;

        uintptr_t comp_data_3 = chunky_entity_component_get(
                utest_fixture->ctx,
                ent,
                64 /* id shouldn't exit */);

        ASSERT_TRUE(comp_data_3 == 0);

        /* Check against memory clobber
         */

        ASSERT_TRUE(tform_data->position[0] == 123.f);
        ASSERT_TRUE(tform_data->scale[0] == 234.f);
        ASSERT_TRUE(tform_data->rotation[0] = 345.f);
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

