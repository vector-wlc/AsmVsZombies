#include "libavz.h"

static bool __isInPaintTickRunner = false;

void APainter::SetFont(const std::string& name) {
    auto wstr = AStrToWstr(name);
    if (_basicPainter.fontName == wstr)
        return;
    _basicPainter.fontName = wstr;
    _basicPainter.ClearFont();
}

// 得到字体
__ANodiscard std::string APainter::GetFont() const {
    return AWStrToStr(_basicPainter.fontName);
}

// 得到字体的大小
__ANodiscard int APainter::GetFontSize() const {
    return _basicPainter.fontSize;
}

void APainter::SetFontSize(int size) {
    if (_basicPainter.fontSize == size)
        return;
    _basicPainter.fontSize = size;
    _basicPainter.ClearFont();
}

void APainter::SetTextColor(DWORD color) {
    _textColor = color;
}

DWORD APainter::GetRectColor() const {
    return _rectColor;
}

void APainter::SetRectColor(DWORD color) {
    _rectColor = color;
}

DWORD APainter::GetTextColor() const {
    return _textColor;
}

#define __ACheckQueueSize(queue)                                                               \
    if (queue.size() > _maxQueueSize) {                                                        \
        queue.clear();                                                                         \
        aLogger->Error("APainter : 绘制对象数量超过 {}，请检查是否有对象泄漏", _maxQueueSize); \
    }

// 警告用户图层合法范围只会出现一次
static bool __isWarnLayer = false;

// Draw 函数最后的一些代码
// 重复代码就不写两次了...
// 优化：如果检测到自己在绘制帧运行中
// 则马上绘制一帧，并减去一次 duration
#define __ADrawTail()                                                        \
    do {                                                                     \
        if (!__isWarnLayer && (layer < 0.0f || layer > 1.0f)) {              \
            __isWarnLayer = true;                                            \
            aLogger->Warning("绘制的图层合法范围为 [0, 1], 已帮您自动修正"); \
        }                                                                    \
        info.layer = std::clamp(layer, 0.0f, 1.0f);                          \
        info.duration = duration;                                            \
        if (__isInPaintTickRunner) {                                         \
            --info.duration;                                                 \
            _basicPainter.Draw(info);                                        \
        }                                                                    \
        if (info.duration <= 0) {                                            \
            break;                                                           \
        }                                                                    \
        if (info.duration == 1) {                                            \
            _basicPainter.singleTickQueue.emplace_back(std::move(info));     \
            __ACheckQueueSize(_basicPainter.singleTickQueue);                \
        } else {                                                             \
            _basicPainter.multiTickQueue.emplace_back(std::move(info));      \
            __ACheckQueueSize(_basicPainter.multiTickQueue);                 \
        }                                                                    \
    } while (false)

void APainter::Draw(const ARect& rect, int duration, float layer) {
    // 跳帧模式下，绘制无效
    if (__aGameControllor.isSkipTick() || duration <= 0)
        return;
    if (!_basicPainter.IsOpen3dAcceleration()) {
        aLogger->Warning("您尚未开启 3D 加速，无法使用绘制类");
        return;
    }
    __ABasicPainter::DrawInfo info;
    info.type = __ABasicPainter::RECT;
    __ABasicPainter::RectInfo rectInfo;
    rectInfo.rect = rect;
    rectInfo.rect.x += rectInfo.rect.width * _basicPainter.posDict[int(rect.pos)][0];
    rectInfo.rect.y += rectInfo.rect.height * _basicPainter.posDict[int(rect.pos)][1];
    rectInfo.color = _rectColor;
    info.var = rectInfo;
    __ADrawTail();
}

void APainter::Draw(const AText& posText, int duration, float layer) {
    // 跳帧模式下，绘制无效
    if (__aGameControllor.isSkipTick() || duration <= 0)
        return;

    if (!_basicPainter.IsOpen3dAcceleration()) {
        aLogger->Warning("您尚未开启 3D 加速，无法使用绘制类");
        return;
    }
    if (posText.text.empty())
        return;
    std::wstring lineText;
    auto wText = AStrToWstr(posText.text);
    int fontSize = _basicPainter.fontSize;
    int width = 0;
    int totalWidth = 0;

    __ABasicPainter::TextInfo textInfo;

    // 对文本进行预处理
    // 遇到 \n 字符时换行
    for (auto&& ch : wText) {
        if (ch != '\n') {
            lineText.push_back(ch);
            width += ch > 0xff ? fontSize : fontSize / 2 + 1;
        } else {
            totalWidth = std::max(width, totalWidth);
            width = 0;
            textInfo.lines.push_back(std::move(lineText));
            lineText.clear();
        }
    }

    if (!lineText.empty()) {
        totalWidth = std::max(width, totalWidth);
        textInfo.lines.push_back(std::move(lineText));
        lineText.clear();
    }
    __ABasicPainter::DrawInfo info;
    int totalHeight = textInfo.lines.size() * _basicPainter.fontSize;

    // 绘制背景框必须在绘制文字之前
    // 不然背景框就会覆盖文字
    if (posText.isHasBkg) {
        // 绘制背景框
        __ABasicPainter::RectInfo rectInfo;
        rectInfo.rect.width = totalWidth;
        rectInfo.rect.height = totalHeight;
        rectInfo.rect.x = posText.x + totalWidth * _basicPainter.posDict[int(posText.pos)][0];
        rectInfo.rect.y = posText.y + totalHeight * _basicPainter.posDict[int(posText.pos)][1];
        rectInfo.color = _rectColor;
        info.var = rectInfo;
        info.type = __ABasicPainter::RECT;
        __ADrawTail();
    }

    textInfo.color = _textColor;
    textInfo.x = posText.x + totalWidth * _basicPainter.posDict[int(posText.pos)][0];
    textInfo.y = posText.y + totalHeight * _basicPainter.posDict[int(posText.pos)][1];
    info.var = std::move(textInfo);
    info.type = __ABasicPainter::TEXT;
    __ADrawTail();
}

void APainter::Draw(const ACursor& cursor, int duration, float layer) {
    // 跳帧模式下，绘制无效
    if (__aGameControllor.isSkipTick() || duration <= 0)
        return;
    __ABasicPainter::DrawInfo info;
    info.var = cursor;
    info.type = __ABasicPainter::CURSOR;
    __ADrawTail();
}

std::vector<std::vector<int>> __ABasicPainter::posDict = {
    {0, -1},
    {0, 0},
    {-1, -1},
    {-1, 0},
};

HCURSOR __ABasicPainter::handCursor = nullptr;
HCURSOR __ABasicPainter::arrowCursor = nullptr;

void __ABasicPainter::ClearFont() {
    DeleteObject(textInfo.HFont);
    textInfo.HFont = nullptr;
    textureDict.clear();
}

void __ABasicPainter::UpdatePaintTime() {
    for (auto painter : GetPainterSet()) {
        // aLogger->Debug("multiTickQueue size : {}", painter->multiTickQueue.size());
        for (auto it = painter->multiTickQueue.begin(); it != painter->multiTickQueue.end();) {
            if (it->duration <= 0)
                it = painter->multiTickQueue.erase(it);
            else {
                --it->duration;
                ++it;
            }
        }
    }
}

void __ABasicPainter::DrawEveryTick() {
    if (AGetMainObject() == nullptr) {
        return;
    }
    if (!__ABasicPainter::IsOpen3dAcceleration()) {
        return;
    }
    for (auto painter : GetPainterSet()) {
        // 优化：如果是单帧的任务，可以直接出队
        for (; !painter->singleTickQueue.empty();) {
            painter->Draw(std::move(painter->singleTickQueue.front()));
            painter->singleTickQueue.pop_front();
        }
        for (auto&& info : painter->multiTickQueue) {
            painter->Draw(info);
        }
    }
    __isInPaintTickRunner = true;
    __aig.tickManagers[ATickRunner::PAINT].RunQueue();
    __isInPaintTickRunner = false;
}

void __ABasicPainter::_BeforeScript() {
    if (!IsOpen3dAcceleration())
        return;
    // InstallDrawHook
    *(uint16_t*)0x54C8CD = 0x5890;
    *(uint32_t*)0x667D0C = (uint32_t)&AsmDraw;
    *(uint32_t*)0x671578 = (uint32_t)&AsmDraw;
    *(uint32_t*)0x676968 = (uint32_t)&AsmDraw;
    static ATickRunner _tickRunner;
    if (!_tickRunner.IsStopped()) {
        return;
    }
    // 优先级设置为最低
    // 以保障这一帧的绘制时间戳都能更新
    _tickRunner.Start([] {
        __ABasicPainter::UpdatePaintTime();
    },
        ATickRunner::GLOBAL, 20);
}

void __ABasicPainter::_ExitFight() {
    if (!IsOpen3dAcceleration())
        return;
    ClearFont();
    singleTickQueue.clear();
    multiTickQueue.clear();
    // UninstallDrawHook
    *(uint16_t*)0x54C8CD = 0xD0FF;
    *(uint32_t*)0x667D0C = 0x54C650;
    *(uint32_t*)0x671578 = 0x54C650;
    *(uint32_t*)0x676968 = 0x54C650;
}

void __ABasicPainter::_AfterInject() {
    // 获取 PvZ 鼠标的样式
    static ATickRunner tickRunner;
    if (!tickRunner.IsStopped())
        return;
    tickRunner.Start([] {
        if (__ABasicPainter::arrowCursor && __ABasicPainter::handCursor) {
            tickRunner.Stop();
            return;
        }
        if (!AGetPvzBase()->MouseWindow()->IsInWindow())
            return;
        CURSORINFO cursorInfo;
        cursorInfo.cbSize = sizeof(CURSORINFO);
        GetCursorInfo(&cursorInfo);
        int cursorType = AGetPvzBase()->MRef<int>(0x4B0);
        if (cursorType == 0) // arrow
            __ABasicPainter::arrowCursor = cursorInfo.hCursor;
        else if (cursorType == 1) // hand
            __ABasicPainter::handCursor = cursorInfo.hCursor;
    },
        ATickRunner::AFTER_INJECT);
}

bool __ABasicPainter::AsmDraw() {
    if (__aig.isReplayPaused) {
        return false;
    }

    static double lastCallTime, lastFinishTime;
    lastCallTime = __AProfiler::CurrentTime();

    // 如果要改动这段代码请咨询零度
    if (__AD3dInfo::device != nullptr) {
        __AD3dInfo::device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 0.0f, 0L);
    }
    // aLogger->Debug("AsmDraw {}", AGetMainObject()->GameClock());
    static int __x = 0;
    __asm__ __volatile__(
        "pushal;"
        "movl 0x6A9EC0, %%ecx;"
        "movl $0x54C650, %%edx;"
        "calll *%%edx;"
        "movl %%eax, %[__x];"
        "popal;"
        : [__x] "=rm"(__x)
        :
        :);

    if (__x) {
        // aLogger->Debug("DrawEveryTick {}", AGetMainObject()->GameClock());
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
        : [__x] "rm"(__x)
        :);
    lastFinishTime = __AProfiler::CurrentTime();
    __aProfiler.paintTime.push_back(lastFinishTime - lastCallTime);
    return __x;
}

void __ABasicPainter::DrawRect(int x, int y, int w, int h, DWORD color, float layer) {
    __AGeoVertex aVertex[4] = {
        {x, y, layer, color},
        {x, y + h, layer, color},
        {x + w, y, layer, color},
        {x + w, y + h, layer, color}};
    textInfo.device->SetTexture(0, nullptr);
    textInfo.device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 68, aVertex, 4, 0);
}

void __ABasicPainter::DrawStr(const std::wstring& text, int x, int y, DWORD color, float layer) {
    for (auto&& ch : text) { // 绘制一行
        auto iter = textureDict.find(ch);
        if (iter == textureDict.end()) { // 将字符串存入字典
            auto info = GetTextNeedInfo();
            if (info == nullptr)
                return;
            textureDict[ch] = std::make_shared<__ATexture>(ch, info);
        }

        textureDict[ch]->Draw(color, x, y, layer);
        x += ch > 0xff ? fontSize : fontSize / 2 + 1;
    }
}

void __ABasicPainter::Draw(const __ABasicPainter::DrawInfo& info) {
    switch (info.type) {
    case RECT: {
        auto&& rectInfo = std::get<RectInfo>(info.var);
        this->DrawRect(rectInfo.rect.x, rectInfo.rect.y,
            rectInfo.rect.width, rectInfo.rect.height,
            rectInfo.color, info.layer);
        break;
    }
    case TEXT: {
        auto&& textInfo = std::get<TextInfo>(info.var);
        int y = textInfo.y;
        for (auto&& line : textInfo.lines) {
            this->DrawStr(line, textInfo.x, y, textInfo.color, info.layer);
            y += this->fontSize;
        }
        break;
    }
    case CURSOR: {
        auto&& cursor = std::get<ACursor>(info.var);
        this->DrawCursor(cursor.x, cursor.y, cursor.type, info.layer);
        break;
    }
    default:
        aLogger->Error("__ABasicPainter::Draw: 参数错误，未知的绘制类型: {}", int(info.type));
        break;
    }
}

void __ABasicPainter::DrawCursor(int x, int y, int type, float layer) {
    static std::unordered_map<int, std::shared_ptr<__ATexture>> m;
    if (m[type] == nullptr) {
        __ACursorInfo cursorInfo;
        auto theArrowCursor = arrowCursor == nullptr ? LoadCursor(NULL, IDC_ARROW) : arrowCursor;
        auto theHandCursor = handCursor == nullptr ? LoadCursor(NULL, IDC_HAND) : handCursor;
        // auto theArrowCursor = LoadCursor(NULL, IDC_ARROW);
        // auto theHandCursor = LoadCursor(NULL, IDC_HAND);
        cursorInfo.hCursor = type == 1 ? theHandCursor : theArrowCursor;
        cursorInfo.type = type;
        m[type] = std::make_shared<__ATexture>(&cursorInfo);
    }
    m[type]->Draw(AArgb(0xff, 0xff, 0xff, 0xff), x, y, layer);
}

__ATextInfo* __ABasicPainter::GetTextNeedInfo() {
    if (!textInfo.HFont) {
        textInfo.HFont = CreateFontW(-fontSize, 0, 0, 0, 100, false, false, false, false, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FIXED_PITCH | FF_DONTCARE, fontName.c_str());
        textInfo.fontW = fontSize * 1.1;
        textInfo.fontH = fontSize * 1.1;
    }

    return textInfo.HFont == nullptr ? nullptr : &textInfo;
}

bool __ABasicPainter::IsOpen3dAcceleration() {
    // 如果要改动这个函数里的代码请咨询零度
    auto p2 = AGetPvzBase()->MPtr<APvzStruct>(0x36C);
    if (!p2)
        return false;
    auto p3 = p2->MPtr<APvzStruct>(0x30);
    if (!p3)
        return false;
    auto surface = p3->MPtr<IDirectDrawSurface7>(0x14);
    __AD3dInfo::device = p3->MPtr<IDirect3DDevice7>(0x20);
    __AD3dInfo::ddraw = p3->MPtr<IDirectDraw7>(0x10);
    auto d3d = p3->MPtr<IDirect3D7>(0x1C);

    if (surface == nullptr
        || __AD3dInfo::device == nullptr
        || __AD3dInfo::ddraw == nullptr
        || d3d == nullptr) {
        return false;
    }
    IDirectDrawSurface7*& zBuffer = ((IDirectDrawSurface7**)p3)[0x18 / sizeof(void*)];
    if (!zBuffer) {
        DDPIXELFORMAT ddpfZBuffer;
        d3d->EnumZBufferFormats(*(IID*)0x6715F4, (LPD3DENUMPIXELFORMATSCALLBACK)0x5649F0, (void*)&ddpfZBuffer);
        DDSURFACEDESC2 ddsd;
        ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
        ddsd.dwSize = sizeof(DDSURFACEDESC2);
        ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
        ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY;
        ddsd.dwWidth = AGetPvzBase()->MRef<int>(0xc0);
        ddsd.dwHeight = AGetPvzBase()->MRef<int>(0xc4);
        memcpy(&ddsd.ddpfPixelFormat, &ddpfZBuffer, sizeof(DDPIXELFORMAT));
        __AD3dInfo::ddraw->CreateSurface(&ddsd, &zBuffer, nullptr);
        surface->AddAttachedSurface(zBuffer);
        __AD3dInfo::device->Clear(0, NULL, D3DCLEAR_TARGET, 0xff000000, 1.0f, 0L);
    }
    *(char*)0x564DC7 = 1; // device->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 1);
    *(char*)0x564DD6 = 1; // device->SetRenderState(D3DRENDERSTATE_ZENABLE, 1);
    __AD3dInfo::device->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_GREATEREQUAL);
    __AD3dInfo::device->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
    __AD3dInfo::device->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
    __AD3dInfo::device->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_POINT);
    __AD3dInfo::device->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);
    __AD3dInfo::device->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTFG_POINT);
    __AD3dInfo::device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    __AD3dInfo::device->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    __AD3dInfo::device->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    return true;
}

IDirect3DDevice7* __AD3dInfo::device = nullptr;
IDirectDraw7* __AD3dInfo::ddraw = nullptr;

__ATexture::__ATexture(wchar_t chr, __ATextInfo* _textInfo)
    : mWidth(0)
    , mHeight(0)
    , u(0)
    , v(0)
    , texture(nullptr)
    , textInfo(_textInfo) {
    wchar_t str[2] = {chr, L'\0'};
    HDC hDc = CreateCompatibleDC(nullptr);
    HFONT anOldFont = (HFONT)SelectObject(hDc, textInfo->HFont);
    HBRUSH anOldBrush = (HBRUSH)SelectObject(hDc, GetStockObject(BLACK_BRUSH));
    RECT aRect = {0, 0, textInfo->fontW, textInfo->fontH + 1};
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
    aHeader.biHeight = -textInfo->fontH - 1;
    aHeader.biPlanes = 1;
    aHeader.biBitCount = 32;
    aHeader.biCompression = BI_RGB;
    DWORD* aBits = nullptr;
    HBITMAP aBitmap = CreateDIBSection(hDc, &aBitmapInfo, DIB_RGB_COLORS, (void**)&aBits, nullptr, 0);
    if (!aBitmap)
        return;
    HBITMAP anOldBitmap = (HBITMAP)SelectObject(hDc, aBitmap);
    Rectangle(hDc, 0, 0, aRect.right, textInfo->fontH + 1);
    SetBkMode(hDc, TRANSPARENT);
    SetTextColor(hDc, RGB(255, 255, 255));
    RECT rect = {0, 1, aRect.right, textInfo->fontH + 1};
    DrawTextW(hDc, str, 1, &rect, DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX);
    GdiFlush();
    SelectObject(hDc, anOldBrush);
    SelectObject(hDc, anOldFont);
    SelectObject(hDc, anOldBitmap);
    mWidth = aRect.right;
    mHeight = textInfo->fontH + 1;
    texture = CreateTextureSurface(mWidth, mHeight);
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

void __ATexture::_DrawBlackBorder(DWORD* bits) {
    // 左右扫描
    for (int i = 0; i < mHeight; ++i) {
        DWORD* preBit = bits;
        for (int j = 0; j < mWidth; ++j) {
            DWORD* curBit = bits + i * mWidth + j;
            if (*preBit != 0xffffff && *curBit == 0xffffff)
                *preBit = 0xff000000; // 左边
            else if (*preBit == 0xffffff && *curBit != 0xffffff)
                *curBit = 0xff000000; // 右边
            preBit = curBit;
        }
    }

    // 上下扫描
    for (int i = 0; i < mWidth; ++i) {
        DWORD* preBit = bits;
        for (int j = 0; j < mHeight; ++j) {
            DWORD* curBit = bits + j * mWidth + i;
            if (*preBit != 0xffffff && *curBit == 0xffffff)
                *preBit = 0xff000000; // 上边
            else if (*preBit == 0xffffff && *curBit != 0xffffff)
                *curBit = 0xff000000; // 下边
            preBit = curBit;
        }
    }
}

void __ATexture::_CopyBitsToSurface(DWORD* src, DDSURFACEDESC2& dst, __ACursorInfo* cursorInfo) {
    bool isSuccessGetPvzCursor = (cursorInfo->type == 0 && cursorInfo->hCursor != LoadCursor(NULL, IDC_ARROW))
        || (cursorInfo->type == 1 && cursorInfo->hCursor != LoadCursor(NULL, IDC_HAND));
    char* surface = (char*)dst.lpSurface;
    if (isSuccessGetPvzCursor) {
        if (cursorInfo->type != 0) {
            // 如果不是箭头，对其加黑色边框
            // 这里不知道为啥不是箭头的时候没有黑边
            // 方法就是左右扫描一遍+上下扫描一遍
            _DrawBlackBorder(src);
        }
        for (int i = 0; i < mHeight; ++i) {
            DWORD* _dst = (DWORD*)surface;
            DWORD* _bits = src;
            for (int j = 0; j < mWidth; ++j) {
                auto val = *_bits++;
                if (val == 0xffffff)
                    *_dst++ = 0xffffffff;
                else if (val == 0)
                    *_dst++ = 0x00000000;
                else
                    *_dst++ = 0xff000000;
            }

            src += mWidth;
            surface += dst.lPitch;
        }
    } else {
        for (int i = 0; i < mHeight; ++i) {
            DWORD* _dst = (DWORD*)surface;
            DWORD* _bits = src;
            for (int j = 0; j < mWidth; ++j)
                *_dst++ = *_bits++;

            src += mWidth;
            surface += dst.lPitch;
        }
    }
}

__ATexture::__ATexture(__ACursorInfo* cursorInfo)
    : mWidth(0)
    , mHeight(0)
    , u(0)
    , v(0)
    , texture(nullptr) {
    mWidth = 1 + GetSystemMetrics(SM_CXCURSOR);
    mHeight = 1 + GetSystemMetrics(SM_CYCURSOR);
    HDC hDc = CreateCompatibleDC(nullptr);
    SelectObject(hDc, GetStockObject(BLACK_BRUSH));
    BITMAPINFO aBitmapInfo;
    memset(&aBitmapInfo, 0, sizeof(aBitmapInfo));
    BITMAPINFOHEADER& aHeader = aBitmapInfo.bmiHeader;
    aHeader.biSize = sizeof(aHeader);
    aHeader.biWidth = mWidth;
    aHeader.biHeight = -mHeight;
    aHeader.biPlanes = 1;
    aHeader.biBitCount = 32;
    aHeader.biCompression = BI_RGB;
    DWORD* aBits = nullptr;
    HBITMAP aBitmap = CreateDIBSection(hDc, &aBitmapInfo, DIB_RGB_COLORS, (void**)&aBits, nullptr, 0);
    if (!aBitmap)
        return;
    HBITMAP anOldBitmap = (HBITMAP)SelectObject(hDc, aBitmap);
    Rectangle(hDc, 0, 0, mWidth, mHeight);
    SetBkMode(hDc, TRANSPARENT);
    SetTextColor(hDc, RGB(0, 255, 255));
    DrawIconEx(hDc, 0, 0, cursorInfo->hCursor, 0, 0, 0, NULL, DI_NORMAL | DI_COMPAT);
    GdiFlush();
    SelectObject(hDc, anOldBitmap);
    texture = CreateTextureSurface(mWidth, mHeight);
    DDSURFACEDESC2 aDesc;
    aDesc.dwSize = sizeof(aDesc);
    texture->Lock(nullptr, &aDesc, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, nullptr);
    u = (float)(mWidth - 1) / aDesc.dwWidth;
    v = (float)(mHeight - 1) / aDesc.dwHeight;
    _CopyBitsToSurface(aBits, aDesc, cursorInfo);
    texture->Unlock(nullptr);
    DeleteObject(aBitmap);
}

__ATexture::~__ATexture() {
    if (texture)
        texture->Release();
}

void __ATexture::Draw(DWORD color, int x, int y, float layer) const {
    __ATexVertex aVertex[4] = {
        {x, y, layer, color, 0, 0},
        {x, y + mHeight, layer, color, 0, v},
        {x + mWidth, y, layer, color, u, 0},
        {x + mWidth, y + mHeight, layer, color, u, v}};
    textInfo->device->SetTexture(0, texture);
    textInfo->device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 452, aVertex, 4, 0);
}

IDirectDrawSurface7* __ATexture::CreateTextureSurface(int theWidth, int theHeight) {
    textInfo->device->SetTexture(0, nullptr);
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
    textInfo->ddraw->CreateSurface(&aDesc, &aSurface, nullptr);
    return aSurface;
}
