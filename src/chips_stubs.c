/* SPDX-License-Identifier: MIT
 * Copyright 2025      Alex Volkov
 * Copyright 2024      Jorenar
 */

#include <fel.h>

/* empty stubs need a non-NULL chip->detect pointer, as it's called by fel_init
   while detecting chips */

static int dummy_detect(struct xfel_ctx_t * ctx, uint32_t id)
{
    return 0;
}

struct chip_t a10          = { .detect = dummy_detect };
struct chip_t a13_a10s_r8  = { .detect = dummy_detect };
struct chip_t a20          = { .detect = dummy_detect };
struct chip_t a23          = { .detect = dummy_detect };
struct chip_t a31          = { .detect = dummy_detect };
struct chip_t a33_r16      = { .detect = dummy_detect };
struct chip_t a40i_r40     = { .detect = dummy_detect };
struct chip_t a64          = { .detect = dummy_detect };
struct chip_t a80          = { .detect = dummy_detect };
struct chip_t a83t         = { .detect = dummy_detect };
struct chip_t d1_f133      = { .detect = dummy_detect };
struct chip_t h2_h3        = { .detect = dummy_detect };
struct chip_t h5           = { .detect = dummy_detect };
struct chip_t h6           = { .detect = dummy_detect };
struct chip_t h616         = { .detect = dummy_detect };
struct chip_t r328         = { .detect = dummy_detect };
struct chip_t r329         = { .detect = dummy_detect };
struct chip_t r528_t113    = { .detect = dummy_detect };
struct chip_t t507         = { .detect = dummy_detect };
struct chip_t v3s_s3       = { .detect = dummy_detect };
struct chip_t v536         = { .detect = dummy_detect };
struct chip_t v831         = { .detect = dummy_detect };
