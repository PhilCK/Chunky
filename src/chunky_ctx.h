#ifndef INCLUDED_9B209C02_94F8_47AA_9873_C9E05012D83D
#define INCLUDED_9B209C02_94F8_47AA_9873_C9E05012D83D

#include <stdint.h>
#include <stddef.h>
#include <chunky.h>

/* -------------------------------------------------------------------------- */
/* Config
 */

#ifndef CHUNKY_MAX_ENTITIES
#define CHUNKY_MAX_ENTITIES 128
#endif

#ifndef CHUNKY_MAX_BLOCKS
#define CHUNKY_MAX_BLOCKS 32
#endif

/* -------------------------------------------------------------------------- */
/* Helpers and Constants
 */

/* We could remove the zero check as we don't really need it. As entities take
 * the inital slot.
 */
static int
ispow2(uint64_t i) {
	return ((i != 0) && ((i & (i - 1)) == 0));
}

/* Since components are powers of two we need to convert them to an index.
 * The general solution to this is (int)log2(n), you could also use a clz
 * implementation. We are doing just a for loop until we have benchmarking.
 * unrolling this could also be an option.
 */
static int
id2idx(uint64_t id) {
        for(uint64_t i = 0; i < 64; ++i) {
        	uint64_t j = 1ULL << i;

        	if(id == j) {
        		return (int)i;
        	}
        }

        return -1;
}

/* -------------------------------------------------------------------------- */
/* Context Data
 */

struct chunk_info {
        uint64_t layout;
};

struct chunk_block {
        struct chunky_block_header header;
        uint8_t data[16384 - sizeof(struct chunky_block_header)];
};

struct chunk_layout {
        uint64_t bit_pattern;
        uint16_t deltas[64];
        uint8_t strides[64];
};

struct chunky_entity {
        size_t idx;
        char name[32];
        uint64_t components;
};

struct chunky_component {
        char name[32];
        size_t bytes;
        uint64_t bit;
};

struct chunky_ctx {
        struct chunk_info info[CHUNKY_MAX_BLOCKS];
        struct chunk_block block[CHUNKY_MAX_BLOCKS];

        struct chunk_layout layouts[64];
        int layout_count;

        uint8_t entity_state[CHUNKY_MAX_ENTITIES];
        struct chunky_entity entities[CHUNKY_MAX_ENTITIES];

        struct chunky_component comps[CHUNKY_MAX_COMPONENTS];
        int comp_count;
};

/* -------------------------------------------------------------------------- */
/* Internal API Calls
 */

int
chunky_block_insert_slot(
        struct chunky_ctx *ctx,
        uint64_t layout,
        uintptr_t entity_id,
        uintptr_t *out_block,
        int *out_slot);



/* -------------------------------------------------------------------------- */

#endif
