#include <anvil_c.h>
#include <stdio.h>
#include <stdlib.h>

static void check(void *ptr, const char *label)
{
    if (!ptr) {
        fprintf(stderr, "Error at %s: %s\n", label, anvil_get_error());
        exit(1);
    }
}

int main(void)
{
    anvil_ctx_t ctx = anvil_ctx_create();
    check(ctx, "ctx");

    anvil_mod_t mod = anvil_mod_create("hello_world_c");
    check(mod, "mod");

    anvil_type_t i32    = anvil_ctx_get_i32_ty(ctx);
    anvil_type_t fn_ty  = anvil_ctx_get_fn_ty(ctx, i32, NULL, 0);
    check(fn_ty, "fn_ty");

    anvil_fn_t fn = anvil_fn_create(fn_ty, "main");
    check(fn, "fn");

    anvil_bb_t entry = anvil_bb_create(ctx, "entry");
    check(entry, "entry");

    anvil_builder_t builder = anvil_builder_create(ctx, entry);
    check(builder, "builder");

    anvil_val_t zero = anvil_ctx_const_int(ctx, i32, 0);
    check(zero, "zero");

    anvil_val_t ret = anvil_build_ret(builder, zero);
    check(ret, "ret");

    anvil_fn_add_bb(fn, entry);
    anvil_mod_add_fn(mod, fn);

    char *ir = anvil_mod_print_to_string(mod);
    check(ir, "print");
    printf("%s", ir);
    free(ir);

    anvil_builder_destroy(builder);
    anvil_mod_destroy(mod);
    anvil_ctx_destroy(ctx);

    return 0;
}
