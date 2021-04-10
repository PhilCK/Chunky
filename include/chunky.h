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
        struct chunky_ctx **ctx);                   /* requierd */

/* -------------------------------------------------------------------------- */
/* Components
 */

struct chunky_component_desc {
        const char *name;
        size_t bytes;
};

int
chunky_components_create(
        struct chunky_ctx *ctx,                     /* requierd */
        const struct chunky_component_desc *desc,   /* requierd */
        int desc_count,                             /* requierd */
        uint64_t *out_component_ids);               /* requierd */

size_t
chunky_components_count(
        const struct chunky_ctx *ctx);              /* requierd */

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
} /* extern */
#endif

#endif
