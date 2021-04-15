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

        if(idx == -1) {
                assert(!"Invalid id - We need a component id");
                return NULL;
        }

        /* Find the offset for the chunk.
         */

        if(!header->components[idx]) {
                assert(!"This component data is not valid in this chunk");
        }

        return (void*)header->components[idx];
}

uintptr_t *
chunky_block_entities(
        struct chunky_ctx *ctx,                         /* required */
        struct chunky_block_header *header)             /* required */
{
        assert(ctx && "Requires a context");
        assert(header && "Requires a header");

        /* Return the entity array
         */

        return (uintptr_t*const)header->entities;
}

/* -------------------------------------------------------------------------- */
/* Internal
 */

int
chunky_block_insert_slot(
        struct chunky_ctx *ctx,
        uint64_t layout,
        uintptr_t entity_id,
        uintptr_t *out_block,
        int *out_slot)
{
        struct chunk_block *block = NULL;

        /* search for a layout with a free slot */

        for(int i = 0; i < CHUNKY_MAX_BLOCKS; ++i) {
                if((ctx->info[i].layout & layout) != layout) {
                        continue;
                }

                if(ctx->block[i].header.count >= ctx->block[i].header.capacity) {
                        continue;
                }

                block = &ctx->block[i];
                break;
        }
        
        /* search for an empty chunk
         */

        if(!block) {
                /* find a new block that is emtpy */

                for(int i = 0; i < CHUNKY_MAX_BLOCKS; ++i) {
                        if(ctx->info[i].layout != 0) {
                                continue;
                        }

                        /* setup chunk */

                        ctx->info[i].layout = layout;
                        block = &ctx->block[i];

                        break;
                }

                if(!block) {
                        assert(!"Failed to find a block");
                        return 0;
                }

                /* Can we fit this into this chunk, right now all we assume we can fit
                 * 4 instances into a chunk, this does mean if our objects are too large
                 * this function will return 0, and if our objects are small there is
                 * alot of wasted space. The 16 byte padding is to remind me to align
                 * on 16 byte boundries...
                 */

                /* bytes needed */

                size_t bytes_per_ent = sizeof(uintptr_t); /* entity */
                size_t elements = 1;

                for(int i = 0; i < CHUNKY_MAX_COMPONENTS; ++i) {
                        
                        if(!(layout & ctx->comps[i].bit)) {
                                continue;
                        }

                        bytes_per_ent += ctx->comps[i].bytes;
                        elements += 1;
                }

                size_t dst_size = sizeof(((struct chunk_block*)0)->data);
                dst_size -= (32 * elements);

                size_t max_capacity = dst_size / bytes_per_ent;

                /* bytes needed */

                // size_t bytes_needed = sizeof(uintptr_t) * 4;
                // bytes_needed += 32;

                // for(int i = 0; i < CHUNKY_MAX_COMPONENTS; ++i) {
                        
                //         if(!(layout & ctx->comps[i].bit)) {
                //                 continue;
                //         }

                //         bytes_needed += ctx->comps[i].bytes * 4;
                //         bytes_needed += 32;
                // }

                // // 15864 space in the data part of a chunk.
                // if(bytes_needed >= 15864) {
                //         assert(!"Doesn't fit in a chunk");
                //         return 0;
                // }

                /* For each component calculate where it sits in the data
                 * section of a chunk.
                 */

                uint16_t deltas[CHUNKY_MAX_COMPONENTS] = {0};
                uint8_t strides[CHUNKY_MAX_COMPONENTS] = {0};

                uint16_t dt_idx = 0;
                deltas[dt_idx] = 0; /* entity array */
                strides[dt_idx] = sizeof(uintptr_t);

                size_t byte_offset = sizeof(uintptr_t) * max_capacity;
                byte_offset += 32;

                for(int i = 0; i < CHUNKY_MAX_COMPONENTS; ++i) {
                        
                        int comp_idx = i;
                        uint64_t src_bit = ctx->comps[comp_idx].bit;

                        if(!(src_bit & layout)) {
                                continue;
                        }

                        size_t src_bytes = ctx->comps[comp_idx].bytes;

                        deltas[dt_idx] = (uint16_t)byte_offset;
                        strides[dt_idx] = (uint8_t)src_bytes;
                        dt_idx += 1;

                        assert(dt_idx <= CHUNKY_MAX_COMPONENTS);

                        byte_offset += src_bytes * max_capacity;
                        byte_offset += 32;

                        assert(byte_offset < 0xFFFF && "Make sure uint16_t is big enough");
                }

                /* Setup chunk 
                 * Now we have processed the layout information we can setup the
                 * chunk ready for use.
                 */

                /* sizes */

                memcpy(block->header.strides, strides, sizeof(block->header.strides));

                /* data offset */

                uintptr_t start = (uintptr_t)block->data;
                block->header.entities = start; /* entity array */

                for(int j = 0; j < CHUNKY_MAX_COMPONENTS; ++j) {
                        block->header.components[j] = (start + deltas[j]) * !!deltas[j];
                }

                block->header.capacity = (uint16_t)max_capacity;
                block->header.count = 0;
        }

        if(!block) {
                assert(!"Failed to setup a new block");
                return 0;
        }


        /* We can insert the new entity into the block
         */

        uintptr_t *ent = (uintptr_t*)block->header.entities;

        size_t curr_idx = block->header.count;
        ent[curr_idx] = entity_id;
        block->header.count += 1;

        return 1;
}

