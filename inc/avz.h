/*
 * @Coding: utf-8
 * @Author: yuchenxi0_0 and vector-wlc
 * @Date: 2020-02-06 10:22:46
 * @Description: High-precision PvZ TAS Frameworks : Assembly vs. Zombies !
 */

#ifndef __AVZ_H__
#define __AVZ_H__

#include "libavz.h"

void AScript();

#define ACoScript()               \
    __ARealScript();              \
    void AScript()                \
    {                             \
        ACoLaunch(__ARealScript); \
    }                             \
    auto __ARealScript()->decltype(__ARealScript())

#define ___AConCat(a, b) a##b
#define __AConCat(a, b) ___AConCat(a, b)
#define AAddStateHook(FuncName, ...)            \
    class : public AStateHook {                 \
        protected :                             \
            virtual void FuncName() __VA_ARGS__ \
    } __AConCat(__aStateHook, __COUNTER__)

#define AOnBeforeScriptCodeBlock(...) AAddStateHook(_BeforeScript, __VA_ARGS__)
#define AOnBeforeScript(...) AAddStateHook(_BeforeScript, { __VA_ARGS__; })
#define AOnAfterScriptCodeBlock(...) AAddStateHook(_AfterScript, __VA_ARGS__)
#define AOnAfterScript(...) AAddStateHook(_AfterScript, { __VA_ARGS__; })
#define AOnEnterFightCodeBlock(...) AAddStateHook(_EnterFight, __VA_ARGS__)
#define AOnEnterFightBeforeScript(...) AAddStateHook(_EnterFight, { __VA_ARGS__; })
#define AOnExitFightCodeBlock(...) AAddStateHook(_ExitFight, __VA_ARGS__)
#define AOnExitFight(...) AAddStateHook(_ExitFight, { __VA_ARGS__; })
#define AOnAfterInjectCodeBlock(...) AAddStateHook(_AfterInject, __VA_ARGS__)
#define AOnAfterInject(...) AAddStateHook(_AfterInject, { __VA_ARGS__; })

#endif