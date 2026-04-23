# Anvil

Anvil is a header-only C++ library for generating LLVM IR directly, without linking against LLVM itself.

It gives you low-level, explicit control over LLVM IR construction while staying lightweight and dependency-free. Anvil is designed for compiler authors, language experimenters, and anyone who wants to understand LLVM IR by building it themselves.

## Hello, world!

LLVM IR doesn’t have a runtime by default, so the canonical “Hello, World” is a function that simply returns 0.

```c++
#include <anvil.hpp> 
#include <fstream>

using namespace anvil::ir;

int main()
{
    Context ctx;
    Module mod("hello_world");

    auto mainFn = std::make_unique<Function>(ctx.getInt32Ty(), "main");
    auto entry = std::make_unique<BasicBlock>("entry");

    IRBuilder builder(&ctx, entry.get());

    auto zero = std::make_unique<ConstantInt>(ctx.getInt32Ty(), 0);
    builder.CreateRet(zero.get());

    mainFn->addBlock(std::move(entry));
    mod.addFunction(std::move(mainFn));

    std::ofstream out(mod.name() + ".ll");
    mod.print(out);
}
```

This will generate a basic main entry point
```llvm
; ModuleID = 'hello_world'

define i32 @main() {
entry:
  ret i32 0
}
```

# Scaling

Anvil scales cleanly from trivial IR to full SSA-heavy control flow:
- Conditional branches
- Nested `if / else`
- Loops with PHI nodes
- Manual SSA construction
- Explicit block wiring

Anvil never guesses.

If something exists in the IR, **you created it**.