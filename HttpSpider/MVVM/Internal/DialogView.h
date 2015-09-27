#pragma once

#include "MVVM\Internal\ViewBase.h"

namespace MVVM
{
    class DialogView : public ViewBase
    {
    public:
        virtual void StartDialog() = 0;
        virtual INT_PTR DialogProc(HWND const window, UINT const messageId, WPARAM const wParam, LPARAM const lParam) = 0;
    };
}