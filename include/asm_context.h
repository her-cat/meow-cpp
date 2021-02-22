#ifndef MEOW_ASM_CONTEXT_H
#define MEOW_ASM_CONTEXT_H

#include "meow.h"
#include "uv.h"

typedef void *fcontext_t;

extern fcontext_t make_fcontext(void *sp, size_t size, void (*fn)(intptr_t)) asm("make_fcontext");
extern intptr_t jump_fcontext(fcontext_t *ofc, fcontext_t nfc, intptr_t vp, bool preserve_fpu = false) asm("jump_fcontext");

#endif /* MEOW_ASM_CONTEXT_H */
