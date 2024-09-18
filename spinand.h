#ifndef __SPINAND_H__
#define __SPINAND_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <fel.h>

int dso2d_dump(struct xfel_ctx_t * ctx, void *buf);
int spinand_detect(struct xfel_ctx_t * ctx, char * name, size_t * capacity);
int dso2d_restore(struct xfel_ctx_t * ctx, void * buf);
int dso2d_erase(struct xfel_ctx_t * ctx);
int dso2d_dump_regs(struct xfel_ctx_t * ctx);

#ifdef __cplusplus
}
#endif

#endif /* __SPINAND_H__ */
