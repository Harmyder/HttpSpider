#include "stdafx.h"

#include "SDK\AvailableItems.h"

#include "SDK\OneWriterQueue.h"

using namespace std;

namespace SDK
{
    AvailableItems::AvailableItems() : _owq(new OneWriterQueue<QueueType>)
    {}

    AvailableItems::~AvailableItems() {}

    void AvailableItems::Add(int count)
    {
        for (int i = 0; i < count; i++)
            AddOne();
    }

    void AvailableItems::AddOne()
    {
        _owq->Push("_");
    }

    bool AvailableItems::RemoveOne()
    {
        QueueType dummy;
        if (_owq->PopFront(dummy) == false)
            return false;
        return true;
    }

    int AvailableItems::RemoveAll()
    {
        int removed = _owq->Size();
        _owq = make_unique<OneWriterQueue<QueueType>>();
        return removed;
    }


    void AvailableItems::Shutdown()
    {
        _owq->Shutdown();
    }
}