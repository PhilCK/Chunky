#ifndef INCLUDED_9B209C02_94F8_47AA_9873_C9E05012D83D
#define INCLUDED_9B209C02_94F8_47AA_9873_C9E05012D83D

#include <stdint.h>
#include <stddef.h>
#include <chunky.h>

/* -------------------------------------------------------------------------- */
/* Config
 */

#ifndef CHUNKY_MAX_ENTITIES
#define CHUNKY_MAX_ENTITIES 4096
#endif

#ifndef CHUNKY_MAX_BLOCKS
#define CHUNKY_MAX_BLOCKS 32
#endif

#ifndef CHUNKY_MAX_NAME_LEN
#define CHUNKY_MAX_NAME_LEN 32
#endif

#ifndef CHUNKY_BLOCK_ALIGNON
#define CHUNKY_BLOCK_ALIGNON 16
#endif

#ifndef CHUNKY_BLOCK_COMPONENT_GAP
#define CHUNKY_BLOCK_COMPONENT_GAP (CHUNKY_BLOCK_ALIGNON * 2)
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
        for(uint64_t i = 0; i < CHUNKY_MAX_COMPONENTS; ++i) {
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
        uint16_t deltas[CHUNKY_MAX_COMPONENTS];
        uint8_t strides[CHUNKY_MAX_COMPONENTS];
};

struct chunky_entity_slot_data {
        int slot_index;
        struct chunk_block *block; 
        uintptr_t components[CHUNKY_MAX_COMPONENTS];
};

struct chunky_entity {
        size_t idx;
        char name[CHUNKY_MAX_NAME_LEN];
        struct chunky_entity_slot_data slot_data;
};

struct chunky_component {
        char name[CHUNKY_MAX_NAME_LEN];
        size_t bytes;
        uint64_t bit;
};

struct chunky_ctx {
        struct chunk_info info[CHUNKY_MAX_BLOCKS];
        struct chunk_block block[CHUNKY_MAX_BLOCKS];

        struct chunk_layout layouts[CHUNKY_MAX_COMPONENTS];
        int layout_count;

        uint8_t entity_state[CHUNKY_MAX_ENTITIES];
        struct chunky_entity entities[CHUNKY_MAX_ENTITIES];

        struct chunky_component comps[CHUNKY_MAX_COMPONENTS];
        int comp_count;
};

/* -------------------------------------------------------------------------- */
/* Internal API Calls
 */

struct chunky_entity_slot_data
chunky_block_append_slot(
        struct chunky_ctx *ctx,
        uint64_t layout,
        uintptr_t entity_id);

int
chunky_block_remove_slot(
        struct chunky_ctx *ctx,
        struct chunk_block *block,
        int slot);

/* -------------------------------------------------------------------------- */

#endif
