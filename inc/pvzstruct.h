#pragma once

#include <cstdint>

struct PvZ;				// 游戏主体
struct MainObject;		// 主要对象
struct Plant;			// 植物
struct Zombie;			// 僵尸
struct Seed;			// 种子（卡片）
struct Item;			// 收集物
struct MouseWindow;		// 鼠标窗口
struct TopMouseWindow;	// 顶层鼠标窗口
struct LetsRockBtn;		// lets_rock 按钮
struct SelectCardUi_m;	// 选卡界面_在 main_object
struct SelectCardUi_p;	// 选卡界面_在 pvz_base
struct Mouse;			// 鼠标
struct MouseExtra;		// 鼠标额外属性
struct Text;			// 文字属性
struct AnimationMain;	// 动画主要对象
struct AnimationOffset; // 动画地址偏移
struct Animation;		// 动画

struct PvZ
{
	MainObject *mainObject()
	{
		return *(MainObject **)((uint8_t *)this + 0x768);
	}

	MouseWindow *mouseWindow()
	{
		return *(MouseWindow **)((uint8_t *)this + 0x320);
	}

	SelectCardUi_p *selectCardUi_p()
	{
		return *(SelectCardUi_p **)((uint8_t *)this + 0x774);
	}

	AnimationMain *animationMain()
	{
		return *(AnimationMain **)((uint8_t *)this + 0x820);
	}

	int &gameUi()
	{
		return (int &)((uint8_t *)this)[0x7fc];
	}
};

struct MainObject
{
	Zombie *zombieArray()
	{
		return *(Zombie **)((uint8_t *)this + 0x90);
	}
	int &zombieTotal()
	{
		return (int &)((uint8_t *)this)[0x94];
	}
	int &zombieLimit()
	{
		return (int &)((uint8_t *)this)[0x98];
	}
	int &zombieNext()
	{
		return (int &)((uint8_t *)this)[0x9c];
	}
	int &zombieCount()
	{
		return (int &)((uint8_t *)this)[0xa0];
	}
	int &zombieLast()
	{
		return (int &)((uint8_t *)this)[0xa4];
	}
	int &zombieRefreshHp()
	{
		return (int &)((uint8_t *)this)[0x5594];
	}
	Plant *plantArray()
	{
		return *(Plant **)((uint8_t *)this + 0xac);
	}
	int &plantCountMax()
	{
		return (int &)((uint8_t *)this)[0xb0];
	}
	int &plantLimit()
	{
		return (int &)((uint8_t *)this)[0xb4];
	}
	int &plantNext()
	{
		return (int &)((uint8_t *)this)[0xb8];
	}
	int &plantCount()
	{
		return (int &)((uint8_t *)this)[0xbc];
	}
	int &plantLast()
	{
		return (int &)((uint8_t *)this)[0xc0];
	}

	Seed *seedArray()
	{
		return *(Seed **)((uint8_t *)this + 0x144);
	}

	Item *itemArray()
	{
		return *(Item **)((uint8_t *)this + 0xe4);
	}
	int &itemTotal()
	{
		return (int &)((uint8_t *)this)[0xe8];
	}
	bool &gamePaused()
	{
		return (bool &)((uint8_t *)this)[0x164];
	}
	int &scene()
	{
		return (int &)((uint8_t *)this)[0x554c];
	}

	int &sun()
	{
		return (int &)((uint8_t *)this)[0x5560];
	}
	int &gameClock()
	{
		return (int &)((uint8_t *)this)[0x5568];
	}
	int &refreshCountdown()
	{
		return (int &)((uint8_t *)this)[0x559c];
	}
	int &hugewaveCountdown()
	{
		return (int &)((uint8_t *)this)[0x55a4];
	}
	int &wave()
	{
		return (int &)((uint8_t *)this)[0x557c];
	}
	int &initialCountdown()
	{
		return (int &)((uint8_t *)this)[0x55a0];
	}

	int &clickPaoCountdown()
	{
		return (int &)((uint8_t *)this)[0x5754];
	}

	Mouse *mouseAttribution()
	{
		return *(Mouse **)((uint8_t *)this + 0x138);
	}

	MouseExtra *mouseExtraAttribution()
	{
		return *(MouseExtra **)((uint8_t *)this + 0x13c);
	}

	SelectCardUi_m *selectCardUi_m()
	{
		return *(SelectCardUi_m **)((uint8_t *)this + 0x15c);
	}

	uint32_t *zombieList()
	{
		return (uint32_t *)((uint8_t *)this + 0x6b4);
	}

	Text *text()
	{
		return *(Text **)((uint8_t *)this + 0x140);
	}
};

struct Plant
{
	uint8_t data[0x14c];

	int &xi()
	{
		return (int &)((uint8_t *)this)[0x8];
	}
	int &yi()
	{
		return (int &)((uint8_t *)this)[0xc];
	}

	int &visible()
	{
		return (int &)((uint8_t *)this)[0x18];
	}
	int &row()
	{
		return (int &)((uint8_t *)this)[0x1c];
	}
	int &type()
	{
		return (int &)((uint8_t *)this)[0x24];
	}
	int &col()
	{
		return (int &)((uint8_t *)this)[0x28];
	}
	//返回植物的状态
	//35：空炮
	//36：正在装填
	//37：准备就绪
	//38：正在发射
	int &state()
	{
		return (int &)((uint8_t *)this)[0x3c];
	}
	int &stateCountdown()
	{
		return (int &)((uint8_t *)this)[0x54];
	}
	int &hp()
	{
		return (int &)((uint8_t *)this)[0x40];
	}
	int &hpMax()
	{
		return (int &)((uint8_t *)this)[0x44];
	}
	int &canShoot()
	{
		return (int &)((uint8_t *)this)[0x48];
	}
	int &bloverCountdown()
	{
		return (int &)((uint8_t *)this)[0x4c];
	}
	int &explodeCountdown()
	{
		return (int &)((uint8_t *)this)[0x50];
	}
	int &shootCountdown()
	{
		return (int &)((uint8_t *)this)[0x90];
	}
	int &mushroomCountdown()
	{
		return (int &)((uint8_t *)this)[0x130];
	}
	bool &isDisappeared()
	{
		return (bool &)((uint8_t *)this)[0x141];
	}
	bool &isCrushed()
	{
		return (bool &)((uint8_t *)this)[0x142];
	}
	bool &sleeping()
	{
		return (bool &)((uint8_t *)this)[0x143];
	}
	// 动作编号
	uint16_t &animationCode()
	{
		return (uint16_t &)((uint8_t *)this)[0x94];
	}
};

struct Zombie
{
	uint8_t data[0x15c];

	uint16_t isExist()
	{
		return (uint16_t)((uint8_t *)this)[0x15a];
	}
	int &row()
	{
		return (int &)((uint8_t *)this)[0x1c];
	}

	float &abscissa()
	{
		return (float &)((uint8_t *)this)[0x2c];
	}

	float &ordinate()
	{
		return (float &)((uint8_t *)this)[0x30];
	}

	int &type()
	{
		return (int &)((uint8_t *)this)[0x24];
	}

	int &hp()
	{
		return (int &)((uint8_t *)this)[0xC8];
	}

	int &oneHp()
	{
		return (int &)((uint8_t *)this)[0xD0];
	}

	int &twoHp()
	{
		return (int &)((uint8_t *)this)[0xDC];
	}

	bool &isEat()
	{
		return (bool &)((uint8_t *)this)[0x51];
	}

	int &state()
	{
		return (int &)((uint8_t *)this)[0x28];
	}

	bool isDead()
	{
		return state() == 1;
	}

	bool isHammering()
	{
		return state() == 70;
	}
	bool &isStealth()
	{
		return (bool &)((uint8_t *)this)[0x18];
	}

	float &speed()
	{
		return (float &)((uint8_t *)this)[0x34];
	}
	int &existTime()
	{
		return (int &)((uint8_t *)this)[0x60];
	}

	int &stateCountdown()
	{
		return (int &)((uint8_t *)this)[0x68];
	}

	bool &isDisappeared()
	{
		return (bool &)((uint8_t *)this)[0xEC];
	}

	int &bulletAbscissa()
	{
		return (int &)((uint8_t *)this)[0x8C];
	}
	int &bulletOrdinate()
	{
		return (int &)((uint8_t *)this)[0x90];
	}

	int &attackAbscissa()
	{
		return (int &)((uint8_t *)this)[0x9C];
	}

	int &attackOrdinate()
	{
		return (int &)((uint8_t *)this)[0xA0];
	}

	int &slowCountdown()
	{
		return (int &)((uint8_t *)this)[0xAC];
	}

	int &fixationCountdown()
	{
		return (int &)((uint8_t *)this)[0xB0];
	}

	int &freezeCountdown()
	{
		return (int &)((uint8_t *)this)[0xB4];
	}
};

struct Seed
{
	uint8_t data[0x50];

	int &count()
	{
		return (int &)((uint8_t *)this)[0x24];
	}

	bool &isUsable()
	{
		return (bool &)((uint8_t *)this)[0x70];
	}

	int &cd()
	{
		return (int &)((uint8_t *)this)[0x4c];
	}

	int &initialCd()
	{
		return (int &)((uint8_t *)this)[0x50];
	}

	int &imitatorType()
	{
		return (int &)((uint8_t *)this)[0x60];
	}

	int &type()
	{
		return (int &)((uint8_t *)this)[0x5c];
	}
};

struct Item
{
	uint8_t data[0xd8];

	bool &isDisappeared()
	{
		return (bool &)((uint8_t *)this)[0x38];
	}

	bool &is_collected()
	{
		return (bool &)((uint8_t *)this)[0x50];
	}

	float &abscissa()
	{
		return (float &)((uint8_t *)this)[0x24];
	}

	float &ordinate()
	{
		return (float &)((uint8_t *)this)[0x28];
	}
};

struct AnimationMain
{
	AnimationOffset *animationOffset()
	{
		return *(AnimationOffset **)((uint8_t *)this + 0x8);
	}
};

struct AnimationOffset
{
	Animation *animationArray()
	{
		return *(Animation **)((uint8_t *)this + 0x0);
	}
};

struct Animation
{
	uint8_t data[0xa0];

	// 动画循环率
	float &circulationRate()
	{
		return (float &)((uint8_t *)this)[0x4];
	}
};

struct TopMouseWindow
{
};

struct MouseWindow
{
	TopMouseWindow *topWindow()
	{
		return *(TopMouseWindow **)((uint8_t *)this + 0x94);
	}

	bool &isInWindow()
	{
		return (bool &)((uint8_t *)this)[0xdc];
	}
};

struct SelectCardUi_m
{
	int &orizontalScreenOffset()
	{
		return (int &)((uint8_t *)this)[0x8];
	}

	bool &isCreatZombie()
	{
		return (bool &)((uint8_t *)this)[0x35];
	}
};

struct SelectCardUi_p
{
	LetsRockBtn *letsRockBtn()
	{
		return *(LetsRockBtn **)((uint8_t *)this + 0x88);
	}
};

struct LetsRockBtn
{
	bool &isUnusable()
	{
		return (bool &)((uint8_t *)this)[0x1a];
	}
};

struct Mouse
{
	// 鼠标上物品的类型
	int &type()
	{
		return (int &)((uint8_t *)this)[0x30];
	}

	int &abscissa()
	{
		return (int &)((uint8_t *)this)[0x8];
	}
};

struct MouseExtra
{
	int &row()
	{
		return (int &)((uint8_t *)this)[0x28];
	}
};

struct Text
{
	int &disappearCountdown()
	{
		return (int &)((uint8_t *)this)[0x88];
	}
};

// 植物类型
enum PlantType
{
	WDSS_0,	  // 豌豆射手
	XRK_1,	  // 向日葵
	YTZD_2,	  // 樱桃炸弹
	JG_3,	  // 坚果
	TDDL_4,	  // 土豆地雷
	HBSS_5,	  // 寒冰射手
	DZH_6,	  // 大嘴花
	SCSS_7,	  // 双重射手
	XPG_8,	  // 小喷菇
	YGG_9,	  // 阳光菇
	DPG_10,	  // 大喷菇
	MBTSZ_11, // 墓碑吞噬者
	MHG_12,	  // 魅惑菇
	DXG_13,	  // 胆小菇
	HBG_14,	  // 寒冰菇
	HMG_15,	  // 毁灭菇
	HY_16,	  // 荷叶
	WG_17,	  // 倭瓜
	SFSS_18,  // 三发射手
	CRHZ_19,  // 缠绕海藻
	HBLJ_20,  // 火爆辣椒
	DC_21,	  // 地刺
	HJSZ_22,  // 火炬树桩
	GJG_23,	  // 高坚果
	SBG_24,	  // 水兵菇
	LDH_25,	  // 路灯花
	XRZ_26,	  // 仙人掌
	SYC_27,	  // 三叶草
	LJSS_28,  // 裂荚射手
	YT_29,	  // 杨桃
	NGT_30,	  // 南瓜头
	CLG_31,	  // 磁力菇
	JXCTS_32, // 卷心菜投手
	HP_33,	  // 花盆
	YMTS_34,  // 玉米投手
	KFD_35,	  // 咖啡豆
	DS_36,	  // 大蒜
	YZBHS_37, // 叶子保护伞
	JZH_38,	  // 金盏花
	XGTS_39,  // 西瓜投手
	JQSS_40,  // 机枪射手
	SZXRK_41, // 双子向日葵
	YYG_42,	  // 忧郁菇
	XP_43,	  // 香蒲
	BXGTS_44, // 冰西瓜投手
	XJC_45,	  // 吸金磁
	DCW_46,	  // 地刺王
	YMJNP_47  // 玉米加农炮
};

enum ZombieType
{
	PJ_0 = 0,  // 普僵
	QZ_1,	   // 旗帜
	LZ_2,	   // 路障
	CG_3,	   // 撑杆
	TT_4,	   // 铁桶
	DB_5,	   // 读报
	TM_6,	   // 铁门
	GL_7,	   // 橄榄
	WW_8,	   // 舞王
	BW_9,	   // 伴舞
	YZ_10,	   // 鸭子
	QS_11,	   // 潜水
	BC_12,	   // 冰车
	XQ_13,	   // 雪橇
	HT_14,	   // 海豚
	XC_15,	   // 小丑
	QQ_16,	   // 气球
	KG_17,	   // 矿工
	TT_18,	   // 跳跳
	XR_19,	   // 雪人
	BJ_20,	   // 蹦极
	FT_21,	   // 扶梯
	TL_22,	   // 投篮
	BY_23,	   // 白眼
	XG_24,	   // 小鬼
	JB_25,	   // 僵博
	HY_32 = 32 // 红眼
};
