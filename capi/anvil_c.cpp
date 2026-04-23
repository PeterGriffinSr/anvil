#include <anvil.hpp>
#include <anvil_c.h>

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace anvil::ir;

static thread_local std::string tl_error;

static void set_error(const std::string &msg) { tl_error = msg; }
static void clear_error() { tl_error.clear(); }

const char *anvil_get_error(void) { return tl_error.c_str(); }

template <typename Ret, typename Fn> static Ret safe(Ret fallback, Fn &&fn) {
  try {
    clear_error();
    return fn();
  } catch (const std::exception &e) {
    set_error(e.what());
    return fallback;
  } catch (...) {
    set_error("unknown error");
    return fallback;
  }
}

struct anvil_ctx_s {
  Context ctx;
};

struct anvil_mod_s {
  std::unique_ptr<Module> mod;
  explicit anvil_mod_s(std::unique_ptr<Module> m) : mod(std::move(m)) {}
};

struct anvil_fn_s {
  std::unique_ptr<Function> fn;
  explicit anvil_fn_s(std::unique_ptr<Function> f) : fn(std::move(f)) {}
};

struct anvil_bb_s {
  std::unique_ptr<BasicBlock> bb;
  explicit anvil_bb_s(std::unique_ptr<BasicBlock> b) : bb(std::move(b)) {}
};

struct anvil_builder_s {
  IRBuilder builder;
  anvil_builder_s(Context *ctx, BasicBlock *bb) : builder(ctx, bb) {}
};

struct anvil_val_s {
  Value *val;
};
struct anvil_type_s {
  Type *type;
};

static anvil_val_t make_val(Value *v) {
  return v ? new anvil_val_s{v} : nullptr;
}
static anvil_type_t make_type(Type *t) {
  return t ? new anvil_type_s{t} : nullptr;
}

anvil_ctx_t anvil_ctx_create(void) {
  return safe<anvil_ctx_t>(nullptr, [] { return new anvil_ctx_s{}; });
}

void anvil_ctx_destroy(anvil_ctx_t ctx) { delete ctx; }

#define CTX_TYPE(name, call)                                                   \
  anvil_type_t anvil_ctx_get_##name(anvil_ctx_t ctx) {                         \
    return safe<anvil_type_t>(nullptr, [&] {                                   \
      if (!ctx)                                                                \
        throw std::runtime_error("null context");                              \
      return make_type(ctx->ctx.call);                                         \
    });                                                                        \
  }

CTX_TYPE(void_ty, getVoidTy())
CTX_TYPE(i1_ty, getInt1Ty())
CTX_TYPE(i8_ty, getInt8Ty())
CTX_TYPE(i16_ty, getInt16Ty())
CTX_TYPE(i32_ty, getInt32Ty())
CTX_TYPE(i64_ty, getInt64Ty())
CTX_TYPE(half_ty, getHalfTy())
CTX_TYPE(float_ty, getFloatTy())
CTX_TYPE(double_ty, getDoubleTy())
CTX_TYPE(label_ty, getLabelTy())

#undef CTX_TYPE

anvil_type_t anvil_ctx_get_ptr_ty(anvil_ctx_t ctx, anvil_type_t elem) {
  return safe<anvil_type_t>(nullptr, [&] {
    if (!ctx || !elem)
      throw std::runtime_error("null argument");
    return make_type(ctx->ctx.getPointerTy(elem->type));
  });
}

anvil_type_t anvil_ctx_get_array_ty(anvil_ctx_t ctx, anvil_type_t elem,
                                    unsigned count) {
  return safe<anvil_type_t>(nullptr, [&] {
    if (!ctx || !elem)
      throw std::runtime_error("null argument");
    return make_type(ctx->ctx.getArrayTy(elem->type, count));
  });
}

anvil_type_t anvil_ctx_get_vector_ty(anvil_ctx_t ctx, anvil_type_t elem,
                                     unsigned count) {
  return safe<anvil_type_t>(nullptr, [&] {
    if (!ctx || !elem)
      throw std::runtime_error("null argument");
    return make_type(ctx->ctx.getVectorTy(elem->type, count));
  });
}

anvil_type_t anvil_ctx_get_fn_ty(anvil_ctx_t ctx, anvil_type_t ret,
                                 const anvil_type_t *params,
                                 unsigned num_params) {
  return safe<anvil_type_t>(nullptr, [&] {
    if (!ctx || !ret)
      throw std::runtime_error("null argument");
    std::vector<Type *> ps;
    ps.reserve(num_params);
    for (unsigned i = 0; i < num_params; ++i) {
      if (!params[i])
        throw std::runtime_error("null param type");
      ps.push_back(params[i]->type);
    }
    return make_type(ctx->ctx.getFunctionTy(ret->type, ps));
  });
}

anvil_type_t anvil_ctx_get_struct_ty(anvil_ctx_t ctx, const anvil_type_t *elems,
                                     unsigned num_elems) {
  return safe<anvil_type_t>(nullptr, [&] {
    if (!ctx)
      throw std::runtime_error("null context");
    std::vector<Type *> es;
    es.reserve(num_elems);
    for (unsigned i = 0; i < num_elems; ++i) {
      if (!elems[i])
        throw std::runtime_error("null element type");
      es.push_back(elems[i]->type);
    }
    return make_type(ctx->ctx.getStructTy(es));
  });
}

anvil_val_t anvil_ctx_const_int(anvil_ctx_t ctx, anvil_type_t ty, int64_t val) {
  return safe<anvil_val_t>(nullptr, [&] {
    if (!ctx || !ty)
      throw std::runtime_error("null argument");
    auto *it = dynamic_cast<IntegerType *>(ty->type);
    if (!it)
      throw std::runtime_error("type is not an integer type");
    return make_val(ctx->ctx.getConstantInt(it, val));
  });
}

anvil_val_t anvil_ctx_const_fp(anvil_ctx_t ctx, anvil_type_t ty, double val) {
  return safe<anvil_val_t>(nullptr, [&] {
    if (!ctx || !ty)
      throw std::runtime_error("null argument");
    auto *ft = dynamic_cast<FloatType *>(ty->type);
    if (!ft)
      throw std::runtime_error("type is not a float type");
    return make_val(ctx->ctx.getConstantFP(ft, val));
  });
}

anvil_val_t anvil_ctx_const_null(anvil_ctx_t ctx, anvil_type_t ptr_ty) {
  return safe<anvil_val_t>(nullptr, [&] {
    if (!ctx || !ptr_ty)
      throw std::runtime_error("null argument");
    auto *pt = dynamic_cast<PointerType *>(ptr_ty->type);
    if (!pt)
      throw std::runtime_error("type is not a pointer type");
    return make_val(ctx->ctx.getConstantNull(pt));
  });
}

anvil_mod_t anvil_mod_create(const char *name) {
  return safe<anvil_mod_t>(nullptr, [&] {
    if (!name)
      throw std::runtime_error("null module name");
    return new anvil_mod_s(std::make_unique<Module>(name));
  });
}

void anvil_mod_destroy(anvil_mod_t mod) { delete mod; }

void anvil_mod_set_target_triple(anvil_mod_t mod, const char *triple) {
  safe<int>(0, [&] {
    if (!mod || !triple)
      throw std::runtime_error("null argument");
    mod->mod->setTargetTriple(triple);
    return 0;
  });
}

void anvil_mod_set_data_layout(anvil_mod_t mod, const char *layout) {
  safe<int>(0, [&] {
    if (!mod || !layout)
      throw std::runtime_error("null argument");
    mod->mod->setDataLayout(layout);
    return 0;
  });
}

void anvil_mod_add_fn(anvil_mod_t mod, anvil_fn_t fn) {
  safe<int>(0, [&] {
    if (!mod || !fn)
      throw std::runtime_error("null argument");
    if (!fn->fn)
      throw std::runtime_error("function already transferred");
    mod->mod->addFunction(std::move(fn->fn));
    return 0;
  });
}

anvil_val_t anvil_mod_add_string(anvil_mod_t mod, anvil_ctx_t ctx,
                                 const char *str) {
  return safe<anvil_val_t>(nullptr, [&] {
    if (!mod || !ctx || !str)
      throw std::runtime_error("null argument");
    return make_val(mod->mod->createStringLiteral(ctx->ctx, str));
  });
}

char *anvil_mod_print_to_string(anvil_mod_t mod) {
  return safe<char *>(nullptr, [&] {
    if (!mod)
      throw std::runtime_error("null module");
    std::ostringstream oss;
    mod->mod->print(oss);
    const std::string &s = oss.str();
    char *out = static_cast<char *>(std::malloc(s.size() + 1));
    if (!out)
      throw std::runtime_error("out of memory");
    std::memcpy(out, s.c_str(), s.size() + 1);
    return out;
  });
}

int anvil_mod_print_to_file(anvil_mod_t mod, const char *path) {
  return safe<int>(-1, [&] {
    if (!mod || !path)
      throw std::runtime_error("null argument");
    std::ofstream f(path);
    if (!f)
      throw std::runtime_error(std::string("cannot open file: ") + path);
    mod->mod->print(f);
    return 0;
  });
}

anvil_fn_t anvil_fn_create(anvil_type_t fn_ty, const char *name) {
  return safe<anvil_fn_t>(nullptr, [&] {
    if (!fn_ty || !name)
      throw std::runtime_error("null argument");
    auto *ft = dynamic_cast<FunctionType *>(fn_ty->type);
    if (!ft)
      throw std::runtime_error("type is not a function type");
    return new anvil_fn_s(std::make_unique<Function>(ft, name));
  });
}

void anvil_fn_destroy(anvil_fn_t fn) { delete fn; }

void anvil_fn_add_bb(anvil_fn_t fn, anvil_bb_t bb) {
  safe<int>(0, [&] {
    if (!fn || !bb)
      throw std::runtime_error("null argument");
    if (!fn->fn)
      throw std::runtime_error("function already transferred to module");
    if (!bb->bb)
      throw std::runtime_error("basic block already transferred");
    fn->fn->addBlock(std::move(bb->bb));
    return 0;
  });
}

unsigned anvil_fn_num_args(anvil_fn_t fn) {
  return safe<unsigned>(0, [&] {
    if (!fn || !fn->fn)
      throw std::runtime_error("null/transferred function");
    return static_cast<unsigned>(fn->fn->getArguments().size());
  });
}

anvil_val_t anvil_fn_get_arg(anvil_fn_t fn, unsigned idx) {
  return safe<anvil_val_t>(nullptr, [&] {
    if (!fn || !fn->fn)
      throw std::runtime_error("null/transferred function");
    return make_val(fn->fn->getArgument(idx));
  });
}

anvil_bb_t anvil_bb_create(anvil_ctx_t ctx, const char *name) {
  return safe<anvil_bb_t>(nullptr, [&] {
    if (!ctx)
      throw std::runtime_error("null context");
    if (!name)
      throw std::runtime_error("null block name");
    return new anvil_bb_s(
        std::make_unique<BasicBlock>(ctx->ctx.getLabelTy(), name));
  });
}

void anvil_bb_destroy(anvil_bb_t bb) { delete bb; }

anvil_builder_t anvil_builder_create(anvil_ctx_t ctx, anvil_bb_t bb) {
  return safe<anvil_builder_t>(nullptr, [&] {
    if (!ctx || !bb || !bb->bb)
      throw std::runtime_error("null argument or transferred basic block");
    return new anvil_builder_s(&ctx->ctx, bb->bb.get());
  });
}

void anvil_builder_destroy(anvil_builder_t b) { delete b; }

void anvil_builder_set_bb(anvil_builder_t b, anvil_bb_t bb) {
  safe<int>(0, [&] {
    if (!b || !bb || !bb->bb)
      throw std::runtime_error("null argument or transferred basic block");
    b->builder.SetInsertPoint(bb->bb.get());
    return 0;
  });
}

#define B (b->builder)
#define V(x) ((x)->val)
#define T(x) ((x)->type)

#define BUILD1(name, method, arg)                                              \
  anvil_val_t anvil_build_##name(anvil_builder_t b, anvil_val_t arg) {         \
    return safe<anvil_val_t>(nullptr, [&] {                                    \
      if (!b || !arg)                                                          \
        throw std::runtime_error("null argument");                             \
      return make_val(B.method(V(arg)));                                       \
    });                                                                        \
  }

#define BUILD2(name, method)                                                   \
  anvil_val_t anvil_build_##name(anvil_builder_t b, anvil_val_t lhs,           \
                                 anvil_val_t rhs) {                            \
    return safe<anvil_val_t>(nullptr, [&] {                                    \
      if (!b || !lhs || !rhs)                                                  \
        throw std::runtime_error("null argument");                             \
      return make_val(B.method(V(lhs), V(rhs)));                               \
    });                                                                        \
  }

anvil_val_t anvil_build_br(anvil_builder_t b, anvil_bb_t dest) {
  return safe<anvil_val_t>(nullptr, [&] {
    if (!b || !dest || !dest->bb)
      throw std::runtime_error("null argument or transferred block");
    return make_val(B.CreateBr(dest->bb.get()));
  });
}

anvil_val_t anvil_build_cond_br(anvil_builder_t b, anvil_val_t cond,
                                anvil_bb_t then_bb, anvil_bb_t else_bb) {
  return safe<anvil_val_t>(nullptr, [&] {
    if (!b || !cond || !then_bb || !else_bb || !then_bb->bb || !else_bb->bb)
      throw std::runtime_error("null argument or transferred block");
    return make_val(
        B.CreateCondBr(V(cond), then_bb->bb.get(), else_bb->bb.get()));
  });
}

anvil_val_t anvil_build_ret(anvil_builder_t b, anvil_val_t val) {
  return safe<anvil_val_t>(nullptr, [&] {
    if (!b || !val)
      throw std::runtime_error("null argument");
    return make_val(B.CreateRet(V(val)));
  });
}

anvil_val_t anvil_build_ret_void(anvil_builder_t b) {
  return safe<anvil_val_t>(nullptr, [&] {
    if (!b)
      throw std::runtime_error("null builder");
    return make_val(B.CreateRetVoid());
  });
}

anvil_val_t anvil_build_unreachable(anvil_builder_t b) {
  return safe<anvil_val_t>(nullptr, [&] {
    if (!b)
      throw std::runtime_error("null builder");
    return make_val(B.CreateUnreachable());
  });
}

BUILD2(add, CreateAdd)
BUILD2(sub, CreateSub)
BUILD2(mul, CreateMul)
BUILD2(udiv, CreateUDiv)
BUILD2(sdiv, CreateSDiv)
BUILD2(urem, CreateURem)
BUILD2(srem, CreateSRem)
BUILD2(shl, CreateShl)
BUILD2(lshr, CreateLShr)
BUILD2(ashr, CreateAShr)
BUILD2(and, CreateAnd)
BUILD2(or, CreateOr)
BUILD2(xor, CreateXor)

BUILD2(fadd, CreateFAdd)
BUILD2(fsub, CreateFSub)
BUILD2(fmul, CreateFMul)
BUILD2(fdiv, CreateFDiv)
BUILD2(frem, CreateFRem)
BUILD1(fneg, CreateFNeg, val)

anvil_val_t anvil_build_alloca(anvil_builder_t b, anvil_type_t ty) {
  return safe<anvil_val_t>(nullptr, [&] {
    if (!b || !ty)
      throw std::runtime_error("null argument");
    return make_val(B.CreateAlloca(T(ty)));
  });
}

anvil_val_t anvil_build_load(anvil_builder_t b, anvil_type_t ty,
                             anvil_val_t ptr) {
  return safe<anvil_val_t>(nullptr, [&] {
    if (!b || !ty || !ptr)
      throw std::runtime_error("null argument");
    return make_val(B.CreateLoad(T(ty), V(ptr)));
  });
}

anvil_val_t anvil_build_store(anvil_builder_t b, anvil_val_t val,
                              anvil_val_t ptr) {
  return safe<anvil_val_t>(nullptr, [&] {
    if (!b || !val || !ptr)
      throw std::runtime_error("null argument");
    return make_val(B.CreateStore(V(val), V(ptr)));
  });
}

anvil_val_t anvil_build_gep(anvil_builder_t b, anvil_val_t base,
                            const anvil_val_t *indices, unsigned num_indices) {
  return safe<anvil_val_t>(nullptr, [&] {
    if (!b || !base)
      throw std::runtime_error("null argument");
    std::vector<Value *> idxs;
    idxs.reserve(num_indices);
    for (unsigned i = 0; i < num_indices; ++i) {
      if (!indices[i])
        throw std::runtime_error("null index");
      idxs.push_back(V(indices[i]));
    }
    return make_val(B.CreateGEP(V(base), idxs));
  });
}

#define BUILD_CAST(name, method)                                               \
  anvil_val_t anvil_build_##name(anvil_builder_t b, anvil_val_t v,             \
                                 anvil_type_t ty) {                            \
    return safe<anvil_val_t>(nullptr, [&] {                                    \
      if (!b || !v || !ty)                                                     \
        throw std::runtime_error("null argument");                             \
      return make_val(B.method(V(v), T(ty)));                                  \
    });                                                                        \
  }

BUILD_CAST(trunc, CreateTrunc)
BUILD_CAST(zext, CreateZExt)
BUILD_CAST(sext, CreateSExt)
BUILD_CAST(fptrunc, CreateFPTrunc)
BUILD_CAST(fpext, CreateFPExt)
BUILD_CAST(fptoui, CreateFPToUI)
BUILD_CAST(fptosi, CreateFPToSI)
BUILD_CAST(uitofp, CreateUIToFP)
BUILD_CAST(sitofp, CreateSIToFP)
BUILD_CAST(ptrtoint, CreatePtrToInt)
BUILD_CAST(inttoptr, CreateIntToPtr)
BUILD_CAST(bitcast, CreateBitCast)

static Instruction::ICmpPredicate translate_pred(anvil_icmp_pred_t p) {
  switch (p) {
  case ANVIL_ICMP_EQ:
    return Instruction::ICmpPredicate::EQ;
  case ANVIL_ICMP_NE:
    return Instruction::ICmpPredicate::NE;
  case ANVIL_ICMP_UGT:
    return Instruction::ICmpPredicate::UGT;
  case ANVIL_ICMP_UGE:
    return Instruction::ICmpPredicate::UGE;
  case ANVIL_ICMP_ULT:
    return Instruction::ICmpPredicate::ULT;
  case ANVIL_ICMP_ULE:
    return Instruction::ICmpPredicate::ULE;
  case ANVIL_ICMP_SGT:
    return Instruction::ICmpPredicate::SGT;
  case ANVIL_ICMP_SGE:
    return Instruction::ICmpPredicate::SGE;
  case ANVIL_ICMP_SLT:
    return Instruction::ICmpPredicate::SLT;
  case ANVIL_ICMP_SLE:
    return Instruction::ICmpPredicate::SLE;
  default:
    throw std::runtime_error("unknown predicate");
  }
}

anvil_val_t anvil_build_icmp(anvil_builder_t b, anvil_icmp_pred_t pred,
                             anvil_val_t lhs, anvil_val_t rhs) {
  return safe<anvil_val_t>(nullptr, [&] {
    if (!b || !lhs || !rhs)
      throw std::runtime_error("null argument");
    return make_val(B.CreateICmp(translate_pred(pred), V(lhs), V(rhs)));
  });
}

anvil_val_t anvil_build_fcmp(anvil_builder_t b, anvil_icmp_pred_t pred,
                             anvil_val_t lhs, anvil_val_t rhs) {
  return safe<anvil_val_t>(nullptr, [&] {
    if (!b || !lhs || !rhs)
      throw std::runtime_error("null argument");
    return make_val(B.CreateFCmp(V(lhs), V(rhs), translate_pred(pred)));
  });
}

anvil_val_t anvil_build_select(anvil_builder_t b, anvil_val_t cond,
                               anvil_val_t true_val, anvil_val_t false_val) {
  return safe<anvil_val_t>(nullptr, [&] {
    if (!b || !cond || !true_val || !false_val)
      throw std::runtime_error("null argument");
    return make_val(B.CreateSelect(V(cond), V(true_val), V(false_val)));
  });
}

anvil_val_t anvil_build_call(anvil_builder_t b, anvil_type_t ret_ty,
                             anvil_val_t callee, const anvil_val_t *args,
                             unsigned num_args) {
  return safe<anvil_val_t>(nullptr, [&] {
    if (!b || !ret_ty || !callee)
      throw std::runtime_error("null argument");
    std::vector<Value *> as;
    as.reserve(num_args);
    for (unsigned i = 0; i < num_args; ++i) {
      if (!args[i])
        throw std::runtime_error("null argument value");
      as.push_back(V(args[i]));
    }
    return make_val(B.CreateCall(T(ret_ty), V(callee), as));
  });
}

anvil_val_t anvil_build_phi(anvil_builder_t b, anvil_type_t ty) {
  return safe<anvil_val_t>(nullptr, [&] {
    if (!b || !ty)
      throw std::runtime_error("null argument");
    return make_val(B.CreatePHI(T(ty)));
  });
}

void anvil_phi_add_incoming(anvil_val_t phi, anvil_val_t val, anvil_bb_t bb) {
  safe<int>(0, [&] {
    if (!phi || !val || !bb || !bb->bb)
      throw std::runtime_error("null argument or transferred block");
    auto *inst = dynamic_cast<Instruction *>(phi->val);
    if (!inst)
      throw std::runtime_error("val is not an instruction");
    inst->addIncoming(V(val), bb->bb.get());
    return 0;
  });
}

void anvil_val_set_name(anvil_val_t val, const char *name) {
  safe<int>(0, [&] {
    if (!val || !name)
      throw std::runtime_error("null argument");
    val->val->setName(name);
    return 0;
  });
}

#undef BUILD1
#undef BUILD2
#undef BUILD_CAST
#undef B
#undef V
#undef T
