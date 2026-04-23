#include <anvil.hpp>
#include <fstream>

using namespace anvil::ir;

int main()
{
    Context ctx;
    Module mod("complex_pattern");

    auto funcTy = ctx.getFunctionTy(ctx.getInt32Ty(), {});
    auto mainFn = std::make_unique<Function>(funcTy, "main");

    auto entry     = std::make_unique<BasicBlock>(ctx.getLabelTy(), "entry",     mainFn.get());
    auto loop_cond = std::make_unique<BasicBlock>(ctx.getLabelTy(), "loop.cond", mainFn.get());
    auto loop_body = std::make_unique<BasicBlock>(ctx.getLabelTy(), "loop.body", mainFn.get());
    auto loop_end  = std::make_unique<BasicBlock>(ctx.getLabelTy(), "loop.end",  mainFn.get());
    auto if_then   = std::make_unique<BasicBlock>(ctx.getLabelTy(), "if.then",   mainFn.get());
    auto if_else   = std::make_unique<BasicBlock>(ctx.getLabelTy(), "if.else",   mainFn.get());
    auto if_merge  = std::make_unique<BasicBlock>(ctx.getLabelTy(), "if.merge",  mainFn.get());

    // All constants owned by ctx
    auto *zero = ctx.getConstantInt(ctx.getInt32Ty(), 0);
    auto *one  = ctx.getConstantInt(ctx.getInt32Ty(), 1);
    auto *five = ctx.getConstantInt(ctx.getInt32Ty(), 5);
    auto *ten  = ctx.getConstantInt(ctx.getInt32Ty(), 10);

    IRBuilder builder(&ctx, entry.get());
    builder.CreateBr(loop_cond.get());

    builder.SetInsertPoint(loop_cond.get());
    Instruction *iPhi = builder.CreatePHI(ctx.getInt32Ty());
    iPhi->addIncoming(zero, entry.get());
    Instruction *cmpLoop = builder.CreateICmp(Instruction::ICmpPredicate::SLT, iPhi, five);
    builder.CreateCondBr(cmpLoop, loop_body.get(), loop_end.get());

    builder.SetInsertPoint(loop_body.get());
    Instruction *isZero = builder.CreateICmp(Instruction::ICmpPredicate::EQ, iPhi, zero);
    builder.CreateCondBr(isZero, if_then.get(), if_else.get());

    builder.SetInsertPoint(if_then.get());
    Instruction *addThen = builder.CreateAdd(iPhi, ten);
    builder.CreateBr(if_merge.get());

    builder.SetInsertPoint(if_else.get());
    Instruction *addElse = builder.CreateAdd(iPhi, five);
    builder.CreateBr(if_merge.get());

    builder.SetInsertPoint(if_merge.get());
    Instruction *phiIf = builder.CreatePHI(ctx.getInt32Ty());
    phiIf->addIncoming(addThen, if_then.get());
    phiIf->addIncoming(addElse, if_else.get());
    Instruction *inc = builder.CreateAdd(iPhi, one);
    iPhi->addIncoming(inc, if_merge.get());
    builder.CreateBr(loop_cond.get());

    builder.SetInsertPoint(loop_end.get());
    builder.CreateRet(iPhi);

    mainFn->addBlock(std::move(entry));
    mainFn->addBlock(std::move(loop_cond));
    mainFn->addBlock(std::move(loop_body));
    mainFn->addBlock(std::move(if_then));
    mainFn->addBlock(std::move(if_else));
    mainFn->addBlock(std::move(if_merge));
    mainFn->addBlock(std::move(loop_end));

    mod.addFunction(std::move(mainFn));

    std::ofstream out(mod.name() + ".ll");
    mod.print(out);

    return 0;
}
