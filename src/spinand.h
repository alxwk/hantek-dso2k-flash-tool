/* SPDX-License-Identifier: MIT
 * Copyright 2024      Jorenar
 * Copyright 2022-2024 DavidAlfa
 * Copyright 2007-2022 Jianjun Jiang <8192542@qq.com>
 */

#ifndef SPINAND_H_
#define SPINAND_H_

#include <fel.h>

int spinand_detect(struct xfel_ctx_t *ctx, char *name, size_t *capacity);

int dso2d_dump(struct xfel_ctx_t *ctx, void *buf);
int dso2d_restore(struct xfel_ctx_t *ctx, void *buf);
int dso2d_erase(struct xfel_ctx_t *ctx);
int dso2d_dump_regs(struct xfel_ctx_t *ctx);

#endif // SPINAND_H_
