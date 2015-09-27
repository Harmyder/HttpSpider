#pragma once

#include "MVVM\Internal\DialogView.h"
#include "SDK\Singleton.h"

namespace MVVM
{
    class MainViewModel;
    struct PropertyChangedEventArgs;

    class MainView : public DialogView
    {
        // The only reason for the class to be a singleton is DialogProcWrapper.
        // Also we can maintain set of HWND, MainView pairs and choose needed in the function.
        DECLARE_CLASS_SINGLETON(MainView);

    public:
        void StartDialog() override;

        void HandlePropertyChanged(const void* sender, const PropertyChangedEventArgs& args);

        INT_PTR DialogProc(HWND const window, UINT const messageId, WPARAM const wParam, LPARAM const lParam) override;

    private:
        void DlgOnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
        void InitDialog();

        void HandleStarted(int linkIndex);
        void HandleDownloading(int linkIndex);
        void HandleFinished(int linkIndex, int errorId, int foundCount);

        void PrepareForStart();

    private:
        std::unique_ptr<MainViewModel> _dataContext;

        HWND _window;
        HWND _editThreadsCount;
        HWND _editStartUrl;
        HWND _editText;
        HWND _editUrlsCount;
        HWND _buttonStart;
        HWND _buttonStop;
        HWND _listScanning;
        HWND _listScanned;

        // *************************************************************************************
        // This part is certainly not from MVVM, but time is expiring

        // Key is a link index, Value is a position in listbox
        std::unordered_map<int, int> _scanning;
        std::map<int, int, std::greater<int>> _scanningReverse;
    };
}
