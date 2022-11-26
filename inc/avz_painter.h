/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-06 15:35:57
 * @Description:
 */

#ifndef __AVZ_PAINTER_H__
#define __AVZ_PAINTER_H__

#include "avz_global.h"
#include <d3d.h>
#include <list>
#include <unordered_map>

struct __AGeoVertex {
    float __x, __y, __z, __rhw;
    DWORD __color;

    __AGeoVertex()
        : __x(0.0f)
        , __y(0.0f)
        , __z(0.0f)
        , __rhw(0.0f)
        , __color(0)
    {
    }
    __AGeoVertex(float x, float y, DWORD color)
        : __x(x)
        , __y(y)
        , __z(0.0f)
        , __rhw(1.0f)
        , __color(color)
    {
    }
    __AGeoVertex(int x, int y, DWORD color)
        : __x((float)x)
        , __y((float)y)
        , __z(0.0f)
        , __rhw(1.0f)
        , __color(color)
    {
    }
};

struct __ATexVertex {
    float __x, __y, __z, __rhw;
    DWORD __color, __spec;
    float __u, __v;

    __ATexVertex()
        : __x(0.0f)
        , __y(0.0f)
        , __z(0.0f)
        , __rhw(0.0f)
        , __color(0)
        , __spec(0)
        , __u(0)
        , __v(0)
    {
    }
    __ATexVertex(float x, float y, DWORD color, float u, float v)
        : __x(x)
        , __y(y)
        , __z(0.0f)
        , __rhw(1.0f)
        , __color(color)
        , __spec(0)
        , __u(u)
        , __v(v)
    {
    }
    __ATexVertex(int x, int y, DWORD color, float u, float v)
        : __x((float)x)
        , __y((float)y)
        , __z(0.0f)
        , __rhw(1.0f)
        , __color(color)
        , __spec(0)
        , __u(u)
        , __v(v)
    {
    }
};

struct __ATextureNeedInfo {
    IDirect3DDevice7* device = nullptr;
    IDirectDraw7* ddraw = nullptr;
    HFONT HFont;
    int fontW;
    int fontH;
    int fontG;
};

// 得到一个 ARGB 颜色, A 的意思是不透明度,  数值范围为 [0, 255]
// R 表示 红色, 数值范围为 [0, 255]
// G 表示 绿色, 数值范围为 [0, 255]
// B 表示 蓝色, 数值范围为 [0, 255]
// 其他颜色需要使用这三原色混合出来
// 例如完全不透明的黄色就是 AArgb(0xff, 0xff, 0xff, 0);
inline constexpr uint32_t AArgb(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
    return uint32_t(a << 24) | uint32_t(r << 16) | uint32_t(g << 8) | uint32_t(b);
}

class __ATexture {
    int mWidth;
    int mHeight;
    float u;
    float v;
    IDirectDrawSurface7* texture;
    __ATextureNeedInfo* textNeedInfo;

public:
    ~__ATexture();

    __ATexture(wchar_t chr, __ATextureNeedInfo* _textNeedInfo);

    void Draw(DWORD color, int x, int y) const;

    IDirectDrawSurface7* createTextureSurface(int theWidth, int theHeight);
};

class __ADraw {
    IDirectDrawSurface7* _surface = nullptr;

public:
    __ATextureNeedInfo textNeedInfo;
    int fontSize = 20;
    std::wstring fontName = L"宋体";
    void DrawRect(int x, int y, int w, int h, DWORD color);
    __ATextureNeedInfo* GetTextureNeedInfo();
    bool Refresh();
    bool IsOpen3dAcceleration();
};

class __AStaticPainter : public AStateHook {
public:
    struct DrawInfo {
        ARect rect;
        std::vector<std::wstring> textVec;
        DWORD rectColor;
        DWORD textColor;
        int duration;
        DrawInfo() = default;
        DrawInfo(DrawInfo&& rhs)
            : rect(rhs.rect)
            , textVec(std::move(rhs.textVec))
            , rectColor(rhs.rectColor)
            , textColor(rhs.textColor)
            , duration(rhs.duration)

        {
        }

        DrawInfo& operator=(DrawInfo&& rhs)
        {
            this->rect = rhs.rect;
            this->textVec = std::move(rhs.textVec);
            this->rectColor = rhs.rectColor;
            this->textColor = rhs.textColor;
            this->duration = rhs.duration;
            return *this;
        }
    };

    static bool AsmDraw();
    static bool IsOk();
    static void DrawEveryTick();
    static void ClearFont();
    static std::deque<DrawInfo> drawInfoQueue;
    static __ADraw draw;
    static std::unordered_map<wchar_t, __ATexture*> textureDict;
    static std::vector<std::vector<int>> posDict;

protected:
    virtual void BeforeScript() override;
    virtual void ExitFight() override;
};

class APainter {
    __ADeleteCopyAndMove(APainter);

public:
    // 设置字体
    // 使用示例
    // SetFont("黑体") ------ 将字体设置为黑体
    void SetFont(const std::string& name);

    // 设置字体大小
    // 使用示例
    // SetFontSize(15) ------ 将字体大小设置为 15
    // 注意此处字体大小不一定与 MS Word 中的相同
    void SetFontSize(int size);

    // 设置文本颜色
    // 使用示例
    // 注意每个参数范围为 [0, 255]
    // SetTextColor(AArgb(0xff, 0, 0, 0)) ----- 将文本的不透明度设置为 0xff, 也就是不透明, 色彩设置为 RGB(0, 0, 0), 也就是黑色
    void SetTextColor(DWORD color);

    __ANodiscard DWORD GetTextColor();

    // 设置矩形框颜色
    // 使用示例
    // 注意每个参数范围为 [0, 255]
    // SetRectColor(AArgb(0xff, 0, 0, 0)) ----- 将矩形框的不透明度设置为 0xff, 也就是不透明, 色彩设置为 RGB(0, 0, 0), 也就是黑色
    void SetRectColor(DWORD color);

    __ANodiscard DWORD GetRectColor();

    // 绘制函数
    // 第一个参数指的是绘制什么: 文本还是矩形
    // 第二个参数指的是绘制的持续时间
    // 绘制文本
    // ***使用示例
    // Draw(AText("hello", 100, 100)) ------ 在游戏画面(100, 100) 处绘制 hello
    // Draw(AText("hello", 100, 100, RIGHT_TOP)) ------ 在游戏画面(100, 100) 处绘制 hello, 并且文本绘制是在 (100, 100) 的右上方
    // 绘制矩形
    // ***使用示例
    // Draw(ARect(100, 100, 200, 200)) ------ 在游戏画面(100, 100) 处绘制宽高为 (200, 200) 的矩形, 默认显示 1cs
    // Draw(ARect(100, 100, 200, 200), 100) ------ 在游戏画面(100, 100) 处绘制宽高为 (200, 200) 的矩形, 显示 100cs
    void Draw(const ARect& rect, int duration = 1);
    void Draw(const AText& posText, int duration = 1);

protected:
    DWORD _textColor = AArgb(0xff, 0, 0xff, 0xff);
    DWORD _rectColor = AArgb(0xaf, 0, 0, 0);
    static __AStaticPainter _aStaticPainter; // 用于生成绘制的 hook
};
#endif