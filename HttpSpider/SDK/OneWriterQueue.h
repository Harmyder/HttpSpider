#pragma once

#include "SDK\IQueue.h"

namespace SDK
{
    template <class T>
    class OneWriterQueue : public IQueue<T>
    {
        OneWriterQueue(const OneWriterQueue&) = delete;
        OneWriterQueue& operator=(const OneWriterQueue&) = delete;

    public:
        class OneWriterQueueError : public std::logic_error {
        public:
            explicit OneWriterQueueError(const std::string& message) : logic_error(message) {}
        };

        OneWriterQueue();
        ~OneWriterQueue();

        int Size() override;
        bool PopFront(T& value) override;
        void Push(T value) override;

        void Shutdown() override;

    private:
        std::queue<T> _queue;
        std::mutex _mutex;
        std::condition_variable _condition;
        bool _shutdown;
    };

    // **********************************************************************************
    template <class T>
    OneWriterQueue<T>::OneWriterQueue() : _shutdown(false)
    {
    }

    template <class T>
    OneWriterQueue<T>::~OneWriterQueue()
    {
    }

    template <class T>
    int OneWriterQueue<T>::Size()
    {
        return _queue.size();
    }

    template <class T>
    bool OneWriterQueue<T>::PopFront(T& t)
    {
        unique_lock<std::mutex> lk(_mutex);

        _condition.wait(lk, [&]()
        {
            return (_shutdown || !(_queue.empty()));
        });

        if (!_shutdown)
        {
            t = move(_queue.front());
            _queue.pop();
        }

        return !_shutdown;
    }

    template <class T>
    void OneWriterQueue<T>::Push(T value)
    {
        {
            lock_guard<std::mutex> lk(_mutex);
            _queue.push(value);
        }
        _condition.notify_one();
    }

    template <class T>
    void OneWriterQueue<T>::Shutdown()
    {
        if (_shutdown)
            throw OneWriterQueueError("Shutdown called twice.");

        _shutdown = true;
        _condition.notify_all();
    }
}