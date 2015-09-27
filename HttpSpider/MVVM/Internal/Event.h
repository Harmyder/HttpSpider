#pragma once

namespace MVVM
{
    struct EventArgs 
    {
        static EventArgs Empty;
    };

    template <class Sender, class Args, class dummy = void>
    class Event;

    template <class Sender, class Args>
    class Event < Sender, Args, typename std::enable_if<std::is_convertible<Args*, EventArgs*>::value>::type >
    {
    public:
        Event(Sender *sender) : _sender(sender) {}

        typedef std::function<void(Sender*, const Args&)> Function;

        struct Comparator
        {
            bool operator()(const Function& f1, const Function& f2) const
            {
                auto ptr1 = f1.target<Function>();
                auto ptr2 = f2.target<Function>();
                return ptr1 < ptr2;
            }
        };

        void operator+=(Function f) { _ASSERT(_observers.find(f) == _observers.end()); _observers.insert(f); }
        void operator-=(Function f) { _ASSERT(_observers.find(f) != _observers.end()); _observers.erase(f); }
        void operator()(const Args& args)
        {
            for (auto observer : _observers)
            {
                observer(_sender, args);
            }
        }

    private:
        Sender *_sender;
        std::set<Function, Comparator> _observers;
    };

#define DEFINE_EVENT(name)                                     \
    template <class Sender>                                    \
    struct name : MVVM::Event<Sender, name##Args>              \
    {                                                          \
        ConnectTaskEvent(Sender *sender) : Event(sender) {}    \
    }

}
