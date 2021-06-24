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

        /* Find a block with space or create a new block.
         * Append this entity into a block ...
         */

        struct chunky_entity_slot_data sl = chunky_block_append_slot(
                ctx,
                components,
                entity);

        /* Set the components
         */

        for(int i = 0; i < CHUNKY_MAX_COMPONENTS; ++i) {
                int comp = 1 << i;

                if(!(comp & components)) {
                        sl.components[i] = 0;
                        continue;
                }

                /* Calculate the bytes offset
                 */

                uintptr_t byte_offset = sl.block->header.components[i];
                byte_offset += (sl.slot_index * ctx->comps[i].bytes);

                sl.components[i] = byte_offset;
        }
        
        ((struct chunky_entity*)entity)->slot_data = sl;

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

        if(e == NULL) {
                assert(!"You can't remove a null entity");
                return 0;
        }

        ctx->entity_state[e->idx] = CHUNKY_ENTITY_FREE;

        /* clear data */

        e->idx = 0;
        e->components = 0;
        memset(e->name, 0, sizeof(e->name));

        /* We need to remove the slot from the chunk it lives in
         */

        (void)chunky_block_remove_slot(
                ctx,
                e->slot_data.block,
                e->slot_data.slot_index);

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
        strncat(ent->name, name, sizeof(ent->name) - 1);
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

uintptr_t
chunky_entity_component_get(
        struct chunky_ctx *ctx,
        uintptr_t entity,
        uint64_t component_type_id)
{
        assert(ctx && "No null ctx");
        assert(entity && "Must be an entity");

        /* Find the components */

        struct chunky_entity *ent = (struct chunky_entity*)entity;

        /* Get the component
         */


        size_t comp_idx = id2idx(component_type_id); 
        return ent->slot_data.components[comp_idx];
}

#undef CHUNKY_ENTITY_EXISTS
#undef CHUNKY_ENTITY_FREE
