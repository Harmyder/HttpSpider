#pragma once

#include "MVVM\Internal\ICommand.h"

namespace MVVM
{
    template <class Action>
    class RelayCommand : public ICommand
    {
        RelayCommand(const RelayCommand&) = delete;
        RelayCommand& operator= (const RelayCommand&) = delete;

    public:
        void Execute() override { _action(); }

    private:
        RelayCommand(Action action) : _action(action) {}
        template <class Action>
        friend RelayCommand<Action>* CreateRelayCommand(Action&& action);

    private:
        const Action _action;
    };

    template <class Action>
    RelayCommand<Action>* CreateRelayCommand(Action&& action) { return new RelayCommand<Action>(std::forward<Action>(action)); }
}