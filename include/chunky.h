#ifndef CHUNKY_INCLUDED_0015A45E_502C_4273_A07A_EF7163D2CCD0
#define CHUNKY_INCLUDED_0015A45E_502C_4273_A07A_EF7163D2CCD0

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct chunky_ctx;

/* -------------------------------------------------------------------------- */
/* Lifetime
 */

struct chunky_ctx*
chunky_create();

int
chunky_destroy(
        struct chunky_ctx **ctx);

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
} /* extern */
#endif

#endif
