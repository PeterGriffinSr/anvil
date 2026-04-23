#ifndef ANVIL_C_H
#define ANVIL_C_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

typedef struct anvil_ctx_s *anvil_ctx_t;
typedef struct anvil_mod_s *anvil_mod_t;
typedef struct anvil_fn_s *anvil_fn_t;
typedef struct anvil_bb_s *anvil_bb_t;
typedef struct anvil_builder_s *anvil_builder_t;
typedef struct anvil_val_s *anvil_val_t;
typedef struct anvil_type_s *anvil_type_t;

const char *anvil_get_error(void);

anvil_ctx_t anvil_ctx_create(void);
void anvil_ctx_destroy(anvil_ctx_t ctx);

anvil_type_t anvil_ctx_get_void_ty(anvil_ctx_t ctx);
anvil_type_t anvil_ctx_get_i1_ty(anvil_ctx_t ctx);
anvil_type_t anvil_ctx_get_i8_ty(anvil_ctx_t ctx);
anvil_type_t anvil_ctx_get_i16_ty(anvil_ctx_t ctx);
anvil_type_t anvil_ctx_get_i32_ty(anvil_ctx_t ctx);
anvil_type_t anvil_ctx_get_i64_ty(anvil_ctx_t ctx);
anvil_type_t anvil_ctx_get_half_ty(anvil_ctx_t ctx);
anvil_type_t anvil_ctx_get_float_ty(anvil_ctx_t ctx);
anvil_type_t anvil_ctx_get_double_ty(anvil_ctx_t ctx);
anvil_type_t anvil_ctx_get_label_ty(anvil_ctx_t ctx);

anvil_type_t anvil_ctx_get_ptr_ty(anvil_ctx_t ctx, anvil_type_t elem);
anvil_type_t anvil_ctx_get_array_ty(anvil_ctx_t ctx, anvil_type_t elem,
                                    unsigned count);
anvil_type_t anvil_ctx_get_vector_ty(anvil_ctx_t ctx, anvil_type_t elem,
                                     unsigned count);
anvil_type_t anvil_ctx_get_fn_ty(anvil_ctx_t ctx, anvil_type_t ret,
                                 const anvil_type_t *params,
                                 unsigned num_params);
anvil_type_t anvil_ctx_get_struct_ty(anvil_ctx_t ctx, const anvil_type_t *elems,
                                     unsigned num_elems);

anvil_val_t anvil_ctx_const_int(anvil_ctx_t ctx, anvil_type_t ty, int64_t val);
anvil_val_t anvil_ctx_const_fp(anvil_ctx_t ctx, anvil_type_t ty, double val);
anvil_val_t anvil_ctx_const_null(anvil_ctx_t ctx, anvil_type_t ptr_ty);

anvil_mod_t anvil_mod_create(const char *name);
void anvil_mod_destroy(anvil_mod_t mod);

void anvil_mod_set_target_triple(anvil_mod_t mod, const char *triple);
void anvil_mod_set_data_layout(anvil_mod_t mod, const char *layout);

void anvil_mod_add_fn(anvil_mod_t mod, anvil_fn_t fn);

anvil_val_t anvil_mod_add_string(anvil_mod_t mod, anvil_ctx_t ctx,
                                 const char *str);

char *anvil_mod_print_to_string(anvil_mod_t mod);

int anvil_mod_print_to_file(anvil_mod_t mod, const char *path);

anvil_fn_t anvil_fn_create(anvil_type_t fn_ty, const char *name);
void anvil_fn_destroy(anvil_fn_t fn);

void anvil_fn_add_bb(anvil_fn_t fn, anvil_bb_t bb);

unsigned anvil_fn_num_args(anvil_fn_t fn);
anvil_val_t anvil_fn_get_arg(anvil_fn_t fn, unsigned idx);

anvil_bb_t anvil_bb_create(anvil_ctx_t ctx, const char *name);
void anvil_bb_destroy(anvil_bb_t bb);

anvil_builder_t anvil_builder_create(anvil_ctx_t ctx, anvil_bb_t bb);
void anvil_builder_destroy(anvil_builder_t b);
void anvil_builder_set_bb(anvil_builder_t b, anvil_bb_t bb);

anvil_val_t anvil_build_br(anvil_builder_t b, anvil_bb_t dest);
anvil_val_t anvil_build_cond_br(anvil_builder_t b, anvil_val_t cond,
                                anvil_bb_t then_bb, anvil_bb_t else_bb);
anvil_val_t anvil_build_ret(anvil_builder_t b, anvil_val_t val);
anvil_val_t anvil_build_ret_void(anvil_builder_t b);
anvil_val_t anvil_build_unreachable(anvil_builder_t b);

anvil_val_t anvil_build_add(anvil_builder_t b, anvil_val_t lhs,
                            anvil_val_t rhs);
anvil_val_t anvil_build_sub(anvil_builder_t b, anvil_val_t lhs,
                            anvil_val_t rhs);
anvil_val_t anvil_build_mul(anvil_builder_t b, anvil_val_t lhs,
                            anvil_val_t rhs);
anvil_val_t anvil_build_udiv(anvil_builder_t b, anvil_val_t lhs,
                             anvil_val_t rhs);
anvil_val_t anvil_build_sdiv(anvil_builder_t b, anvil_val_t lhs,
                             anvil_val_t rhs);
anvil_val_t anvil_build_urem(anvil_builder_t b, anvil_val_t lhs,
                             anvil_val_t rhs);
anvil_val_t anvil_build_srem(anvil_builder_t b, anvil_val_t lhs,
                             anvil_val_t rhs);
anvil_val_t anvil_build_shl(anvil_builder_t b, anvil_val_t lhs,
                            anvil_val_t rhs);
anvil_val_t anvil_build_lshr(anvil_builder_t b, anvil_val_t lhs,
                             anvil_val_t rhs);
anvil_val_t anvil_build_ashr(anvil_builder_t b, anvil_val_t lhs,
                             anvil_val_t rhs);
anvil_val_t anvil_build_and(anvil_builder_t b, anvil_val_t lhs,
                            anvil_val_t rhs);
anvil_val_t anvil_build_or(anvil_builder_t b, anvil_val_t lhs, anvil_val_t rhs);
anvil_val_t anvil_build_xor(anvil_builder_t b, anvil_val_t lhs,
                            anvil_val_t rhs);

anvil_val_t anvil_build_fadd(anvil_builder_t b, anvil_val_t lhs,
                             anvil_val_t rhs);
anvil_val_t anvil_build_fsub(anvil_builder_t b, anvil_val_t lhs,
                             anvil_val_t rhs);
anvil_val_t anvil_build_fmul(anvil_builder_t b, anvil_val_t lhs,
                             anvil_val_t rhs);
anvil_val_t anvil_build_fdiv(anvil_builder_t b, anvil_val_t lhs,
                             anvil_val_t rhs);
anvil_val_t anvil_build_frem(anvil_builder_t b, anvil_val_t lhs,
                             anvil_val_t rhs);
anvil_val_t anvil_build_fneg(anvil_builder_t b, anvil_val_t val);

anvil_val_t anvil_build_alloca(anvil_builder_t b, anvil_type_t ty);
anvil_val_t anvil_build_load(anvil_builder_t b, anvil_type_t ty,
                             anvil_val_t ptr);
anvil_val_t anvil_build_store(anvil_builder_t b, anvil_val_t val,
                              anvil_val_t ptr);
anvil_val_t anvil_build_gep(anvil_builder_t b, anvil_val_t base,
                            const anvil_val_t *indices, unsigned num_indices);

anvil_val_t anvil_build_trunc(anvil_builder_t b, anvil_val_t v,
                              anvil_type_t ty);
anvil_val_t anvil_build_zext(anvil_builder_t b, anvil_val_t v, anvil_type_t ty);
anvil_val_t anvil_build_sext(anvil_builder_t b, anvil_val_t v, anvil_type_t ty);
anvil_val_t anvil_build_fptrunc(anvil_builder_t b, anvil_val_t v,
                                anvil_type_t ty);
anvil_val_t anvil_build_fpext(anvil_builder_t b, anvil_val_t v,
                              anvil_type_t ty);
anvil_val_t anvil_build_fptoui(anvil_builder_t b, anvil_val_t v,
                               anvil_type_t ty);
anvil_val_t anvil_build_fptosi(anvil_builder_t b, anvil_val_t v,
                               anvil_type_t ty);
anvil_val_t anvil_build_uitofp(anvil_builder_t b, anvil_val_t v,
                               anvil_type_t ty);
anvil_val_t anvil_build_sitofp(anvil_builder_t b, anvil_val_t v,
                               anvil_type_t ty);
anvil_val_t anvil_build_ptrtoint(anvil_builder_t b, anvil_val_t v,
                                 anvil_type_t ty);
anvil_val_t anvil_build_inttoptr(anvil_builder_t b, anvil_val_t v,
                                 anvil_type_t ty);
anvil_val_t anvil_build_bitcast(anvil_builder_t b, anvil_val_t v,
                                anvil_type_t ty);

typedef enum {
  ANVIL_ICMP_EQ,
  ANVIL_ICMP_NE,
  ANVIL_ICMP_UGT,
  ANVIL_ICMP_UGE,
  ANVIL_ICMP_ULT,
  ANVIL_ICMP_ULE,
  ANVIL_ICMP_SGT,
  ANVIL_ICMP_SGE,
  ANVIL_ICMP_SLT,
  ANVIL_ICMP_SLE
} anvil_icmp_pred_t;

anvil_val_t anvil_build_icmp(anvil_builder_t b, anvil_icmp_pred_t pred,
                             anvil_val_t lhs, anvil_val_t rhs);
anvil_val_t anvil_build_fcmp(anvil_builder_t b, anvil_icmp_pred_t pred,
                             anvil_val_t lhs, anvil_val_t rhs);
anvil_val_t anvil_build_select(anvil_builder_t b, anvil_val_t cond,
                               anvil_val_t true_val, anvil_val_t false_val);

anvil_val_t anvil_build_call(anvil_builder_t b, anvil_type_t ret_ty,
                             anvil_val_t callee, const anvil_val_t *args,
                             unsigned num_args);

anvil_val_t anvil_build_phi(anvil_builder_t b, anvil_type_t ty);
void anvil_phi_add_incoming(anvil_val_t phi, anvil_val_t val, anvil_bb_t bb);

void anvil_val_set_name(anvil_val_t val, const char *name);

#ifdef __cplusplus
}
#endif

#endif
