#ifndef __AVZ_H__
#define __AVZ_H__

#include "libavz.h"

void AScript();

#define ACoScript()                              \
    __ARealScript();                             \
    void AScript() { ACoLaunch(__ARealScript); } \
    auto __ARealScript() -> decltype(__ARealScript())

#define ___AConCat(a, b) a##b
#define __AConCat(a, b) ___AConCat(a, b)
#define AAddStateHook(FuncName, ...)                        \
    class : public A##FuncName##Hook {                      \
        protected :                                         \
            virtual void _##FuncName() override __VA_ARGS__ \
    } __AConCat(__aStateHook, __COUNTER__);

#define AOnBeforeScript(...) AAddStateHook(BeforeScript, { __VA_ARGS__; })
#define AOnAfterScript(...) AAddStateHook(AfterScript, { __VA_ARGS__; })
#define AOnEnterFight(...) AAddStateHook(EnterFight, { __VA_ARGS__; })
#define AOnExitFight(...) AAddStateHook(ExitFight, { __VA_ARGS__; })
#define AOnAfterInject(...) AAddStateHook(AfterInject, { __VA_ARGS__; })
#define AOnBeforeTick(...) AAddStateHook(BeforeTick, { __VA_ARGS__; })
#define AOnAfterTick(...) AAddStateHook(AfterTick, { __VA_ARGS__; })
#define AOnBeforeExit(...) AAddStateHook(BeforeExit, { __VA_ARGS__; })

#endif
