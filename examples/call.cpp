#include <anvil.hpp>
#include <fstream>

using namespace anvil::ir;

int main() {
  Context ctx;
  Module mod("call");

  Type *i32 = ctx.getInt32Ty();

  // Define: i32 @add(i32 %0, i32 %1)
  FunctionType *addTy = ctx.getFunctionTy(i32, {i32, i32});
  auto addFn = std::make_unique<Function>(addTy, "add");
  Function *addFnRaw = addFn.get();

  {
    auto entry =
        std::make_unique<BasicBlock>(ctx.getLabelTy(), "entry", addFnRaw);
    IRBuilder b(&ctx, entry.get());

    Argument *a = addFnRaw->getArgument(0);
    Argument *b0 = addFnRaw->getArgument(1);
    a->setName("a");
    b0->setName("b");

    Instruction *sum = b.CreateAdd(a, b0);
    b.CreateRet(sum);

    addFn->addBlock(std::move(entry));
  }
  mod.addFunction(std::move(addFn));

  // Define: i32 @main()
  FunctionType *mainTy = ctx.getFunctionTy(i32, {});
  auto mainFn = std::make_unique<Function>(mainTy, "main");

  auto entry =
      std::make_unique<BasicBlock>(ctx.getLabelTy(), "entry", mainFn.get());
  auto x_then =
      std::make_unique<BasicBlock>(ctx.getLabelTy(), "x.then", mainFn.get());
  auto x_else =
      std::make_unique<BasicBlock>(ctx.getLabelTy(), "x.else", mainFn.get());
  auto x_merge =
      std::make_unique<BasicBlock>(ctx.getLabelTy(), "x.merge", mainFn.get());
  auto y_then =
      std::make_unique<BasicBlock>(ctx.getLabelTy(), "y.then", mainFn.get());
  auto y_else =
      std::make_unique<BasicBlock>(ctx.getLabelTy(), "y.else", mainFn.get());
  auto y_merge =
      std::make_unique<BasicBlock>(ctx.getLabelTy(), "y.merge", mainFn.get());

  auto *c1 = ctx.getConstantInt(ctx.getInt32Ty(), 1);
  auto *c2 = ctx.getConstantInt(ctx.getInt32Ty(), 2);
  auto *c10 = ctx.getConstantInt(ctx.getInt32Ty(), 10);
  auto *c20 = ctx.getConstantInt(ctx.getInt32Ty(), 20);

  IRBuilder builder(&ctx, entry.get());
  Instruction *cmpX =
      builder.CreateICmp(Instruction::ICmpPredicate::EQ, c1, c1);
  builder.CreateCondBr(cmpX, x_then.get(), x_else.get());

  builder.SetInsertPoint(x_then.get());
  Instruction *cmpY =
      builder.CreateICmp(Instruction::ICmpPredicate::EQ, c2, c2);
  builder.CreateCondBr(cmpY, y_then.get(), y_else.get());

  builder.SetInsertPoint(y_then.get());
  Instruction *callAdd1 = builder.CreateCall(i32, addFnRaw, {c1, c2});
  builder.CreateBr(y_merge.get());

  builder.SetInsertPoint(y_else.get());
  Instruction *callAdd2 = builder.CreateCall(i32, addFnRaw, {c1, c10});
  builder.CreateBr(y_merge.get());

  builder.SetInsertPoint(y_merge.get());
  Instruction *yPhi = builder.CreatePHI(i32);
  yPhi->addIncoming(callAdd1, y_then.get());
  yPhi->addIncoming(callAdd2, y_else.get());
  builder.CreateBr(x_merge.get());

  builder.SetInsertPoint(x_else.get());
  Instruction *callAdd3 = builder.CreateCall(i32, addFnRaw, {c2, c20});
  builder.CreateBr(x_merge.get());

  builder.SetInsertPoint(x_merge.get());
  Instruction *xPhi = builder.CreatePHI(i32);
  xPhi->addIncoming(yPhi, y_merge.get());
  xPhi->addIncoming(callAdd3, x_else.get());
  builder.CreateRet(xPhi);

  mainFn->addBlock(std::move(entry));
  mainFn->addBlock(std::move(x_then));
  mainFn->addBlock(std::move(x_else));
  mainFn->addBlock(std::move(y_then));
  mainFn->addBlock(std::move(y_else));
  mainFn->addBlock(std::move(y_merge));
  mainFn->addBlock(std::move(x_merge));

  mod.addFunction(std::move(mainFn));

  std::ofstream out(mod.name() + ".ll");
  mod.print(out);
}
