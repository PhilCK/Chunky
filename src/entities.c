#include <chunky.h>
#include "chunky_ctx.h"
#include <stdlib.h>
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

        /* search for an space */

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