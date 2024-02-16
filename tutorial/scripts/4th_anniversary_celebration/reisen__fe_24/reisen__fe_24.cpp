/*
MIT License

Copyright (c) 2024 Reisen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "avz.h"
#include "dsl/shorthand.h"
#include "mod/mod.h"
#include "SetZombiesPlus/main.h"

using namespace std;

class {
public:
    AMainObject* operator->() {
        return AGetMainObject();
    }
} mo;

// PropFilter(&AZombie::Hp, greater(), 3000)
template <typename T>
AAliveFilter<T> PropFilter(auto (T::* prop)(), const auto& op, auto value) {
    return AAliveFilter<T>([=](T* x){ return op((x->*prop)(), value); });
}

// PropFilter(&AZombie::Type, AGIGA_GARGANTUAR)
template <typename T>
AAliveFilter<T> PropFilter(auto (T::* prop)(), auto value) {
    return PropFilter(prop, equal_to(), value);
}

class : AStateHook {
private:
    int cnt;

    void _BeforeScript() override {
        cnt = 0;
        OnWave(1_20) [this] {
            for (auto& z : PropFilter(&AZombie::ExistTime, 0))
                if (z.Type() == AGIGA_GARGANTUAR)
                    cnt++;
        };
    }

public:
    operator int() {
        return cnt;
    }
} gigaCount;

int GetCobReadyTime(int nCobs = 1) {
    auto cobs = aCobManager.GetRecoverList();
    ranges::sort(cobs, [](auto a, auto b){ return a.recoverTime < b.recoverTime; });
    return cobs[nCobs - 1].recoverTime + (aFieldInfo.isRoof ? 387 : 373);
}

// ----------------------------------------------------------------

unordered_map<string, ATimeline> states;

ATimeline Transition(int wl, string notActivate, string activate, string noGiga, string final) {
    return At(wl - 200) Do {
        int currentWave = ANowWave();
        while (ANowTime(currentWave) != wl - 200)
            currentWave--;
        if (ANowTime(currentWave + 1) != INT_MIN) {
            int wl_ = ANowTime(currentWave) - ANowTime(currentWave + 1);
            if (wl_ != wl) {
                AGetInternalLogger()->Error("第 # 波提前刷新，预期波长 >=#cs，实际波长 #cs", currentWave, wl, wl_);
                return;
            }
        }

        const string* nextState;
        if (ANowTime(currentWave + 1) == INT_MIN)
            nextState = &notActivate;
        else if (currentWave % 10 == 8)
            nextState = &final;
        else if (gigaCount >= 50 && !noGiga.empty())
            nextState = &noGiga;
        else
            nextState = &activate;

        if (states.contains(*nextState)) {
            if (nextState == &notActivate)
                OnWave(currentWave) states[*nextState];
            else
                OnWave(currentWave + 1) states[*nextState];
        } else {
            string stateName = (nextState->empty() ? "null" : *nextState);
            AGetInternalLogger()->Error("状态 " + stateName + " 不存在");
        }
    };
}

//#define TEST

#ifdef TEST
constexpr auto RELOAD_MODE = AReloadMode::MAIN_UI_OR_FIGHT_UI;
constexpr float GAME_SPEED = 10.0;
constexpr int SELECT_CARDS_INTERVAL = 0;
constexpr bool SKIP_TICK = true;
#elifdef DEMO
constexpr auto RELOAD_MODE = AReloadMode::MAIN_UI;
constexpr float GAME_SPEED = 1.0;
constexpr int SELECT_CARDS_INTERVAL = 17;
constexpr bool SKIP_TICK = false;
#else
constexpr auto RELOAD_MODE = AReloadMode::MAIN_UI_OR_FIGHT_UI;
constexpr float GAME_SPEED = 5.0;
constexpr int SELECT_CARDS_INTERVAL = 0;
constexpr bool SKIP_TICK = false;
#endif

#ifdef TEST
ALogger<AConsole> logger;

int completedFlags = 0;
int loadCount = 0;

AOnExitFight(
    completedFlags++;
    if (completedFlags % 50 == 0)
        logger.Info("已完成 #f  当前阳光: #", completedFlags * 2, mo->Sun());
    if (AGetPvzBase()->GameUi() == AAsm::ZOMBIES_WON) {
        ABackToMain(false);
        AEnterGame(14);
    }
    if (AGetPvzBase()->GameUi() != AAsm::LEVEL_INTRO) {
        loadCount++;
        logger.Error("第 # 次 SL", loadCount);
    }
)
#endif

void AScript() {
    ASetReloadMode(RELOAD_MODE);
    AConnect('Q', []{ ATerminate(); });
    ASetGameSpeed(GAME_SPEED);
    EnableModsScoped(SaveDataReadOnly, RemoveFog, DisableItemDrop);
    if (AMRef<int>(0x6a9ec0, 0x768, 0x160, 0x6c) == 1000)
        ASetZombies({AZOMBIE, APOLE_VAULTING_ZOMBIE, ADANCING_ZOMBIE, ADOLPHIN_RIDER_ZOMBIE, ABALLOON_ZOMBIE, APOGO_ZOMBIE, ABUNGEE_ZOMBIE, ALADDER_ZOMBIE, ACATAPULT_ZOMBIE, AGARGANTUAR, AGIGA_GARGANTUAR}, ASetZombieMode::INTERNAL);
    #ifdef DEMO
    ASetZombiesLimits({{9, AGIGA_GARGANTUAR, "==", 0}, {19, AGIGA_GARGANTUAR, "!=", 0}});
    ASetWavelength({ATime{5, 1127}});
    #endif
    if (AGetZombieTypeList()[AGIGA_GARGANTUAR] && AGetZombieTypeList()[AGARGANTUAR])
        ASelectCards({AICE_SHROOM, AM_ICE_SHROOM, ADOOM_SHROOM, AJALAPENO, ASQUASH, APUMPKIN, APUFF_SHROOM, ASUN_SHROOM, ASCAREDY_SHROOM, AFLOWER_POT}, SELECT_CARDS_INTERVAL);
    else
        ASelectCards({AICE_SHROOM, ADOOM_SHROOM, AJALAPENO, ASQUASH, APUMPKIN, APUFF_SHROOM, AM_PUFF_SHROOM, ASUN_SHROOM, ASCAREDY_SHROOM, AFLOWER_POT}, SELECT_CARDS_INTERVAL);
    if (SKIP_TICK)
        ASkipTick([]{ return true; });
    #ifdef TEST
    AGetInternalLogger()->SetLevel({});
    #endif

    // 轨道 a：红白关，逐波 P6

    static AFodder C2(ASCAREDY_SHROOM, AFLOWER_POT);

    // PPDDDD
    states["a1"] = {
        Transition(601, "", "a2", "b1", ""),
        At(225_cs) PP(),
        At(288_cs) PP() + DD<110>(8.75),
    };

    // PPDD
    states["a2"] = {
        Transition(601, "a3*", "a3", "", ""),
        At(359_cs) PP() + DD<107>(7.8125),
    };

    // 延迟：PPDD-PPSSSS
    states["a3*"] = {
        Transition(1202, "", "a4", "b1", ""),
        At(1002_cs) P(122556, 9),
    };

    // PPDDDD
    states["a3"] = {
        Transition(601, "", "a4", "b1", ""),
        At(249_cs) PP() + DD<110>(8.75) + DD<220>(8.75),
    };

    // PPDD
    states["a4"] = {
        Transition(601, "a5*", "a5", "", ""),
        At(359_cs) PP() + DD<107>(7.8125),
    };

    // 延迟：PPDD-cccPPI
    states["a5*"] = {
        Transition(1191, "", "a6", "", ""),
        At(891_cs) Card(AICE_SHROOM, 1, 9) + C.TriggerBy(AGIGA_GARGANTUAR)(100, 256),
        At(991_cs) PP(),
    };

    // cccPPI
    states["a5"] = {
        Transition(601, "a6*", "a6", "", ""),
        At(218_cs) Card(AICE_SHROOM, 2, 9) + C.TriggerBy(AGIGA_GARGANTUAR & PREV_WAVES)(1, 56),
        At(281_cs) C2.TriggerBy(AGIGA_GARGANTUAR & PREV_WAVES)(1, 1),
        At(318_cs) PP(),
    };

    // 延迟：cccPPI-PPI
    states["a6*"] = {
        Transition(1127, "", "a7", "", ""),
        At(927_cs) I(1, 9) + PP(),
    };

    // PPaI
    states["a6"] = {
        Transition(601, "", "a7", "", ""),
        At(318_cs) PP(),
        At(319_cs) I(1, 9),
        At(400_cs) a(5, 9)
    };

    // PPSSDD
    states["a7"] = {
        Transition(601, "", "a8", "b1", ""),
        At(232_cs) P(1256, 8.75),
        At(240_cs) Do {
            for (auto& z : aAliveZombieFilter)
                if (z.Type() == APOGO_ZOMBIE || z.Type() == AIMP)
                    ACard(AJALAPENO, z.Row() + 1, 9);
        },
        At(359_cs) DD(8.75),
    };

    // PPDDcccc
    states["a8"] = {
        Transition(601, "a9*", "a9", "", "a9f"),
        At(359_cs) PP() + DD<107>(7.8125),
        At(360_cs) C.TriggerBy(AGIGA_GARGANTUAR)(240)
    };

    // 延迟：PPDDcccc-PPDDcccc
    states["a9*"] = {
        Transition(1202, "", "", "", "a10f"),
        At(1002_cs) PP() + DD<110>(7.8125),
        At(1111_cs) C.TriggerBy(AGIGA_GARGANTUAR)(100)
    };

    // PPDDcccc
    states["a9"] = {
        Transition(601, "", "", "", "a10f"),
        At(359_cs) PP() + DD<107>(7.8125),
        At(510_cs) C.TriggerBy(AGIGA_GARGANTUAR)(134)
    };

    // 收尾有红：PPDDDD-PP; 无红：PPDD-PP
    ATimeline finalWithGiga = {
        At(377_cs) DD(8.75) + DD<110>(8.75),
        At(1150_cs) PP()
    }, finalWithoutGiga = {
        At(377_cs) DD(8.1),
        At(620_cs) PP()
    };

    states["a9f"] = states["a10f"] = {
        At(270_cs) PP(),
        At(4_cs) Do {
            bool gigaExist = false;
            for (auto& z : PropFilter(&AZombie::AtWave, ANowWave() - 1))
                if (z.Type() == AGIGA_GARGANTUAR)
                    gigaExist = true;
            OnWave(ANowWave()) (gigaExist ? finalWithGiga : finalWithoutGiga);
        }
    };

    // 轨道 b：无红关 / 变速后，冰代奏连续 PD

    // PPDD / PPI
    states["b1"] = {
        Transition(601, "b1*", "b1", "", "b1f"),
        At(-200_cs) Do {
            if (GetCobReadyTime(4) <= 200 + 398 && GetCobReadyTime(6) <= 200 + 601 + 318)
                At(now + 200_cs + 288_cs) PP() + DD<110>(8.75);
            else
                At(now + 200_cs + 318_cs) PP() + I(2, 9);
        }
    };

    // 延迟：PPDD-PP
    states["b1*"] = {
        Transition(1202, "", "b1", "", "b1f"),
        At(1002_cs) PP()
    };

    // 收尾：PPDD-PP
    states["b1f"] = {
        At(225_cs) PP() + DD<107>(8.1875),
        At(758_cs) PP()
    };

    // 轨道 c: 单红关，Pcc/PSD | PSD/Pcc 循环（舞王撑杆不同出时不垫）

    ATimeline PPSD, PSDP;
    if (AGetZombieTypeList()[APOLE_VAULTING_ZOMBIE]) {
        PPSD = {
            At(195_cs) C.TriggerBy(ADANCING_ZOMBIE)(80, 12),
            At(379_cs) P(2, 8.75),
            At(288_cs) P(56, 9) + D<110>(5, 8.75)
        };
        PSDP = {
            At(195_cs) C.TriggerBy(ADANCING_ZOMBIE)(80, 56),
            At(379_cs) P(5, 8.75),
            At(288_cs) P(12, 9) + D<110>(1, 8.75)
        };
    } else {
        PPSD = {
            At(318_cs) P(2, 8.75),
            At(288_cs) P(56, 9) + D<110>(5, 8.75)
        };
        PSDP = {
            At(318_cs) P(5, 8.75),
            At(288_cs) P(12, 9) + D<110>(1, 8.75)
        };
    }

    states["c1"] = {
        Transition(601, "", "c2", "c2t", "c2f"),
        PPSD
    };

    states["c1t"] = {
        Transition(601, "", "b1", "", "b1f"),
        PPSD
    };

    states["c1*"] = {
        Transition(601, "c2*", "c2", "c2t", ""),
        PPSD
    };

    states["c1f"] = {
        PPSD,
        At(401_cs) Do {
            if (ANowTime(ANowWave() + 1) == INT_MIN)
                At(now + 200_cs) PSDP;
        }
    };

    states["c2"] = {
        Transition(601, "", "c1", "c1t", "c1f"),
        PSDP
    };

    states["c2t"] = {
        Transition(601, "", "b1", "", "b1f"),
        PSDP
    };

    states["c2*"] = {
        Transition(1202, "", "c1", "c1t", ""),
        At(1002_cs) P(1225, 9)
    };

    states["c2f"] = {
        PSDP,
        At(401_cs) Do {
            if (ANowTime(ANowWave() + 1) == INT_MIN)
                At(now + 200_cs) PPSD;
        }
    };

    AMaidCheats::Dancing();
    At(20_wave) Do { AMaidCheats::Stop(); };
    aPlantFixer.Start(APUMPKIN, {}, 4000 / 3);
    if (AGetZombieTypeList()[AGIGA_GARGANTUAR] && AGetZombieTypeList()[AGARGANTUAR])
        OnWave(1, 10) states["a1"];
    else if (AGetZombieTypeList()[AGIGA_GARGANTUAR])
        OnWave(1, 10) states["c1"] | states["c1*"];
    else
        OnWave(1, 10) states["b1"];
    At(20_wave) {
        At(250_cs) P(4, 7.5),
        At(341_cs) PP() + DD<110>(8.75) + DD<220>(8.75),
        At(1114_cs) PP()
    };
}
