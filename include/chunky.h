#ifndef CHUNKY_INCLUDED_0015A45E_502C_4273_A07A_EF7163D2CCD0
#define CHUNKY_INCLUDED_0015A45E_502C_4273_A07A_EF7163D2CCD0

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct chunky_ctx;
struct chunky_recorder;

/* -------------------------------------------------------------------------- */
/* Lifetime
 */

struct chunky_ctx*
chunky_create();

int
chunky_destroy(
        struct chunky_ctx **ctx);                       /* required */

/* -------------------------------------------------------------------------- */
/* Entities
 */

uintptr_t
chunky_entity_create(
        struct chunky_ctx *ctx,                         /* required */
        uint64_t components);

int
chunky_entity_destroy(
        struct chunky_ctx *ctx,                         /* required */
        uintptr_t entity);                              /* required */

/* -------------------------------------------------------------------------- */
/* Components
 */

struct chunky_component_desc {
        const char *name;
        size_t bytes;
};

int
chunky_components_create(
        struct chunky_ctx *ctx,                         /* required */
        const struct chunky_component_desc *desc,       /* required */
        int desc_count,                                 /* required */
        uint64_t *out_component_ids);                   /* required */

size_t
chunky_components_count(
        const struct chunky_ctx *ctx);                  /* required */

/* -------------------------------------------------------------------------- */
/* Chunks
 */

struct chunky_chunk_header {
        uint32_t count;
        uint32_t capacity;
        uint8_t strides[64];
        uintptr_t components[64];
};

int
chunky_find_chunks(
        struct chunky_ctx *ctx,                         /* required */
        uint64_t components,                            /* required */
        struct chunky_chunk_header **out_headers,       /* optional */
        size_t *count);                                 /* required */

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
} /* extern */
#endif

#endif
