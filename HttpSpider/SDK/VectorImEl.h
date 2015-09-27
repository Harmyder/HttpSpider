#pragma once

namespace SDK
{
    template <class T>
    class VectorImEl
    {
    public:
        const T& operator[](int i) const
        {
            return _data[i];
        }

        int Push(T t) 
        { 
            std::lock_guard<mutex> lk(_mutex);

            _data.push_back(std::move(t)); 
            return _data.size(); 
        }

    private:
        std::vector<T> _data;
        mutable std::mutex _mutex;
    };
}