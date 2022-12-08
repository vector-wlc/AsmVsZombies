/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-09 09:56:27
 * @Description:
 */
#include "avz_painter.h"
#include "avz_logger.h"

__AStaticPainter APainter::_aStaticPainter;

void APainter::SetFont(const std::string& name)
{
    __AStaticPainter::draw.fontName = AStrToWstr(name);
    __AStaticPainter::ClearFont();
}

void APainter::SetFontSize(int size)
{
    __AStaticPainter::draw.fontSize = size;
    __AStaticPainter::ClearFont();
}

void APainter::SetTextColor(DWORD color)
{
    _textColor = color;
}

DWORD APainter::GetRectColor()
{
    return _textColor;
}

void APainter::SetRectColor(DWORD color)
{
    _rectColor = color;
}

DWORD APainter::GetTextColor()
{
    return _rectColor;
}

void APainter::Draw(const ARect& rect, int duration)
{
    if (!_aStaticPainter.draw.IsOpen3dAcceleration()) {
        __aInternalGlobal.loggerPtr->Warning("您尚未开启 3D 加速，无法使用绘制类");
        return;
    }
    __AStaticPainter::DrawInfo info;
    info.rect = rect;
    info.rect.x += info.rect.width * __AStaticPainter::posDict[int(rect.pos)][0];
    info.rect.y += info.rect.height * __AStaticPainter::posDict[int(rect.pos)][1];
    info.duration = duration;
    info.rectColor = _rectColor;
    __AStaticPainter::drawInfoQueue.emplace_back(std::move(info));
}

void APainter::Draw(const AText& posText, int duration)
{
    if (!_aStaticPainter.draw.IsOpen3dAcceleration()) {
        __aInternalGlobal.loggerPtr->Warning("您尚未开启 3D 加速，无法使用绘制类");
        return;
    }
    if (posText.text.empty()) {
        return;
    }
    __AStaticPainter::DrawInfo info;
    std::wstring lineText;
    auto wText = AStrToWstr(posText.text);
    int fontSize = __AStaticPainter::draw.fontSize;
    int textRightX = posText.x;
    int width = 0;

    // 对文本进行预处理
    // 遇到 \n 字符或者文本宽度超出 pvz 窗口宽度时换行
    for (auto&& ch : wText) {
        if (ch != '\n' && textRightX < 800 - fontSize) {
            textRightX += ch > 0xff ? fontSize : fontSize / 2 + 1;
            lineText.push_back(ch);
        } else {
            if (ch != '\n') {
                lineText.push_back(ch);
            }
            if (width < textRightX - posText.x) {
                width = textRightX - posText.x;
            }
            textRightX = posText.x;
            info.textVec.push_back(std::move(lineText));
            lineText.clear();
        }
    }

    if (!lineText.empty()) {
        if (width < textRightX - posText.x) {
            width = textRightX - posText.x;
        }
        textRightX = posText.x;
        info.textVec.push_back(std::move(lineText));
    }

    if (posText.isHasBkg) {
        info.rect.width = width;
        info.rect.height = info.textVec.size() * __AStaticPainter::draw.fontSize;
        info.rect.x = posText.x + info.rect.width * __AStaticPainter::posDict[int(posText.pos)][0];
        info.rect.y = posText.y + info.rect.height * __AStaticPainter::posDict[int(posText.pos)][1];
        info.rectColor = _rectColor;
    } else { // 不绘制背景，将宽度设置为 -1
        info.rect.width = -1;
    }
    info.duration = duration;
    info.textColor = _textColor;
    __AStaticPainter::drawInfoQueue.emplace_back(std::move(info));
}

std::deque<__AStaticPainter::DrawInfo> __AStaticPainter::drawInfoQueue;
__ADraw __AStaticPainter::draw;
std::unordered_map<wchar_t, __ATexture*> __AStaticPainter::textureDict;
std::vector<std::vector<int>> __AStaticPainter::posDict = {
    {0, -1},
    {0, 0},
    {-1, -1},
    {-1, 0},
};

void __AStaticPainter::ClearFont()
{
    DeleteObject(draw.textNeedInfo.HFont);
    draw.textNeedInfo.HFont = nullptr;
    // free texture memory
    for (auto&& obj : textureDict) {
        delete obj.second;
    }
    textureDict.clear();
}

bool __AStaticPainter::IsOk()
{
    static int recordClock = 0;
    static bool isOk = false;
    int gameClock = __aInternalGlobal.mainObject->GlobalClock();
    if (gameClock != recordClock) { // 一帧刷新一次
        recordClock = gameClock;
        isOk = draw.Refresh();
    }
    return isOk;
}

void __AStaticPainter::DrawEveryTick()
{
    if (!IsOk()) {
        return;
    }

    while (!drawInfoQueue.empty()) {
        if (drawInfoQueue.front().duration <= 0) { // 释放已经不显示的内存
            drawInfoQueue.pop_front();
        } else {
            break;
        }
    }

    for (auto&& info : drawInfoQueue) {
        if (info.duration <= 0) {
            continue;
        }
        if (info.rect.width != -1) { // 需要绘制矩形
            draw.DrawRect(info.rect.x, info.rect.y, info.rect.width, info.rect.height, info.rectColor);
        }
        if (!info.textVec.empty()) { // 需要绘制字符串
            int y = info.rect.y;
            for (auto&& wstr : info.textVec) {
                int x = info.rect.x;
                for (auto&& ch : wstr) { // 绘制一行
                    auto iter = textureDict.find(ch);
                    if (iter == textureDict.end()) { // 将字符串存入字典
                        auto texPtr = new __ATexture(ch, draw.GetTextureNeedInfo());
                        textureDict[ch] = texPtr;
                    }

                    textureDict[ch]->Draw(info.textColor, x, y);
                    x += ch > 0xff ? draw.fontSize : draw.fontSize / 2 + 1;
                }
                y += draw.fontSize;
            }
        }
        --info.duration;
    }
}

void __AStaticPainter::_BeforeScript()
{
    if (!draw.IsOpen3dAcceleration()) {
        return;
    }
    // InstallDrawHook
    *(uint16_t*)0x54C8CD = 0x5890;
    *(uint32_t*)0x667D0C = (uint32_t)&AsmDraw;
    *(uint32_t*)0x671578 = (uint32_t)&AsmDraw;
    *(uint32_t*)0x676968 = (uint32_t)&AsmDraw;
}

void __AStaticPainter::_ExitFight()
{
    if (!draw.IsOpen3dAcceleration()) {
        return;
    }
    ClearFont();
    drawInfoQueue.clear();
    // UninstallDrawHook
    *(uint16_t*)0x54C8CD = 0xD0FF;
    *(uint32_t*)0x667D0C = 0x54C650;
    *(uint32_t*)0x671578 = 0x54C650;
    *(uint32_t*)0x676968 = 0x54C650;
}

bool __AStaticPainter::AsmDraw()
{
    static int __x = 0;

#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushal;"
        "movl 0x6A9EC0, %%ecx;"
        "movl $0x54C650, %%edx;"
        "calll *%%edx;"
        "movl %%eax, %[__x];"
        "popal;"
        :
        : [__x] "m"(__x)
        :);

    if (__x) {
        DrawEveryTick();
        __asm__ __volatile__(
            "pushal;"
            "pushl $0;"
            "movl 0x6A9EC0, %%ecx;"
            "movl $0X54BAE0, %%edx;"
            "calll *%%edx;"
            "popal;"
            :
            :
            :);
    }

    __asm__ __volatile__(
        "movl %[__x], %%eax;"
        :
        : [__x] "m"(__x)
        :);

#else
    // static int ans
    // asm mov ecx [0x6A9EC0]
    // asm call 0x54C650
    // asm mov ans eax
    // if(ans){
    //     _DrawEveryTick()
    //     asm push 0
    //     asm mov ecx [0x6A9EC0]
    //     asm call 0X54BAE0
    // }
    // asm mov eax ans
#endif
    return __x;
}

__ATexture::__ATexture(wchar_t chr, __ATextureNeedInfo* _textNeedInfo)
    : mWidth(0)
    , mHeight(0)
    , u(0)
    , v(0)
    , texture(nullptr)
    , textNeedInfo(_textNeedInfo)
{
    wchar_t str[2] = {chr, L'\0'};
    HDC hDc = CreateCompatibleDC(nullptr);
    HFONT anOldFont = (HFONT)SelectObject(hDc, textNeedInfo->HFont);
    HBRUSH anOldBrush = (HBRUSH)SelectObject(hDc, GetStockObject(BLACK_BRUSH));
    RECT aRect = {0, 0, textNeedInfo->fontW, textNeedInfo->fontH + 1};
    DrawTextW(hDc, str, 1, &aRect, DT_CALCRECT | DT_NOPREFIX);
    if (aRect.right == 0) {
        SelectObject(hDc, anOldBrush);
        SelectObject(hDc, anOldFont);
        return;
    }
    BITMAPINFO aBitmapInfo;
    memset(&aBitmapInfo, 0, sizeof(aBitmapInfo));
    BITMAPINFOHEADER& aHeader = aBitmapInfo.bmiHeader;
    aHeader.biSize = sizeof(aHeader);
    aHeader.biWidth = aRect.right;
    aHeader.biHeight = -textNeedInfo->fontH - 1;
    aHeader.biPlanes = 1;
    aHeader.biBitCount = 32;
    aHeader.biCompression = BI_RGB;
    DWORD* aBits = nullptr;
    HBITMAP aBitmap = CreateDIBSection(hDc, &aBitmapInfo, DIB_RGB_COLORS, (void**)&aBits, nullptr, 0);
    if (!aBitmap) {
        return;
    }
    HBITMAP anOldBitmap = (HBITMAP)SelectObject(hDc, aBitmap);
    Rectangle(hDc, 0, 0, aRect.right, textNeedInfo->fontH + 1);
    SetBkMode(hDc, TRANSPARENT);
    SetTextColor(hDc, RGB(255, 255, 255));
    RECT rect = {0, 1, aRect.right, textNeedInfo->fontH + 1};
    DrawTextW(hDc, str, 1, &rect, DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX);
    GdiFlush();
    SelectObject(hDc, anOldBrush);
    SelectObject(hDc, anOldFont);
    SelectObject(hDc, anOldBitmap);
    mWidth = aRect.right;
    mHeight = textNeedInfo->fontH + 1;
    texture = createTextureSurface(mWidth, mHeight);
    DDSURFACEDESC2 aDesc;
    aDesc.dwSize = sizeof(aDesc);
    texture->Lock(nullptr, &aDesc, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, nullptr);
    u = (float)(mWidth - 1) / aDesc.dwWidth;
    v = (float)(mHeight - 1) / aDesc.dwHeight;
    char* dst = (char*)aDesc.lpSurface;
    for (int i = 0; i < mHeight; ++i) {
        DWORD* _dst = (DWORD*)dst;
        DWORD* _bits = aBits;
        for (int j = 0; j < mWidth; ++j)
            *_dst++ = ((*_bits++) << 24) | 0xFFFFFFul;
        aBits += mWidth;
        dst += aDesc.lPitch;
    }
    texture->Unlock(nullptr);
    DeleteObject(aBitmap);
}

__ATexture::~__ATexture()
{
    if (texture) {
        texture->Release();
    }
}

void __ATexture::Draw(DWORD color, int x, int y) const
{
    __ATexVertex aVertex[4] = {
        {x, y, color, 0, 0},
        {x, y + mHeight, color, 0, v},
        {x + mWidth, y, color, u, 0},
        {x + mWidth, y + mHeight, color, u, v}};
    textNeedInfo->device->SetTexture(0, texture);
    textNeedInfo->device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 452, aVertex, 4, 0);
}

IDirectDrawSurface7* __ATexture::createTextureSurface(int theWidth, int theHeight)
{
    textNeedInfo->device->SetTexture(0, nullptr);
    DDSURFACEDESC2 aDesc;
    IDirectDrawSurface7* aSurface;
    ZeroMemory(&aDesc, sizeof(aDesc));
    aDesc.dwSize = sizeof(aDesc);
    aDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    aDesc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
    aDesc.ddsCaps.dwCaps2 = DDSCAPS2_D3DTEXTUREMANAGE;
    aDesc.dwWidth = theWidth;
    aDesc.dwHeight = theHeight;
    aDesc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
    aDesc.ddpfPixelFormat.dwFlags = DDPF_ALPHAPIXELS | DDPF_RGB;
    aDesc.ddpfPixelFormat.dwRGBBitCount = 32;
    aDesc.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;
    aDesc.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
    aDesc.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
    aDesc.ddpfPixelFormat.dwBBitMask = 0x000000FF;
    textNeedInfo->ddraw->CreateSurface(&aDesc, &aSurface, nullptr);
    return aSurface;
}

void __ADraw::DrawRect(int x, int y, int w, int h, DWORD color)
{
    __AGeoVertex aVertex[4] = {
        {x, y, color},
        {x, y + h, color},
        {x + w, y, color},
        {x + w, y + h, color}};
    textNeedInfo.device->SetTexture(0, nullptr);
    textNeedInfo.device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 68, aVertex, 4, 0);
}

__ATextureNeedInfo* __ADraw::GetTextureNeedInfo()
{
    return &textNeedInfo;
}

bool __ADraw::IsOpen3dAcceleration()
{
    auto p2 = __aInternalGlobal.pvzBase->MPtr<APvzStruct>(0x36C);
    if (!p2) {
        return false;
    }
    auto p3 = p2->MPtr<APvzStruct>(0x30);
    if (!p3) {
        return false;
    }
    _surface = p3->MPtr<IDirectDrawSurface7>(0x14);
    textNeedInfo.device = p3->MPtr<IDirect3DDevice7>(0x20);
    textNeedInfo.ddraw = p3->MPtr<IDirectDraw7>(0x10);
    return (_surface != nullptr &&        //
        textNeedInfo.device != nullptr && //
        textNeedInfo.ddraw != nullptr);
}

bool __ADraw::Refresh()
{
    if (!IsOpen3dAcceleration()) {
        return false;
    }
    textNeedInfo.device->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
    textNeedInfo.device->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
    textNeedInfo.device->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_POINT);
    textNeedInfo.device->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);
    textNeedInfo.device->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTFG_POINT);
    textNeedInfo.device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    textNeedInfo.device->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    textNeedInfo.device->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    if (textNeedInfo.HFont) {
        return true;
    }
    textNeedInfo.HFont = CreateFontW(-fontSize, 0, 0, 0, 100, false, false, false, false, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FIXED_PITCH | FF_DONTCARE, fontName.c_str());

    if (!textNeedInfo.HFont) {
        return false;
    }

    textNeedInfo.fontW = fontSize * 1.1;
    textNeedInfo.fontH = fontSize * 1.1;
    return true;
}