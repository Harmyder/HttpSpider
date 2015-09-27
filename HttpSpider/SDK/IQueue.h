#pragma once

namespace SDK
{
    template <class T>
    struct IQueue
    {
        virtual int Size() = 0;
        virtual bool PopFront(T& t) = 0;
        virtual void Push(T value) = 0;

        virtual void Shutdown() = 0;
    };
}