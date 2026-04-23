#include <anvil.hpp>
#include <fstream>

using namespace anvil::ir;

int main()
{
    Context ctx;
    Module mod("nested_match");

    auto funcTy = ctx.getFunctionTy(ctx.getInt32Ty(), {});
    auto mainFn = std::make_unique<Function>(funcTy, "main");

    auto entry   = std::make_unique<BasicBlock>(ctx.getLabelTy(), "entry",   mainFn.get());
    auto x_then  = std::make_unique<BasicBlock>(ctx.getLabelTy(), "x.then",  mainFn.get());
    auto x_else  = std::make_unique<BasicBlock>(ctx.getLabelTy(), "x.else",  mainFn.get());
    auto x_merge = std::make_unique<BasicBlock>(ctx.getLabelTy(), "x.merge", mainFn.get());
    auto y_then  = std::make_unique<BasicBlock>(ctx.getLabelTy(), "y.then",  mainFn.get());
    auto y_else  = std::make_unique<BasicBlock>(ctx.getLabelTy(), "y.else",  mainFn.get());
    auto y_merge = std::make_unique<BasicBlock>(ctx.getLabelTy(), "y.merge", mainFn.get());

    auto *one = ctx.getConstantInt(ctx.getInt32Ty(), 1);
    auto *two = ctx.getConstantInt(ctx.getInt32Ty(), 2);
    auto *v10 = ctx.getConstantInt(ctx.getInt32Ty(), 10);
    auto *v20 = ctx.getConstantInt(ctx.getInt32Ty(), 20);
    auto *v30 = ctx.getConstantInt(ctx.getInt32Ty(), 30);

    IRBuilder builder(&ctx, entry.get());
    Instruction *cmpX = builder.CreateICmp(Instruction::ICmpPredicate::EQ, one, one);
    builder.CreateCondBr(cmpX, x_then.get(), x_else.get());

    builder.SetInsertPoint(x_then.get());
    Instruction *cmpY = builder.CreateICmp(Instruction::ICmpPredicate::EQ, two, two);
    builder.CreateCondBr(cmpY, y_then.get(), y_else.get());

    builder.SetInsertPoint(y_then.get());
    builder.CreateBr(y_merge.get());

    builder.SetInsertPoint(y_else.get());
    builder.CreateBr(y_merge.get());

    builder.SetInsertPoint(y_merge.get());
    Instruction *yPhi = builder.CreatePHI(ctx.getInt32Ty());
    yPhi->addIncoming(v10, y_then.get());
    yPhi->addIncoming(v20, y_else.get());
    builder.CreateBr(x_merge.get());

    builder.SetInsertPoint(x_else.get());
    builder.CreateBr(x_merge.get());

    builder.SetInsertPoint(x_merge.get());
    Instruction *xPhi = builder.CreatePHI(ctx.getInt32Ty());
    xPhi->addIncoming(yPhi, y_merge.get());
    xPhi->addIncoming(v30, x_else.get());
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
