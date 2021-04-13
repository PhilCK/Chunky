#include <chunky.h>
#include "chunky_ctx.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

/* -------------------------------------------------------------------------- */
/* Public
 */

int
chunky_find_blocks(
        struct chunky_ctx *ctx,                         /* required */
        uint64_t components,                            /* required */
        struct chunky_block_header **out_headers,       /* optional */
        size_t *out_count)                              /* required */
{
        size_t count = 0;

        /* Search for chunks that contain the types given.
         */
        for(int i = 0; i < CHUNKY_MAX_BLOCKS; ++i) {

                /* Chunk has nothing
                 */

                if(ctx->info[i].layout == 0) {
                        continue;
                }

                /* Chunk contains these types
                 */

                if(!(~ctx->info[i].layout & components)) {
                        if(out_headers) {
                                out_headers[count] = &ctx->block[i].header;
                        }

                        count += 1;
                }
        }

        *out_count = count;

        return 1;
}

void *
chunky_block_data(
        struct chunky_ctx *ctx,                         /* required */
        struct chunky_block_header *header,             /* required */
        uint64_t component)
{
        assert(ctx && "Requires a context");
        assert(header && "Requires a header");
        assert(component && "Requires a component");

        /* Check this component is actually in this chunk
         */

        int idx = id2idx(component);

        if(idx == 0) {
                assert(!"Invalid id - We need a component id");
                return NULL;
        }

        /* Find the offset for the chunk.
         */

        if(!header->components[idx]) {
                //assert(!"This component data is not valid in this chunk");
        }

        return (void*)header->components[idx];
}

/* -------------------------------------------------------------------------- */
/* Internal
 */

int
chunky_block_insert_slot(
        struct chunky_ctx *ctx,
        uint64_t layout,
        uintptr_t entity_id,
        uint8_t *component_strides,
        uint16_t *component_offsets,
        uintptr_t *out_block,
        int *out_slot)
{
        /* search for a layout with a free slot */

        struct chunk_block *ch = 0;

        for(int i = 0; i < CHUNKY_MAX_BLOCKS; ++i) {
                if((ctx->info[i].layout & layout) != layout) {
                        continue;
                }

                if(ctx->block[i].header.count >= ctx->block[i].header.capacity) {
                        continue;
                }

                ch = &ctx->block[i];
                break;
        }
        
        /* search for an empty chunk */

        if(!ch) {
                for(int i = 0; i < CHUNKY_MAX_BLOCKS; ++i) {
                        if(ctx->info[i].layout != 0) {
                                continue;
                        }

                        /* setup chunk */

                        ctx->info[i].layout = layout;
                        ch = &ctx->block[i];

                        /* sizes */

                        memcpy(ch->header.strides, component_strides, sizeof(ch->header.strides));

                        /* data offset */

                        uintptr_t start = (uintptr_t)ch->data;
                        ch->header.components[0] = start; /* entity array */

                        for(int j = 1; j < 64; ++j) {
                                ch->header.components[j] = (start + component_offsets[j]) * !!component_offsets[j];
                        }

                        ch->header.capacity = 4;
                        ch->header.count = 0;

                        break;
                }
        }

        /* if we don't have an empty block its all full */

        if(!ch) {
                assert(!"All blocks are full");
                return 0;
        }

        /* insert new entity */

        uintptr_t *entities = (uintptr_t*)ch->header.components[0];
        entities[ch->header.count] = entity_id;

        *out_block = (uintptr_t)ch;
        *out_slot = ch->header.count;

        ch->header.count += 1;

        return 1;
}

