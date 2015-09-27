#pragma once

#define DECLARE_CLASS_SINGLETON(S)          \
    static S    instance;                   \
                                            \
    public:                                 \
        static S& GetInstance()             \
        {                                   \
            return instance;                \
        }                                   \
    private:                                \
        S();                                \
        S(S const&);                        \
        void operator=(S const&);           \

#define DEFINE_SINGLETON(S)   \
    S::S() {}                 \
    S S::instance