/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-13 13:05:00
 * @Description:
 */

#include "avz_connector.h"

void ATimeConnectHandle::Stop()
{
    if (_iter) {
        if (ANowTime(_time.wave) < _time.time) { // 操作尚未运行
            (*_iter)->second.isStopped = true;
            return;
        }
    }
    auto&& pattern = __aInternalGlobal.loggerPtr->GetPattern();
    __aInternalGlobal.loggerPtr->Warning("时间连接 [" + pattern + ", " + pattern + "] 连接已失效, Stop 无效调用",
        _time.wave, _time.time);
}

std::vector<ATimeConnectHandle> AConnect(const ATime& time, ARelOp&& reOp)
{
    std::vector<ATimeConnectHandle> ret;
    int wave = time.wave;
    for (auto&& op : reOp.OpVec) {
        ret.push_back(AConnect(ATime(wave, time.time + op.relativeTime), std::move(op.operation)));
    }
    return ret;
}

std::vector<ATimeConnectHandle> AConnect(const ATime& time, const ARelOp& reOp)
{
    std::vector<ATimeConnectHandle> ret;
    int wave = time.wave;
    for (auto&& op : reOp.OpVec) {
        ret.push_back(AConnect(ATime(wave, time.time + op.relativeTime), op.operation));
    }
    return ret;
}

void __AConnectVec::ExitFight()
{
    for (auto tick : tickVec) {
        delete tick;
    }

    tickVec.clear();
}

std::vector<std::string> __AKeyManager::_keyVec;
std::unordered_map<AKey, AConnectHandle> __AKeyManager::_keyMap;
__AKeyManager __akm; // AStateHook

__AKeyManager::KeyState __AKeyManager::ToVaildKey(AKey& key)
{
    KeyState keyState = VALID;

    if (key >= _keyVec.size() || _keyVec[key].empty()) {
        keyState = UNKNOWN;
    }

    if (keyState == UNKNOWN) {
        __aInternalGlobal.loggerPtr->Error("不允许绑定未知的按键 : " + //
                __aInternalGlobal.loggerPtr->GetPattern(),
            key);
        return UNKNOWN;
    }
    auto iter = _keyMap.find(key);
    if (iter == _keyMap.end() || iter->second.isStopped()) {
        return VALID;
    } else {
        __aInternalGlobal.loggerPtr->Error("按键 : " + //
                __aInternalGlobal.loggerPtr->GetPattern()
                + "已被绑定, AConnect(AKey, AOperation) 不允许重复绑定按键",
            __AKeyManager::ToName(key));
        return REPEAT;
    }
}

__AKeyManager::__AKeyManager()
{
    _keyVec.resize(0xFF);

#define __AddKeyMap(keyName) _keyVec[keyName] = #keyName

    /*
     * Virtual Keys, Standard Set
     */
    __AddKeyMap(VK_LBUTTON);
    __AddKeyMap(VK_RBUTTON);
    __AddKeyMap(VK_CANCEL);
    __AddKeyMap(VK_MBUTTON); /* NOT contiguous with L & RBUTTON */

#if (_WIN32_WINNT >= 0x0500)
    __AddKeyMap(VK_XBUTTON1); /* NOT contiguous with L & RBUTTON */
    __AddKeyMap(VK_XBUTTON2); /* NOT contiguous with L & RBUTTON */
#endif                        /* _WIN32_WINNT >= 0x0500 */

    /*
     * 0x07 : reserved
     */

    __AddKeyMap(VK_BACK);
    __AddKeyMap(VK_TAB);

    /*
     * 0x0A - 0x0B : reserved
     */

    __AddKeyMap(VK_CLEAR);
    __AddKeyMap(VK_RETURN);

    /*
     * 0x0E - 0x0F : unassigned
     */

    __AddKeyMap(VK_SHIFT);
    __AddKeyMap(VK_CONTROL);
    __AddKeyMap(VK_MENU);
    __AddKeyMap(VK_PAUSE);
    __AddKeyMap(VK_CAPITAL);

    __AddKeyMap(VK_KANA);
    __AddKeyMap(VK_HANGEUL); /* old name - should be here for compatibility */
    __AddKeyMap(VK_HANGUL);
    // __AddKeyMap(VK_IME_ON);
    __AddKeyMap(VK_JUNJA);
    __AddKeyMap(VK_FINAL);
    __AddKeyMap(VK_HANJA);
    __AddKeyMap(VK_KANJI);
    // __AddKeyMap(VK_IME_OFF);

    __AddKeyMap(VK_ESCAPE);

    __AddKeyMap(VK_CONVERT);
    __AddKeyMap(VK_NONCONVERT);
    __AddKeyMap(VK_ACCEPT);
    __AddKeyMap(VK_MODECHANGE);

    __AddKeyMap(VK_SPACE);
    __AddKeyMap(VK_PRIOR);
    __AddKeyMap(VK_NEXT);
    __AddKeyMap(VK_END);
    __AddKeyMap(VK_HOME);
    __AddKeyMap(VK_LEFT);
    __AddKeyMap(VK_UP);
    __AddKeyMap(VK_RIGHT);
    __AddKeyMap(VK_DOWN);
    __AddKeyMap(VK_SELECT);
    __AddKeyMap(VK_PRINT);
    __AddKeyMap(VK_EXECUTE);
    __AddKeyMap(VK_SNAPSHOT);
    __AddKeyMap(VK_INSERT);
    __AddKeyMap(VK_DELETE);
    __AddKeyMap(VK_HELP);

    /*
     * VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
     * 0x3A - 0x40 : unassigned
     * VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
     */

    __AddKeyMap('0');
    __AddKeyMap('1');
    __AddKeyMap('2');
    __AddKeyMap('3');
    __AddKeyMap('4');
    __AddKeyMap('5');
    __AddKeyMap('6');
    __AddKeyMap('7');
    __AddKeyMap('8');
    __AddKeyMap('9');

    __AddKeyMap('A');
    __AddKeyMap('B');
    __AddKeyMap('C');
    __AddKeyMap('D');
    __AddKeyMap('E');
    __AddKeyMap('F');
    __AddKeyMap('G');
    __AddKeyMap('H');
    __AddKeyMap('I');
    __AddKeyMap('J');
    __AddKeyMap('K');
    __AddKeyMap('L');
    __AddKeyMap('M');
    __AddKeyMap('N');
    __AddKeyMap('O');
    __AddKeyMap('P');
    __AddKeyMap('Q');
    __AddKeyMap('R');
    __AddKeyMap('S');
    __AddKeyMap('T');
    __AddKeyMap('U');
    __AddKeyMap('V');
    __AddKeyMap('W');
    __AddKeyMap('X');
    __AddKeyMap('Y');
    __AddKeyMap('Z');

    __AddKeyMap(VK_LWIN);
    __AddKeyMap(VK_RWIN);
    __AddKeyMap(VK_APPS);

    /*
     * 0x5E : reserved
     */

    __AddKeyMap(VK_SLEEP);

    __AddKeyMap(VK_NUMPAD0);
    __AddKeyMap(VK_NUMPAD1);
    __AddKeyMap(VK_NUMPAD2);
    __AddKeyMap(VK_NUMPAD3);
    __AddKeyMap(VK_NUMPAD4);
    __AddKeyMap(VK_NUMPAD5);
    __AddKeyMap(VK_NUMPAD6);
    __AddKeyMap(VK_NUMPAD7);
    __AddKeyMap(VK_NUMPAD8);
    __AddKeyMap(VK_NUMPAD9);
    __AddKeyMap(VK_MULTIPLY);
    __AddKeyMap(VK_ADD);
    __AddKeyMap(VK_SEPARATOR);
    __AddKeyMap(VK_SUBTRACT);
    __AddKeyMap(VK_DECIMAL);
    __AddKeyMap(VK_DIVIDE);
    __AddKeyMap(VK_F1);
    __AddKeyMap(VK_F2);
    __AddKeyMap(VK_F3);
    __AddKeyMap(VK_F4);
    __AddKeyMap(VK_F5);
    __AddKeyMap(VK_F6);
    __AddKeyMap(VK_F7);
    __AddKeyMap(VK_F8);
    __AddKeyMap(VK_F9);
    __AddKeyMap(VK_F10);
    __AddKeyMap(VK_F11);
    __AddKeyMap(VK_F12);
    __AddKeyMap(VK_F13);
    __AddKeyMap(VK_F14);
    __AddKeyMap(VK_F15);
    __AddKeyMap(VK_F16);
    __AddKeyMap(VK_F17);
    __AddKeyMap(VK_F18);
    __AddKeyMap(VK_F19);
    __AddKeyMap(VK_F20);
    __AddKeyMap(VK_F21);
    __AddKeyMap(VK_F22);
    __AddKeyMap(VK_F23);
    __AddKeyMap(VK_F24);

#if (_WIN32_WINNT >= 0x0604)

    /*
     * 0x88 - 0x8F : UI navigation
     */

    __AddKeyMap(VK_NAVIGATION_VIEW);   // reserved
    __AddKeyMap(VK_NAVIGATION_MENU);   // reserved
    __AddKeyMap(VK_NAVIGATION_UP);     // reserved
    __AddKeyMap(VK_NAVIGATION_DOWN);   // reserved
    __AddKeyMap(VK_NAVIGATION_LEFT);   // reserved
    __AddKeyMap(VK_NAVIGATION_RIGHT);  // reserved
    __AddKeyMap(VK_NAVIGATION_ACCEPT); // reserved
    __AddKeyMap(VK_NAVIGATION_CANCEL); // reserved

#endif /* _WIN32_WINNT >= 0x0604 */

    __AddKeyMap(VK_NUMLOCK);
    __AddKeyMap(VK_SCROLL);

    /*
     * NEC PC-9800 kbd definitions
     */
    // __AddKeyMap(VK_OEM_NEC_EQUAL); // '=' key on numpad

    /*
     * Fujitsu/OASYS kbd definitions
     */
    // __AddKeyMap(VK_OEM_FJ_JISHO);   // 'Dictionary' key
    // __AddKeyMap(VK_OEM_FJ_MASSHOU); // 'Unregister word' key
    // __AddKeyMap(VK_OEM_FJ_TOUROKU); // 'Register word' key
    // __AddKeyMap(VK_OEM_FJ_LOYA);    // 'Left OYAYUBI' key
    // __AddKeyMap(VK_OEM_FJ_ROYA);    // 'Right OYAYUBI' key

    /*
     * 0x97 - 0x9F : unassigned
     */

    /*
     * VK_L* & VK_R* - left and right Alt, Ctrl and Shift virtual keys.
     * Used only as parameters to GetAsyncKeyState() and GetKeyState().
     * No other API or message will distinguish left and right keys in this way.
     */
    __AddKeyMap(VK_LSHIFT);
    __AddKeyMap(VK_RSHIFT);
    __AddKeyMap(VK_LCONTROL);
    __AddKeyMap(VK_RCONTROL);
    __AddKeyMap(VK_LMENU);
    __AddKeyMap(VK_RMENU);

#if (_WIN32_WINNT >= 0x0500)
    __AddKeyMap(VK_BROWSER_BACK);
    __AddKeyMap(VK_BROWSER_FORWARD);
    __AddKeyMap(VK_BROWSER_REFRESH);
    __AddKeyMap(VK_BROWSER_STOP);
    __AddKeyMap(VK_BROWSER_SEARCH);
    __AddKeyMap(VK_BROWSER_FAVORITES);
    __AddKeyMap(VK_BROWSER_HOME);

    __AddKeyMap(VK_VOLUME_MUTE);
    __AddKeyMap(VK_VOLUME_DOWN);
    __AddKeyMap(VK_VOLUME_UP);
    __AddKeyMap(VK_MEDIA_NEXT_TRACK);
    __AddKeyMap(VK_MEDIA_PREV_TRACK);
    __AddKeyMap(VK_MEDIA_STOP);
    __AddKeyMap(VK_MEDIA_PLAY_PAUSE);
    __AddKeyMap(VK_LAUNCH_MAIL);
    __AddKeyMap(VK_LAUNCH_MEDIA_SELECT);
    __AddKeyMap(VK_LAUNCH_APP1);
    __AddKeyMap(VK_LAUNCH_APP2);

#endif /* _WIN32_WINNT >= 0x0500 */

    /*
     * 0xB8 - 0xB9 : reserved
     */

    __AddKeyMap(VK_OEM_1); // ';:' for US
    // __AddKeyMap(VK_OEM_PLUS);   // '+' any country
    // __AddKeyMap(VK_OEM_COMMA);  // ',' any country
    // __AddKeyMap(VK_OEM_MINUS);  // '-' any country
    // __AddKeyMap(VK_OEM_PERIOD); // '.' any country
    __AddKeyMap(VK_OEM_2); // '/?' for US
    __AddKeyMap(VK_OEM_3); // '`~' for US

    /*
     * 0xC1 - 0xC2 : reserved
     */

#if (_WIN32_WINNT >= 0x0604)

    /*
     * 0xC3 - 0xDA : Gamepad input
     */

    __AddKeyMap(VK_GAMEPAD_A);                       // reserved
    __AddKeyMap(VK_GAMEPAD_B);                       // reserved
    __AddKeyMap(VK_GAMEPAD_X);                       // reserved
    __AddKeyMap(VK_GAMEPAD_Y);                       // reserved
    __AddKeyMap(VK_GAMEPAD_RIGHT_SHOULDER);          // reserved
    __AddKeyMap(VK_GAMEPAD_LEFT_SHOULDER);           // reserved
    __AddKeyMap(VK_GAMEPAD_LEFT_TRIGGER);            // reserved
    __AddKeyMap(VK_GAMEPAD_RIGHT_TRIGGER);           // reserved
    __AddKeyMap(VK_GAMEPAD_DPAD_UP);                 // reserved
    __AddKeyMap(VK_GAMEPAD_DPAD_DOWN);               // reserved
    __AddKeyMap(VK_GAMEPAD_DPAD_LEFT);               // reserved
    __AddKeyMap(VK_GAMEPAD_DPAD_RIGHT);              // reserved
    __AddKeyMap(VK_GAMEPAD_MENU);                    // reserved
    __AddKeyMap(VK_GAMEPAD_VIEW);                    // reserved
    __AddKeyMap(VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON);  // reserved
    __AddKeyMap(VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON); // reserved
    __AddKeyMap(VK_GAMEPAD_LEFT_THUMBSTICK_UP);      // reserved
    __AddKeyMap(VK_GAMEPAD_LEFT_THUMBSTICK_DOWN);    // reserved
    __AddKeyMap(VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT);   // reserved
    __AddKeyMap(VK_GAMEPAD_LEFT_THUMBSTICK_LEFT);    // reserved
    __AddKeyMap(VK_GAMEPAD_RIGHT_THUMBSTICK_UP);     // reserved
    __AddKeyMap(VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN);   // reserved
    __AddKeyMap(VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT);  // reserved
    __AddKeyMap(VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT);   // reserved

#endif /* _WIN32_WINNT >= 0x0604 */

    __AddKeyMap(VK_OEM_4); //  '[{' for US
    __AddKeyMap(VK_OEM_5); //  '\|' for US
    __AddKeyMap(VK_OEM_6); //  ']}' for US
    __AddKeyMap(VK_OEM_7); //  ''"' for US
    __AddKeyMap(VK_OEM_8);

    /*
     * 0xE0 : reserved
     */

    /*
     * Various extended or enhanced keyboards
     */
    // __AddKeyMap(VK_OEM_AX);   //  'AX' key on Japanese AX kbd
    // __AddKeyMap(VK_OEM_102);  //  "<>" or "\|" on RT 102-key kbd.
    // __AddKeyMap(VK_ICO_HELP); //  Help key on ICO
    // __AddKeyMap(VK_ICO_00);   //  00 key on ICO

#if (WINVER >= 0x0400)
    __AddKeyMap(VK_PROCESSKEY);
#endif /* WINVER >= 0x0400 */

    // __AddKeyMap(VK_ICO_CLEAR);

#if (_WIN32_WINNT >= 0x0500)
    __AddKeyMap(VK_PACKET);
#endif /* _WIN32_WINNT >= 0x0500 */

    /*
     * 0xE8 : unassigned
     */

    /*
     * Nokia/Ericsson definitions
     */
    // __AddKeyMap(VK_OEM_RESET);
    // __AddKeyMap(VK_OEM_JUMP);
    // __AddKeyMap(VK_OEM_PA1);
    // __AddKeyMap(VK_OEM_PA2);
    // __AddKeyMap(VK_OEM_PA3);
    // __AddKeyMap(VK_OEM_WSCTRL);
    // __AddKeyMap(VK_OEM_CUSEL);
    // __AddKeyMap(VK_OEM_ATTN);
    // __AddKeyMap(VK_OEM_FINISH);
    // __AddKeyMap(VK_OEM_COPY);
    // __AddKeyMap(VK_OEM_AUTO);
    // __AddKeyMap(VK_OEM_ENLW);
    // __AddKeyMap(VK_OEM_BACKTAB);

    __AddKeyMap(VK_ATTN);
    __AddKeyMap(VK_CRSEL);
    __AddKeyMap(VK_EXSEL);
    __AddKeyMap(VK_EREOF);
    __AddKeyMap(VK_PLAY);
    __AddKeyMap(VK_ZOOM);
    __AddKeyMap(VK_NONAME);
    __AddKeyMap(VK_PA1);
    __AddKeyMap(VK_OEM_CLEAR);
}
