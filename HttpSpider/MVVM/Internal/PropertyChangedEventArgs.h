#pragma once

#include "MVVM\Internal\Event.h"

namespace MVVM
{
    struct PropertyChangedEventArgs final : EventArgs
    {
        explicit PropertyChangedEventArgs(std::string propertyName) : PropertyName(move(propertyName)) {}

        std::string PropertyName;
    };

    template <class Sender>
    struct PropertyChangedEvent : MVVM::Event<Sender, PropertyChangedEventArgs>
    {
        PropertyChangedEvent(Sender *sender) : Event(sender) {}
    };
}
