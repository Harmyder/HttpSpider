#pragma once

namespace SDK
{
    template <class T>
    class OneWriterQueue;

    class AvailableItems
    {
        AvailableItems(const AvailableItems&) = delete;
        AvailableItems& operator=(const AvailableItems&) = delete;

    public:
        AvailableItems();
        ~AvailableItems();

        void Add(int count);
        void AddOne();
        bool RemoveOne();
        int RemoveAll();
        
        void Shutdown();

    private:
        typedef bool QueueType;
        std::unique_ptr<OneWriterQueue<QueueType>> _owq;
    };
}