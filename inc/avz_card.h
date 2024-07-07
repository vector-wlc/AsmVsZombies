#ifndef __AVZ_CARD_H__
#define __AVZ_CARD_H__

#include "avz_state_hook.h"
#include "avz_tick_runner.h"

struct ACardName {
    APlantType plantType;
    int row;
    float col;
};

class __ACardManager : public AOrderedBeforeScriptHook<-1>,
                       public AOrderedEnterFightHook<-1> {
public:
    void SelectCards(const std::vector<int>& lst, int selectInterval);
    int GetCardIndex(APlantType plantType);
    std::vector<APlant*> Card(const std::vector<ACardName>& lst);
    APlant* Card(int seedIndex, int row, float col);
    APlant* Card(int seedIndex, const std::vector<APosition>& lst);
    APlant* Card(APlantType plantType, int row, float col);
    APlant* Card(APlantType plantType, const std::vector<APosition>& lst);
    const std::string& GetCardName(APlantType type);
    const std::string& GetCardName(ASeed* seed);

protected:
    static std::vector<std::string> _cardName;
    std::vector<int> _selectCardVec;
    std::unordered_map<int, int> _cardNameToIndexMap;
    int _selectInterval;
    ATickRunner _tickRunner;
    bool _isSelectCards = false;
    APlant* _CardWithoutCheck(int seedIndex, int row, float col);
    APlant* _BasicCard(int seedIndex, int row, float col);
    APlant* _BasicCard(int seedIndex, const std::vector<APosition>& lst);
    bool _CheckCard(int seedIndex);
    void _ChooseSingleCard();

    virtual void _BeforeScript() override;
    virtual void _EnterFight() override;
};

inline __ACardManager __aCardManager;

// 选择一堆卡片
// ***注意：卡片名称与英文原版图鉴一致
// *** 使用示例：
// ASelectCards({
//     AICE_SHROOM,   // 寒冰菇
//     AM_ICE_SHROOM, // 模仿寒冰菇
//     ACOFFEE_BEAN,  // 咖啡豆
//     ADOOM_SHROOM,  // 毁灭菇
//     ALILY_PAD,     // 荷叶
//     ASQUASH,       // 倭瓜
//     ACHERRY_BOMB,  // 樱桃炸弹
//     ABLOVER,       // 三叶草
//     APUMPKIN,      // 南瓜头
//     APUFF_SHROOM,  // 小喷菇
// });
//
// ASelectCards({
//     AICE_SHROOM,   // 寒冰菇
//     AM_ICE_SHROOM, // 模仿寒冰菇
//     ACOFFEE_BEAN,  // 咖啡豆
//     ADOOM_SHROOM,  // 毁灭菇
//     ALILY_PAD,     // 荷叶
//     ASQUASH,       // 倭瓜
//     ACHERRY_BOMB,  // 樱桃炸弹
//     ABLOVER,       // 三叶草
//     APUMPKIN,      // 南瓜头
//     APUFF_SHROOM,  // 小喷菇
// }, 1); --------------------------- 将选卡间隔更改为 1cs 以增加选卡速度
//
// ASelectCards({
//     AICE_SHROOM,   // 寒冰菇
//     AM_ICE_SHROOM, // 模仿寒冰菇
//     ACOFFEE_BEAN,  // 咖啡豆
//     ADOOM_SHROOM,  // 毁灭菇
//     ALILY_PAD,     // 荷叶
//     ASQUASH,       // 倭瓜
//     ACHERRY_BOMB,  // 樱桃炸弹
//     ABLOVER,       // 三叶草
//     APUMPKIN,      // 南瓜头
//     APUFF_SHROOM,  // 小喷菇
// }, 0); --------------------------- 将选卡间隔更改为 0cs 以进行极速选卡，注意 0 为特殊值，效果为瞬间跳到战斗界面
//
// ASelectCards({
//     AICE_SHROOM,   // 寒冰菇
// }, 0); --------------------------- 将选卡间隔更改为 0cs 以进行极速选卡，注意 0 为特殊值，效果为瞬间跳到战斗界面，然后剩下的九个卡槽会被随机填充
inline void ASelectCards(const std::vector<int>& lst = {}, int selectInterval = 17) {
    __aCardManager.SelectCards(lst, selectInterval);
}

// 根据卡片名称得到卡片索引
// *** 注意：卡片索引从 0 开始, 不存在返回 -1
// *** 使用示例：
// AGetCardIndex(AICE_SHROOM) ---- 得到寒冰菇的卡片索引
__ANodiscard inline int AGetCardIndex(APlantType plantType) {
    return __aCardManager.GetCardIndex(plantType);
}

// 根据卡片名称得到卡片指针
// // *** 注意：不存在返回 nullptr
// *** 使用示例：
// AGetCardIndex(AICE_SHROOM) ---- 得到寒冰菇的卡片指针
__ANodiscard ASeed* AGetCardPtr(APlantType plantType);

// 用卡函数
// *** 注意：ACard 不支持根据卡槽位置用卡的多张调用形式
// ACard 一次种一张卡片会返回一个 APlant*, 如果值为 nullptr，则 ACard 种植失败，否则代表 ACard 刚种下植物的对象指针
// 一次种植多张卡片会返回 std::vector<APlant*>, 意义与上述相同
// *** 使用示例：
// ACard(1, 2, 3)---------选取第1张卡片，放在2行,3列
// ACard(1, {{2, 3}, {3, 4}})--------选取第1张卡片，优先放在2行,3列，其次放在3行,4列
// 以下用卡片名称使用 Card,卡片名称为拼音首字母，具体参考图鉴
// ACard({{CHERRY_BOMB, 2, 3}, {JALAPENO, 3, 4}})---------选取樱桃卡片，放在2行,3列，选取辣椒卡片，放在3行,4列
// ACard({AHY_16, AHMG_15, AKFD_35, ANGT_30}, 2, 3) ----- 将荷叶，毁灭菇，咖啡豆，南瓜头放在二行三列
// ACard({AHY_16, AHMG_15, AKFD_35, ANGT_30}, {{3, 2}, {3, 3}, {3, 4}}) ---- 将荷叶，毁灭菇，咖啡豆，南瓜头放在二行三列 三行三列 三行四列
//                                                                           注意是先将第一颗植物尝试所有位置，在进行下一个植物的尝试
std::vector<APlant*> ACard(const std::vector<ACardName>& lst);
APlant* ACard(int seedIndex, int row, float col);
APlant* ACard(int seedIndex, const std::vector<APosition>& lst);
APlant* ACard(int seedIndex, const std::vector<AGrid>& lst);
APlant* ACard(int seedIndex, const std::initializer_list<AGrid>& lst);
APlant* ACard(APlantType plantType, int row, float col);
APlant* ACard(APlantType plantType, const std::vector<APosition>& lst);
APlant* ACard(APlantType plantType, const std::vector<AGrid>& lst);
APlant* ACard(APlantType plantType, const std::initializer_list<AGrid>& lst);
std::vector<APlant*> ACard(const std::vector<APlantType>& plantTypeVec, int row, float col);
std::vector<APlant*> ACard(const std::vector<APlantType>& plantTypeVec, const std::vector<APosition>& lst);
std::vector<APlant*> ACard(const std::vector<APlantType>& plantTypeVec, const std::vector<AGrid>& lst);
std::vector<APlant*> ACard(const std::vector<APlantType>& plantTypeVec, const std::initializer_list<AGrid>& lst);

#endif
