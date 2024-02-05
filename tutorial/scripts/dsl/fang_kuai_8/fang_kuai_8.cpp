// 布阵码：LI4/bIyUhDSHVHA19DUxlxh0EkJ1H1lU1OJDR1A=
// 节奏：ch4 I-PPDD|I-PPDD 2030|2030

#include <avz.h>
#include <dsl/shorthand.h>

ALogger<AConsole> logger;

void AScript()
{
    ASetInternalLogger(logger);
    ASetReloadMode(AReloadMode::MAIN_UI);
    ASetZombies({AZOMBIE, APOLE_VAULTING_ZOMBIE, AZOMBONI, AJACK_IN_THE_BOX_ZOMBIE, ABALLOON_ZOMBIE, APOGO_ZOMBIE, ABUNGEE_ZOMBIE, ALADDER_ZOMBIE, ACATAPULT_ZOMBIE, AGARGANTUAR, AGIGA_GARGANTUAR});
    ASelectCards({AICE_SHROOM, AM_ICE_SHROOM, ACOFFEE_BEAN, ADOOM_SHROOM, ACHERRY_BOMB, AJALAPENO, AFLOWER_POT});
    aIceFiller.Start({{1, 3}, {3, 3}, {5, 3}});

    OnWave(1, 10) {
        At(401_cs) Trig() + P(flat, 24, 9) + D<110>(slope, 2, 7.625, flat, 4, 7.625),
    };

    // 智能在巨人多的一侧放樱桃
    At(10_wave + 281_cs) Do {
        int cnt1 = 0, cnt2 = 0;
        for (auto& zombie : aAliveZombieFilter)
            if (zombie.Type() == AGARGANTUAR || zombie.Type() == AGIGA_GARGANTUAR) {
                if (0 <= zombie.Row() && zombie.Row() <= 2)
                    cnt1++;
                if (2 <= zombie.Row() && zombie.Row() <= 4)
                    cnt2++;
            }
        int row = cnt1 > cnt2 ? 2 : 4;
        At(now + 100_cs) A(row, 9) + Shovel(row, 9);
    };

    OnWave(2_8, 11_18) {
        At(1_cs) I(),
        At(1830_cs) Trig() + P(flat, 4, 8.5, 2, 8.75) + D<200>(slope, 2, 7, flat, 4, 7),
    };

    OnWave(9, 19) {
        At(34_cs) J(3, 6) + Shovel(3, 6),
        At(35_cs) I(),
        At(1245_cs) {
            P(flat, 24, 9),
            At(214_cs) N(3, 8),
            At(214_cs + 220_cs) P(flat, 14, 8.75),
        },
        At(3500_cs) P(slope, 2, 7, flat, 4, 7),
    };

    OnWave(20) {
        At(200_cs) I(),
        At(1200_cs) P(flat, 2244, 9) + D<220>(slope, 24, 9),
    };
}
