#ifndef __AVZ_PAINTER_H__
#define __AVZ_PAINTER_H__

#include "avz_state_hook.h"
#include <d3d.h>
#include <list>
#include <memory>
#include <unordered_map>
#include <unordered_set>

struct __AGeoVertex {
    float __x, __y, __z, __rhw;
    DWORD __color;

    __AGeoVertex()
        : __x(0.0f), __y(0.0f), __z(0.0f), __rhw(0.0f), __color(0) {}

    __AGeoVertex(float x, float y, DWORD color)
        : __x(x), __y(y), __z(0.0f), __rhw(1.0f), __color(color) {}

    __AGeoVertex(int x, int y, DWORD color)
        : __x((float)x), __y((float)y), __z(0.0f), __rhw(1.0f), __color(color) {}
};

struct __ATexVertex {
    float __x, __y, __z, __rhw;
    DWORD __color, __spec;
    float __u, __v;

    __ATexVertex()
        : __x(0.0f), __y(0.0f), __z(0.0f), __rhw(0.0f), __color(0), __spec(0), __u(0), __v(0) {}

    __ATexVertex(float x, float y, DWORD color, float u, float v)
        : __x(x), __y(y), __z(0.0f), __rhw(1.0f), __color(color), __spec(0), __u(u), __v(v) {}

    __ATexVertex(int x, int y, DWORD color, float u, float v)
        : __x((float)x), __y((float)y), __z(0.0f), __rhw(1.0f), __color(color), __spec(0), __u(u), __v(v) {}
};

struct __AD3dInfo {
    static IDirect3DDevice7* device;
    static IDirectDraw7* ddraw;
};

struct __ATextInfo : public __AD3dInfo {
    HFONT HFont;
    int fontW;
    int fontH;
    int fontG;
};

struct __ACursorInfo : public __AD3dInfo {
    int type;
    HCURSOR hCursor;
};

// 得到一个 ARGB 颜色, A 的意思是不透明度,  数值范围为 [0, 255]
// R 表示 红色, 数值范围为 [0, 255]
// G 表示 绿色, 数值范围为 [0, 255]
// B 表示 蓝色, 数值范围为 [0, 255]
// 其他颜色需要使用这三原色混合出来
// 例如完全不透明的黄色就是 AArgb(0xff, 0xff, 0xff, 0);
inline constexpr uint32_t AArgb(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
    return uint32_t(a << 24) | uint32_t(r << 16) | uint32_t(g << 8) | uint32_t(b);
}

class __ATexture {
    int mWidth;
    int mHeight;
    float u;
    float v;
    IDirectDrawSurface7* texture;
    __ATextInfo* textInfo = nullptr;

public:
    ~__ATexture();

    __ATexture(wchar_t chr, __ATextInfo* _textInfo);
    __ATexture(__ACursorInfo* cursorInfo);

    void Draw(DWORD color, int x, int y) const;

    IDirectDrawSurface7* CreateTextureSurface(int theWidth, int theHeight);

protected:
    // 给一个图像画上黑色边框
    void _DrawBlackBorder(DWORD* bits);

    // 将 bit 复制到 surface 上
    void _CopyBitsToSurface(DWORD* src, DDSURFACEDESC2& dst, __ACursorInfo* cursorInfo);
};

class __ABasicPainter : public AOrderedBeforeScriptHook<-1>, //
                        public AOrderedExitFightHook<-1>,
                        public AOrderedAfterInjectHook<-1> {
    __ADeleteCopyAndMove(__ABasicPainter);

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
            , duration(rhs.duration) {}

        DrawInfo& operator=(DrawInfo&& rhs) {
            this->rect = rhs.rect;
            this->textVec = std::move(rhs.textVec);
            this->rectColor = rhs.rectColor;
            this->textColor = rhs.textColor;
            this->duration = rhs.duration;
            return *this;
        }
    };

    __ABasicPainter() {
        GetPainterSet().insert(this);
    }

    ~__ABasicPainter() {
        GetPainterSet().erase(this);
    }

    // Hook
    static bool AsmDraw();
    static void DrawEveryTick();

    bool IsOk();
    void ClearFont();
    std::deque<DrawInfo> drawInfoQueue;
    std::deque<std::pair<ACursor, int>> cursorQueue;
    std::unordered_map<wchar_t, std::shared_ptr<__ATexture>> textureDict;
    static std::vector<std::vector<int>> posDict;

    void DrawRect(int x, int y, int w, int h, DWORD color);
    void DrawStr(const std::wstring& text, int x, int y, DWORD color);
    static void DrawCursor(int x, int y, int type); // 0: 普通的 1: 手
    __ATextInfo* GetTextNeedInfo();
    bool Refresh();
    bool IsOpen3dAcceleration();

    static std::unordered_set<__ABasicPainter*>& GetPainterSet() {
        static std::unordered_set<__ABasicPainter*> __;
        return __;
    }

    __ATextInfo textInfo;
    int fontSize = 20;
    std::size_t maxQueueSize = 1e4;
    std::wstring fontName = L"宋体";
    static HCURSOR handCursor;
    static HCURSOR arrowCursor;

protected:
    virtual void _BeforeScript() override;
    virtual void _ExitFight() override;
    virtual void _AfterInject() override;
};

class APainter {
    __ADeleteCopyAndMove(APainter);

public:
    APainter() = default;
    // 设置字体
    // 使用示例
    // SetFont("黑体") ------ 将字体设置为黑体
    void SetFont(const std::string& name);

    // 得到字体
    __ANodiscard std::string GetFont() const;

    // 设置字体大小
    // 使用示例
    // SetFontSize(15) ------ 将字体大小设置为 15
    // 注意此处字体大小不一定与 MS Word 中的相同
    void SetFontSize(int size);

    // 得到字体的大小
    __ANodiscard int GetFontSize() const;

    // 设置文本颜色
    // 使用示例
    // 注意每个参数范围为 [0, 255]
    // SetTextColor(AArgb(0xff, 0, 0, 0)) ----- 将文本的不透明度设置为 0xff, 也就是不透明, 色彩设置为 RGB(0, 0, 0), 也就是黑色
    void SetTextColor(DWORD color);

    __ANodiscard DWORD GetTextColor() const;

    // 设置矩形框颜色
    // 使用示例
    // 注意每个参数范围为 [0, 255]
    // SetRectColor(AArgb(0xff, 0, 0, 0)) ----- 将矩形框的不透明度设置为 0xff, 也就是不透明, 色彩设置为 RGB(0, 0, 0), 也就是黑色
    void SetRectColor(DWORD color);

    __ANodiscard DWORD GetRectColor() const;

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
    void Draw(const ACursor& cursor, int duration = 1);

    // 设定队列最大容量
    // 这个容量是为了防止内存泄露的
    // 设置的越小就会限制在屏幕中显示的数目
    // 设置的越大就越可能导致跳帧时的内存泄露
    void SetMaxQueueSize(std::size_t size) {
        _maxQueueSize = size;
    }

protected:
    DWORD _textColor = AArgb(0xff, 0, 0xff, 0xff);
    DWORD _rectColor = AArgb(0xaf, 0, 0, 0);
    __ABasicPainter _basicPainter;
    std::size_t _maxQueueSize = 1e4;
};

#endif
