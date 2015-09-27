#pragma once

namespace MVVM
{
    class ViewModelBase
    {
    public:
        virtual ~ViewModelBase() {}
        virtual void InitView() = 0;
    };
}