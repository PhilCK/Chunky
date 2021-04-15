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

        if(block != NULL) {
                return entity;
        }

        /* No suitable block was found, we need to create a new block with
         * these types.
         */

        chunky_block_insert_slot(ctx, components, entity, NULL, NULL);

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

int
chunky_entity_name_set(
        struct chunky_ctx *ctx,
        uintptr_t entity,
        const char *name)
{
        assert(ctx && "No null ctx");
        assert(entity && "Must be an entity");

        struct chunky_entity *ent = (struct chunky_entity*)entity;

        /* set the name 
         * we append a null char on the end just to be sure
         */

        memset(ent->name, 0, sizeof(ent->name));
        strncat(ent->name, name, sizeof(ent->name));
        ent->name[CHUNKY_MAX_NAME_LEN - 1] = '\0';

        return 1;
}

int
chunky_entity_name_get(
        struct chunky_ctx *ctx,
        uintptr_t entity,
        char *name,
        size_t *name_len)
{
        assert(ctx && "No null ctx");
        assert(entity && "Must be an entity");

        struct chunky_entity *ent = (struct chunky_entity*)entity;

        /* get the name 
         */

        size_t len = strlen(ent->name);
        *name_len = len;

        if(!name) {
                return 1;
        }

        memcpy(name, ent->name, len);

        return 1;
}

#undef CHUNKY_ENTITY_EXISTS
#undef CHUNKY_ENTITY_FREE
