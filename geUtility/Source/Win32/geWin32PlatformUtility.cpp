/*****************************************************************************/
/**
 * @file    geWin32PlatformUtility.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/03/11
 * @brief   Provides access to Windows specific utility functions
 *
 * Provides access to various Windows specific utility functions
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include <windows.h>
#include <iphlpapi.h>

#include "gePrerequisitesUtil.h"
#include "Win32/geWin32PlatformUtility.h"
#include "geColor.h"

namespace geEngineSDK {
  void
  PlatformUtility::terminate(bool force) {
    if (!force) {
      PostQuitMessage(0);
    }
    else {
      TerminateProcess(GetCurrentProcess(), 0);
    }
  }

  void
  PlatformUtility::copyToClipboard(const WString& string) {
    HANDLE hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
                               (string.size() + 1) * sizeof(WString::value_type));
    WString::value_type* buffer = static_cast<WString::value_type*>(GlobalLock(hData));

    string.copy(buffer, string.size());
    buffer[string.size()] = '\0';

    GlobalUnlock(hData);

    if (OpenClipboard(NULL)) {
      EmptyClipboard();
      SetClipboardData(CF_UNICODETEXT, hData);
      CloseClipboard();
    }
    else {
      GlobalFree(hData);
    }
  }

  WString
  PlatformUtility::copyFromClipboard() {
    if (OpenClipboard(NULL)) {
      HANDLE hData = GetClipboardData(CF_UNICODETEXT);

      if (hData != NULL) {
        WString::value_type* buffer = static_cast<WString::value_type*>(GlobalLock(hData));
        WString string(buffer);
        GlobalUnlock(hData);
        CloseClipboard();
        return string;
      }
      
      CloseClipboard();
    }

    return L"";
  }

  WString
  PlatformUtility::keyCodeToUnicode(uint32 keyCode) {
    static HKL keyboardLayout = GetKeyboardLayout(0);
    static uint8 keyboarState[256];

    if (FALSE == GetKeyboardState(keyboarState)) {
      return 0;
    }

    uint32 virtualKey = MapVirtualKeyExW(keyCode, 1, keyboardLayout);

    UNICHAR output[2];
    int count = ToUnicodeEx(virtualKey,
                            keyCode,
                            keyboarState,
                            output,
                            2,
                            0,
                            keyboardLayout);
    if (0 < count) {
      return WString(output, count);
    }

    return StringUtil::WBLANK;
  }

  bool
  PlatformUtility::getMACAddress(MACAddress& address) {
    std::memset(&address, 0, sizeof(address));

    PIP_ADAPTER_INFO adapterInfo = ge_alloc<IP_ADAPTER_INFO>();
    ULONG len = sizeof(IP_ADAPTER_INFO);
    DWORD rc = GetAdaptersInfo(adapterInfo, &len);

    if (ERROR_BUFFER_OVERFLOW == rc) {
      ge_free(adapterInfo);
      adapterInfo = reinterpret_cast<IP_ADAPTER_INFO*>(ge_alloc(len));
    }
    else if (ERROR_SUCCESS != rc) {
      ge_free(adapterInfo);
      return false;
    }

    if (NO_ERROR == GetAdaptersInfo(adapterInfo, &len)) {
      PIP_ADAPTER_INFO curAdapter = nullptr;
      curAdapter = adapterInfo;

      while (curAdapter) {
        if (MIB_IF_TYPE_ETHERNET == curAdapter->Type && 
            sizeof(address) == curAdapter->AddressLength) {
          std::memcpy(&address, curAdapter->Address, curAdapter->AddressLength);
          return true;
        }

        curAdapter = curAdapter->Next;
      }
    }

    ge_free(adapterInfo);
    return false;
  }

  String
  PlatformUtility::generateUUID() {
    UUID uuid;
    UuidCreate(&uuid);

    uint8* uuidStr;
    UuidToStringA(&uuid, &uuidStr);

    String output((char*)uuidStr);
    RpcStringFreeA(&uuidStr);

    return output;
  }

  void
  PlatformUtility::open(const Path& path) {
    ShellExecute(nullptr, "open", path.toString().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
  }

  HBITMAP
  Win32PlatformUtility::createBitmap(const Color* pixels,
                                     uint32 width,
                                     uint32 height,
                                     bool premultiplyAlpha) {
    BITMAPINFO bi;

    memset(&bi, 0, sizeof(BITMAPINFO));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = width;
    bi.bmiHeader.biHeight = height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    HDC hDC = GetDC(nullptr);

    void* data = nullptr;
    HBITMAP hBitmap = CreateDIBSection(hDC,
                                       &bi,
                                       DIB_RGB_COLORS,
                                       reinterpret_cast<void**>(&data),
                                       nullptr,
                                       0);
    HDC hBitmapDC = CreateCompatibleDC(hDC);
    ReleaseDC(nullptr, hDC);

    //Select the bitmaps to DC
    HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(hBitmapDC, hBitmap));

    //Scan each pixel of the source bitmap and create the masks
    Color pixel;
    DWORD* dst = (DWORD*)data;
    for (uint32 y = 0; y < height; ++y) {
      for (uint32 x = 0; x < width; ++x) {
        uint32 revY = height - y - 1;
        pixel = pixels[revY*width + x];

        if (premultiplyAlpha) {
          pixel.R *= pixel.A;
          pixel.G *= pixel.A;
          pixel.B *= pixel.A;
        }

        *dst = pixel.DWColor();
        dst++;
      }
    }

    SelectObject(hBitmapDC, hOldBitmap);
    DeleteDC(hBitmapDC);

    return hBitmap;
  }
}
