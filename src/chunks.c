#include <chunky.h>
#include "chunky_ctx.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

int
chunky_find_chunks(
        struct chunky_ctx *ctx,                         /* required */
        uint64_t components,                            /* required */
        struct chunky_chunk_header **out_headers,       /* optional */
        size_t *out_count)                              /* required */
{
        size_t count = 0;

        /* Search for chunks that contain the types given.
         */
        for(int i = 0; i < CHUNKY_MAX_CHUNKS; ++i) {

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
