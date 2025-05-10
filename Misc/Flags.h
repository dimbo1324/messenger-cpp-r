#pragma once

/// @brief Класс для операций с флагами
/// @tparam T Только enum флаги
template <typename T>
class Flags
{
private:
public:
    Flags() = default;

    /// @brief Добавляет и(или) удаляет флаги
    /// @param sourceFlags T
    /// @param addedFlags T
    /// @param deletedFlags T
    /// @return
    T flagsReplace(T sourceFlags = (T)0, T addedFlags = (T)0, T deletedFlags = (T)0);

    /// @brief Добавляет и(или) удаляет флаги.
    /// @param sourceFlags T
    /// @param addedFlags int
    /// @param deletedFlags int
    /// @return
    T flagsReplace(T sourceFlags = (T)0, int addedFlags = 0, int deletedFlags = 0);

    /// @brief Добавляет и(или) удаляет флаги.
    /// @param sourceFlags int
    /// @param addedFlags int
    /// @param deletedFlags int
    /// @return
    T flagsReplace(int sourceFlags = 0, int addedFlags = 0, int deletedFlags = 0);

    /// @brief Проверка включенных флагов
    /// @param sourceFlags T
    /// @param flag T
    /// @return
    bool hasFlag(T sourceFlags, T flag);

    /// @brief Проверка включенных флагов
    /// @param sourceFlags T
    /// @param flag int
    /// @return
    bool hasFlag(T sourceFlags, int flag);

    /// @brief Проверка включенных флагов
    /// @param sourceFlags int
    /// @param flag int
    /// @return
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
