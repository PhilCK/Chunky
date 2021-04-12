#include <chunky.h>
#include "chunky_ctx.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define CHUNKY_ENTITY_FREE 0
#define CHUNKY_ENTITY_EXISTS 1

uintptr_t
chunky_entity_create(
        struct chunky_ctx *ctx,
        uint64_t components)
{
        assert(ctx && "Need a valid context");

        /* Search for an space in the entity array
         */

        uintptr_t entity = 0;

        for(int i = 0; i < CHUNKY_MAX_ENTITIES; ++i) {
                if(ctx->entity_state[i] != CHUNKY_ENTITY_FREE) {
                        continue;
                }

                ctx->entity_state[i] = CHUNKY_ENTITY_EXISTS;
                entity = (uintptr_t)&ctx->entities[i];

                /* The index's are lazy setup
                 */

                ctx->entities[i].idx = i;

                break;
        }

        if(entity == 0) {
                assert(!"Failed to find a space");
                return 0;
        }

        /* Find or create a block for this entity's data to live in.
         */

        struct chunk_block *block = NULL;

        /* Search for a chunk with these types, and has an available slot for
         * the entity to take.
         */

        // for(int i = 0; i < CHUNKY_MAX_CHUNKS; ++i) {

        //         /* Skip as chunk doesn't have the right types.
        //          */

        //         if(ctx->info[i].layout != components) {
        //                 continue;
        //         }

        //         /* Skip as chunk doesn't have space
        //          */

        //         struct chunk_block *potential = &ctx->block[i];

        //         if(potential->header.count >= potential->header.capacity) {
        //                 continue;
        //         }

        //         /* Ok we seem to have found a block that has this type and
        //          * some space.
        //          */

        //         block = potential;
        //         break;
        // }

        if(block != NULL) {
                return entity;
        }

        /* No suitable block was found, we need to create a new block with
         * these types.
         */

        for(int i = 0; i < CHUNKY_MAX_CHUNKS; ++i) {

                /* If this chunk has a type keep moving.
                 */

                if(ctx->info[i].layout != 0) {
                        continue;
                }

                /* We found a free chunk so we can allocate this entity to it.
                 */

                ctx->info[i].layout = components;
                ctx->block[i].header.count = 1;
                ctx->block[i].header.capacity = 4;

                break;
        }

        return entity;
}

int
chunky_entity_destroy(
        struct chunky_ctx *ctx,
        uintptr_t entity)
{
        assert(ctx && "No null ctx");
        assert(entity && "Must be an entity");

        /* mark as free */

        struct chunky_entity *e = NULL;
        e = (struct chunky_entity*)entity;

        ctx->entity_state[e->idx] = CHUNKY_ENTITY_FREE;

        /* clear data */

        e->idx = 0;
        e->components = 0;
        memset(e->name, 0, sizeof(e->name));

        return 1;
}

#undef CHUNKY_ENTITY_EXISTS
#undef CHUNKY_ENTITY_FREE
