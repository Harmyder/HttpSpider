#include "stdafx.h"

#include "MVVM\ViewModel\MainViewModel.h"

#include "MVVM\Internal\RelayCommand.h"
#include "BusinessLogic\Spider.h"

using namespace std;

namespace MVVM
{
    MainViewModel::MainViewModel() : 
        PropertyChanged(this),
        _start(CreateRelayCommand([this] { this->StartSpider(); })),
        _stop(CreateRelayCommand([this] { this->StopSpider(); })),
        _httpSpider(new BusinessLogic::Spider())
    {
    }

    void MainViewModel::InitView()
    {
        CanStart(true);
        CanStop(false);
        ThreadsCount(_threadsCount);
        UrlsCount(_urlsCount);
        StartUrl(_startUrl.c_str());
        Text(_text.c_str());

        PropertyChanged(PropertyChangedEventArgs("CanStart"));
        PropertyChanged(PropertyChangedEventArgs("CanStop"));
        PropertyChanged(PropertyChangedEventArgs("ThreadsCount"));
        PropertyChanged(PropertyChangedEventArgs("UrlsCount"));
        PropertyChanged(PropertyChangedEventArgs("StartUrl"));
        PropertyChanged(PropertyChangedEventArgs("Text"));
    }

    // ******************************************************************
    void MainViewModel::SetHWND(HWND hWnd)
    {
        _httpSpider->SetHWND(hWnd);
    }

    // ******************************************************************
    int MainViewModel::ThreadsCount()
    {
        return _threadsCount;
    }
    void MainViewModel::ThreadsCount(int value, bool notify)
    {
        if (_threadsCount != value)
        {
            _threadsCount = value;
            if (notify) PropertyChanged(PropertyChangedEventArgs("ThreadsCount"));
        }
    }

    // ******************************************************************
    int MainViewModel::UrlsCount()
    {
        return _urlsCount;
    }
    void MainViewModel::UrlsCount(int value, bool notify)
    {
        if (_urlsCount != value)
        {
            _urlsCount = value;
            if (notify) PropertyChanged(PropertyChangedEventArgs("UrlsCount"));
        }
    }

    // ******************************************************************
    const char* MainViewModel::StartUrl()
    {
        return _startUrl.c_str();
    }
    void MainViewModel::StartUrl(const char* value, bool notify)
    {
        if (_startUrl != value)
        {
            _startUrl = value;
            if (notify) PropertyChanged(PropertyChangedEventArgs("StartUrl"));
        }
    }

    // ******************************************************************
    const char* MainViewModel::Text()
    {
        return _text.c_str();
    }
    void MainViewModel::Text(const char* value, bool notify)
    {
        if (_text != value)
        {
            _text = value;
            if (notify) PropertyChanged(PropertyChangedEventArgs("Text"));
        }
    }

    // ******************************************************************
    void MainViewModel::StartSpider()
    {
        CanStart(false);
        CanStop(true);

        _httpSpider->Start(_startUrl, _urlsCount, _threadsCount, _text);
    }

    void MainViewModel::StopSpider()
    {
        CanStop(false);
        _httpSpider->Stop();
        CanStart(true);
    }

    void MainViewModel::CanStart(bool value)
    {
        if (value != _canStart)
        {
            _canStart = value;
            PropertyChanged(PropertyChangedEventArgs("CanStart"));
        }
    }

    void MainViewModel::CanStop(bool value)
    {
        if (value != _canStop)
        {
            _canStop = value;
            PropertyChanged(PropertyChangedEventArgs("CanStop"));
        }
    }

    const char* MainViewModel::GetLink(int i)
    {
        return _httpSpider->GetLink(i);
    }

}