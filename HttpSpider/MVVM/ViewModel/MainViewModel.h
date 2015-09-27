#pragma once

#include "MVVM\Internal\ViewModelBase.h"

#include "MVVM\Internal\PropertyChangedEventArgs.h"

namespace BusinessLogic
{
    class Spider;
}

namespace MVVM
{
    struct ICommand;

    class MainViewModel : public ViewModelBase
    {
    public:
        MainViewModel();

        void InitView() override;
        void SetHWND(HWND hWnd);

        PropertyChangedEvent<MainViewModel> PropertyChanged;

        ICommand* Start() { return _start.get(); }
        ICommand* Stop() { return _stop.get(); }
        
        bool CanStart() { return _canStart; }
        void CanStart(bool value);
        bool CanStop() { return _canStop; }
        void CanStop(bool value);

        int ThreadsCount();
        void ThreadsCount(int threadsCount, bool notify = true);

        int UrlsCount();
        void UrlsCount(int urlsCount, bool notify = true);

        const char* StartUrl();
        void StartUrl(const char* startUrl, bool notify = true);

        const char* Text();
        void Text(const char* text, bool notify = true);

        const char* GetLink(int i);

    private:
        void StartSpider();
        void StopSpider();

    private:
        std::unique_ptr<ICommand> _start;
        std::unique_ptr<ICommand> _stop;

        std::unique_ptr<BusinessLogic::Spider> _httpSpider;

        bool _canStart = true;
        bool _canStop = false;

        int _threadsCount = 2;
        int _urlsCount = 100;
        std::string _startUrl = std::string("http://www.w3.org/");
        std::string _text = std::string("Frame Timing Draft Published");
    };
}