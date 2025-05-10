#pragma once
template <typename T>
class Flags
{
private:
public:
    Flags() = default;
    T flagsReplace(T sourceFlags = (T)0, T addedFlags = (T)0, T deletedFlags = (T)0);
    T flagsReplace(T sourceFlags = (T)0, int addedFlags = 0, int deletedFlags = 0);
    T flagsReplace(int sourceFlags = 0, int addedFlags = 0, int deletedFlags = 0);
    bool hasFlag(T sourceFlags, T flag);
    bool hasFlag(T sourceFlags, int flag);
    bool hasFlag(int sourceFlags, int flag);
    T removeFlag(T sourceFlags, int flag);
    T removeFlag(T &sourceFlags, int &flag);
    T addFlag(T sourceFlags, int flag);
    T addFlag(T &sourceFlags, int &flag);
};
template <typename T>
inline T Flags<T>::flagsReplace(T sourceFlags, T addedFlags, T deletedFlags)
{
    return (T)((sourceFlags | addedFlags) & ~deletedFlags);
}
template <typename T>
inline T Flags<T>::flagsReplace(T sourceFlags, int addedFlags, int deletedFlags)
{
    return (T)(((int)sourceFlags | addedFlags) & ~deletedFlags);
}
template <typename T>
inline T Flags<T>::flagsReplace(int sourceFlags, int addedFlags, int deletedFlags)
{
    return (T)((sourceFlags | addedFlags) & ~deletedFlags);
}
template <typename T>
inline bool Flags<T>::hasFlag(T sourceFlags, T flag)
{
    return sourceFlags & flag;
}
template <typename T>
inline bool Flags<T>::hasFlag(T sourceFlags, int flag)
{
    return sourceFlags & flag;
}
template <typename T>
inline bool Flags<T>::hasFlag(int sourceFlags, int flag)
{
    return sourceFlags & flag;
}
template <typename T>
inline T Flags<T>::removeFlag(T sourceFlags, int flag)
{
    return (T)(sourceFlags & ~flag);
}
template <typename T>
inline T Flags<T>::removeFlag(T &sourceFlags, int &flag)
{
    return (T)(sourceFlags & ~flag);
}
template <typename T>
inline T Flags<T>::addFlag(T sourceFlags, int flag)
{
    return (T)(sourceFlags | flag);
}
template <typename T>
inline T Flags<T>::addFlag(T &sourceFlags, int &flag)
{
    return (T)(sourceFlags | flag);
}
