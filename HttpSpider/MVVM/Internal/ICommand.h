#pragma once

namespace MVVM
{
    struct ICommand
    {
        virtual void Execute() = 0;
    };
}