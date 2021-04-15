#ifndef CHUNKY_INCLUDED_0015A45E_502C_4273_A07A_EF7163D2CCD0
#define CHUNKY_INCLUDED_0015A45E_502C_4273_A07A_EF7163D2CCD0

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */
/* Opaque Types and Other Bits
 */

struct chunky_ctx;
struct chunky_recorder;

#define CHUNKY_MAJOR_VER 0
#define CHUNKY_MINOR_VER 0
#define CHUNKY_PATCH_VER 0

#define CHUNKY_MAX_COMPONENTS 64

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

int
chunky_entity_name_set(
        struct chunky_ctx *ctx,                         /* required */
        uintptr_t entity,
        const char *name);                              /* required */

int
chunky_entity_name_get(
        struct chunky_ctx *ctx,                         /* required */
        uintptr_t entity,
        char *name,                                     /* required */
        size_t *name_len);                              /* required */

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
/* Blocks
 */

struct chunky_block_header {
        uint16_t count;
        uint16_t capacity;
        uint8_t strides[64];
        uintptr_t entities;
        uintptr_t components[64];
};

int
chunky_find_blocks(
        struct chunky_ctx *ctx,                         /* required */
        uint64_t components,                            /* required */
        struct chunky_block_header **out_headers,       /* optional */
        size_t *count);                                 /* required */

uintptr_t *
chunky_block_entities(
        struct chunky_ctx *ctx,                         /* required */
        struct chunky_block_header *header);            /* required */

void *
chunky_block_data(
        struct chunky_ctx *ctx,                         /* required */
        struct chunky_block_header *header,             /* required */
        uint64_t component);

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
} /* extern */
#endif

#endif
