#include "stdafx.h"

#include "MVVM\View\MainView.h"
#include "MVVM\ViewModel\MainViewModel.h"
#include "MVVM\Internal\ICommand.h"
#include "BusinessLogic\LinkConstants.h"

#include <windowsx.h>
#include "Resources\Resource.h"

using namespace std;

namespace MVVM
{
    DEFINE_SINGLETON(MainView);

    INT_PTR __stdcall DialogProcWrapper(HWND const window, UINT const messageId, WPARAM const wParam, LPARAM const lParam)
    {
        return MainView::GetInstance().DialogProc(window, messageId, wParam, lParam);
    }

    void MainView::StartDialog()
    {
        _dataContext = make_unique<MainViewModel>();
        _dataContext->PropertyChanged += bind(&MainView::HandlePropertyChanged, this, placeholders::_1, placeholders::_2);

        const char* dlgResourceId = MAKEINTRESOURCE(IDD_MAIN_DIALOG);
        DialogBox(GetModuleHandle(nullptr), dlgResourceId, 0, &DialogProcWrapper);
    }


    void MainView::HandlePropertyChanged(const void* sender, const PropertyChangedEventArgs& args)
    {
        _ASSERT(sender == _dataContext.get());

        if (args.PropertyName == "CanStart")
        {
            bool enable = _dataContext->CanStart();
            EnableWindow(_buttonStart, enable);
        }
        else if (args.PropertyName == "CanStop")
        {
            bool enable = _dataContext->CanStop();
            EnableWindow(_buttonStop, enable);
        }
        else if (args.PropertyName == "ThreadsCount")
        {
            SetDlgItemText(_window, IDC_THREADS_COUNT, to_string(_dataContext->ThreadsCount()).c_str());
        }
        else if (args.PropertyName == "UrlsCount")
        {
            SetDlgItemText(_window, IDC_URLS_COUNT, to_string(_dataContext->UrlsCount()).c_str());
        }
        else if (args.PropertyName == "StartUrl")
        {
            SetDlgItemText(_window, IDC_START_URL, _dataContext->StartUrl());
        }
        else if (args.PropertyName == "Text")
        {
            SetDlgItemText(_window, IDC_TEXT, _dataContext->Text());
        }
    }

    void MainView::DlgOnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
    {
        UNREFERENCED_PARAMETER(hwnd);
        UNREFERENCED_PARAMETER(hwndCtl);
        UNREFERENCED_PARAMETER(codeNotify);

        switch (id)
        {
        case IDSTART:
            _ASSERT(_dataContext->CanStart());
            PrepareForStart();
            _dataContext->Start()->Execute();
            break;
        case IDSTOP:
            _ASSERT(_dataContext->CanStop());
            _dataContext->Stop()->Execute();
            break;
        case IDC_URLS_COUNT: {
            char buffer[255];
            GetWindowText(_editUrlsCount, buffer, 255);
            _dataContext->UrlsCount(stoi(buffer), false);
        } break;
        case IDC_THREADS_COUNT: {
            char buffer[255];
            GetWindowText(_editThreadsCount, buffer, 255);
            _dataContext->ThreadsCount(stoi(buffer), false);
        } break;
        case IDC_START_URL: {
            char buffer[255];
            GetWindowText(_editStartUrl, buffer, 255);
            _dataContext->StartUrl(buffer, false);
        } break;
        case IDC_TEXT: {
            char buffer[255];
            GetWindowText(_editText, buffer, 255);
            _dataContext->Text(buffer, false);
        } break;
        }
    }

    void MainView::InitDialog()
    {
        _editThreadsCount = GetDlgItem(_window, IDC_THREADS_COUNT);
        _editStartUrl = GetDlgItem(_window, IDC_START_URL);
        _editText = GetDlgItem(_window, IDC_TEXT);
        _editUrlsCount = GetDlgItem(_window, IDC_URLS_COUNT);
        _buttonStart = GetDlgItem(_window, IDSTART);
        _buttonStop = GetDlgItem(_window, IDSTOP);
        _listScanning = GetDlgItem(_window, IDL_URLS_SCANNING);
        _listScanned = GetDlgItem(_window, IDL_URLS_SCANNED);

        _dataContext->SetHWND(_window);
        _dataContext->InitView();
    }

    INT_PTR MainView::DialogProc(HWND const window, UINT const messageId, WPARAM const wParam, LPARAM const lParam)
    {
        switch (messageId)
        {
        case WM_COMMAND:
            return (SetDlgMsgResult(window, messageId, HANDLE_WM_COMMAND((window), (wParam), (lParam), (DlgOnCommand))));
        case WM_CLOSE:
            EndDialog(window, IDOK); 
            ExitProcess(0);
            break;
        case WM_INITDIALOG:
            _window = window;
            InitDialog();
            break;
        case UWM_STARTED: {
            int linkIndex = wParam;
            HandleStarted(linkIndex);
        }   break;
        case UWM_DOWNLOADING: {
            int linkIndex = wParam;
            HandleDownloading(linkIndex);
        }   break;
        case UWM_FINISHED: {
            int linkIndex = wParam;
            int errorId = LOWORD(lParam);
            int foundCount = HIWORD(lParam);
            HandleFinished(linkIndex, errorId, foundCount);
        }   break;
        case UWM_CONNECT_TASK_FINISHED:
            _dataContext->Stop()->Execute();
            break;
        }

        return 0;
    }

    void MainView::HandleStarted(int linkIndex)
    {
        const int index = SendMessage(_listScanning, LB_ADDSTRING, 0, (LPARAM)(string("STARTING ") + string(_dataContext->GetLink(linkIndex) + 7)).c_str());
        _scanning[linkIndex] = index;
        _scanningReverse[index] = linkIndex;
    }

    void MainView::HandleDownloading(int linkIndex)
    {
        int lbIndex = _scanning[linkIndex];
        SendMessage(_listScanning, LB_DELETESTRING, lbIndex, 0);
        SendMessage(_listScanning, LB_INSERTSTRING, lbIndex, (LPARAM)(string("DOWNLOADING ") + string(_dataContext->GetLink(linkIndex) + 7)).c_str());
    }

    void MainView::HandleFinished(int linkIndex, int errorId, int foundCount)
    {
        auto iterator = _scanningReverse.upper_bound(INT_MAX);
        int linkIndexAtMaxLbIndex = iterator->second;
        int maxLbIndex = iterator->first;
        SendMessage(_listScanning, LB_DELETESTRING, maxLbIndex, 0);
        _scanningReverse.erase(iterator);
        int lbIndexToUpdate = _scanning[linkIndex];
        _scanning.erase(linkIndex);
        _scanning[linkIndexAtMaxLbIndex] = lbIndexToUpdate;
        _scanningReverse[lbIndexToUpdate] = linkIndexAtMaxLbIndex;
        SendMessage(_listScanning, LB_DELETESTRING, lbIndexToUpdate, 0);
        string error;
        string found;
        switch (errorId)
        {
        case ERROR_SUCCESS_ID:
            error = ERROR_SUCCESS_STRING;
            break;
        case ERROR_CONNECTING_ID:
            error = ERROR_CONNECTING_STRING;
            break;
        case ERROR_RECEIVING_ID:
            error = ERROR_RECEIVING_STRING;
            break;
        }
        found = to_string(foundCount);
        SendMessage(_listScanned, LB_INSERTSTRING, 0, (LPARAM)(error + " " + found + " " + string(_dataContext->GetLink(linkIndex) + 7)).c_str());
    }

    void MainView::PrepareForStart()
    {
        _scanning.clear();
        _scanningReverse.clear();
        SendMessage(_listScanning, LB_RESETCONTENT, 0, 0);
        SendMessage(_listScanned, LB_RESETCONTENT, 0, 0);
    }

}
