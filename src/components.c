#include <chunky.h>
#include "chunky_ctx.h"
#include <assert.h>
#include <string.h>

int
chunky_components_create(
        struct chunky_ctx *ctx,
        const struct chunky_component_desc *desc,
        int desc_count,
        uint64_t *out_component_ids)
{
        assert(ctx && "CTX cannot be null");

        if(desc_count) {
                assert(out_component_ids && "We need to write data out.");
        }

        /* Insert the components into the array, and fill in the out array.
         */

        for(int i = 0; i < desc_count; ++i)
        {
                const struct chunky_component_desc *cd = &desc[i];
                out_component_ids[i] = 0;

                /* First search to see if this component has already been added
                 * and use that id instead.
                 */

                for(int j = 0; j < CHUNKY_MAX_COMPONENTS; ++j) {
                        struct chunky_component *cur = &ctx->comps[j];

                        if(strcmp(cd->name, cur->name) == 0) {
                                /* Same bytes? Same Type :/ */
                                assert(cd->bytes == cur->bytes);
                                out_component_ids[i] = cur->bit;
                                break;
                        }
                }

                if(out_component_ids[i]) {
                        continue;
                }

                /* Now add that component
                 */

                if(ctx->comp_count >= CHUNKY_MAX_COMPONENTS) {
                        assert(!"Component limit reached, max is 64!");
                        return 0;
                }

                struct chunky_component *cd_dst = &ctx->comps[ctx->comp_count];

                /* I haven't tested this, but an empty struct/class in C++ is 1
                 * byte, and I'm not sure what the various C standards and
                 * Compilers actually do for C code, So I'm pushing it onto the
                 * calling code to have at least 1 byte components.
                 */
                assert(cd->bytes && "Cannot have zero sized components");

                cd_dst->bytes = cd->bytes;
                cd_dst->bit = 1ULL << ctx->comp_count;
                (void)strncpy(cd_dst->name, cd->name, sizeof(cd_dst->name));

                out_component_ids[i] = cd_dst->bit;
                ctx->comp_count += 1;
        }

        return 1;
}

size_t
chunky_components_count(
        const struct chunky_ctx *ctx)
{
        return ctx->comp_count;
}
