/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-06 15:40:52
 * @Description:
 */

#ifndef __AVZ_CARD_H__
#define __AVZ_CARD_H__

#include "avz_state_hook.h"

struct ACardName {
    APlantType plantType;
    int row;
    float col;
};

class __ACardManager : public AOrderedStateHook<-1> {
public:
    static void ChooseSingleCard();
    static void SelectCards(const std::vector<int>& lst, int selectInterval);
    static int GetCardIndex(APlantType plantType);
    static std::vector<APlant*> Card(const std::vector<ACardName>& lst);
    static APlant* Card(int seedIndex, int row, float col);
    static APlant* Card(int seedIndex, const std::vector<APosition>& lst);
    static APlant* Card(APlantType plantType, int row, float col);
    static APlant* Card(APlantType plantType, const std::vector<APosition>& lst);

protected:
    static std::vector<int> _selectCardVec;
    static std::unordered_map<int, int> _seedNameToIndexMap;
    static AMainObject* _mainObject;
    static APvzBase* _pvzBase;
    static int _selectInterval;
    static APlant* _BasicCard(int seedIndex, int row, float col);
    static APlant* _BasicCard(int seedIndex, const std::vector<APosition>& lst);
    static bool _Check(int seedIndex);
    virtual void _EnterFight() override;
};

inline __ACardManager __cm; // AStateHook

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
inline void ASelectCards(const std::vector<int>& lst, int selectInterval = 17)
{
    __ACardManager::SelectCards(lst, selectInterval);
}

// 根据卡片名称得到卡片索引
// *** 注意：卡片索引从 0 开始, 不存在返回 -1
// *** 使用示例：
// AGetCardIndex(AICE_SHROOM) ---- 得到寒冰菇的卡片索引
__ANodiscard inline int AGetCardIndex(APlantType plantType)
{
    return __ACardManager::GetCardIndex(plantType);
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
inline std::vector<APlant*> ACard(const std::vector<ACardName>& lst)
{
    return __ACardManager::Card(lst);
}
inline APlant* ACard(int seedIndex, int row, float col)
{
    return __ACardManager::Card(seedIndex, row, col);
}
inline APlant* ACard(int seedIndex, const std::vector<APosition>& lst)
{
    return __ACardManager::Card(seedIndex, lst);
}
inline APlant* ACard(APlantType plantType, int row, float col)
{
    return __ACardManager::Card(plantType, row, col);
}
inline APlant* ACard(APlantType plantType, const std::vector<APosition>& lst)
{
    return __ACardManager::Card(plantType, lst);
}
inline std::vector<APlant*> ACard(const std::vector<APlantType>& plantTypeVec, int row, float col)
{
    std::vector<APlant*> ret;
    for (auto&& plantType : plantTypeVec) {
        ret.push_back(__ACardManager::Card(plantType, row, col));
    }
    return ret;
}
inline std::vector<APlant*> ACard(const std::vector<APlantType>& plantTypeVec, const std::vector<APosition>& lst)
{
    std::vector<APlant*> ret;
    for (auto&& plantType : plantTypeVec) {
        for (auto&& pos : lst) {
            auto ptr = __ACardManager::Card(plantType, pos.row, pos.col);
            if (ptr != nullptr) {
                ret.push_back(ptr);
                break;
            }
        }
    }
    return ret;
}

#endif