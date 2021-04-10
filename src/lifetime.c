#include <chunky.h>
#include "chunky_ctx.h"
#include <stdlib.h>

struct chunky_ctx*
chunky_create()
{
        struct chunky_ctx *ctx = malloc(sizeof(struct chunky_ctx));
        return ctx;
}

int
chunky_destroy(
        struct chunky_ctx **ctx)
{
        free(*ctx);
        *ctx = 0;

        return 1; 
}
