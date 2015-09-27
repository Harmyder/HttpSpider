#pragma once

#include "MVVM\Internal\Event.h"
#include "BusinessLogic\LinkConstants.h"

namespace BusinessLogic
{
    struct LinkStateEventArgs final : MVVM::EventArgs
    {
        LinkStateEventArgs(int linkIndex, LinkState state, int errorId, int foundCount) :
            LinkIndex(linkIndex),
            State(state), 
            ErrorId(errorId),
            FoundCount(foundCount)
        {}

        int LinkIndex;
        LinkState State;
        int ErrorId;
        int FoundCount;
    };

    template <class Sender>
    struct LinkStateEvent : MVVM::Event<Sender, LinkStateEventArgs>
    {
        LinkStateEvent(Sender *sender) : Event(sender) {}
    };
}