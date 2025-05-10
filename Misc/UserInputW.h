#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <exception>

#include <codecvt>
#include <cstdint>
#include <locale>

/// @brief Создает станицу обработки ввода пользователя
/// @tparam I - Тип данных ввода
/// @tparam O - Тип данных вывода
template <typename I, typename O>
class UserInput
{
private:
    std::wstring Description;
    std::wstring MainMessage;
    std::wstring FailMessage;
    int ioCount = 0;
    int ioLength = 0;
    std::unique_ptr<I[]> inputs = nullptr;
    std::unique_ptr<O[]> outputs = nullptr;
    bool throughIO = false; // output = input
    bool matchedIO = false; // inputs[n] -> outputs[n]
    void addInputs(){};
    void addOutputs(){};

public:
    /// @brief Создает полную страницу множественного пользовательского ввода
    /// @param description - Заголовок страницы
    /// @param mainMessage - Сообщение пользователю
    /// @param failMessage - Сообщение при неверном вводе
    /// @param ioCapacity - Количество возможных вариантов ввода
    UserInput(const std::string &description, const std::string &mainMessage, const std::string &failMessage, int ioCapacity);

    /// @brief Создает страницу сквозного пользовательского ввода. output = input.
    /// @param description - Заголовок страницы
    /// @param mainMessage - Сообщение пользователю
    /// @param failMessage - Сообщение при неверном вводе
    UserInput(const std::string &description, const std::string &mainMessage, const std::string &failMessage);

    /// @brief Создает страницу сквозного пользовательского ввода (output = input). Без указания сообщений запроса.
    /// Запросы можно задать во время выполнения.
    UserInput();
    ~UserInput() = default;

    /// @brief Задает возможные варианты множественного ввода. Количество аргументов должно быть равным ioCapacity конструктора
    /// @tparam IO
    /// @tparam ...Args
    /// @param value
    /// @param ...args
    /// @exception out_of_range
    template <typename IO, typename... Args>
    void addInputs(IO value, Args... args);

    /// @brief Задает возможные варианты множественного вывода в соответствии addInputs.
    /// @tparam IO
    /// @tparam ...Args
    /// @param value
    /// @param ...args
    /// @exception out_of_range
    template <typename IO, typename... Args>
    void addOutputs(IO value, Args... args);

    /// @brief Задает возможные варианты множественного ввода и вывода. output = input
    void addIO(){};
    template <typename IO, typename... Args>
    void addIO(IO value, Args... args);

    /// @brief (Deprecated) Отображает страницу обработки множественного ввода пользователя. Использует std::wcin
    /// @return
    O IOcin();

    /// @brief Отображает страницу обработки сквозного ввода пользователя output = input. Использует wcin.
    /// Подходит для запроса чисел.
    /// @return
    I getThroughIO();

    /// @brief Отображает страницу обработки множественного ввода пользователя.
    /// @return
    O getMultipleIO();
    /// @brief Отображает страницу обработки сквозного ввода пользователя output = input. Использует std::getline
    /// @param denyEmpty true - запретить пустой ввод, по умолчанию false
    /// @return
    std::string getStringIO(bool denyEmpty = false, bool no_pass = true);

    void setDescription(const std::wstring &newText);
    void setMainMessage(const std::wstring &newText);
    void setFailMessage(const std::wstring &newText);

    std::wstring to_wstring(const std::string &str);
    std::string to_string(const std::wstring &wstr);
};

template <typename I, typename O>
inline UserInput<I, O>::UserInput(const std::string &description,
                                  const std::string &mainMessage,
                                  const std::string &failMessage,
                                  int ioCapacity)
    : Description(to_wstring(description)),
      MainMessage(to_wstring(mainMessage)),
      FailMessage(to_wstring(failMessage)),
      ioCount{0},
      ioLength(ioCapacity),
      inputs(std::make_unique<I[]>(ioLength)),
      outputs(std::make_unique<O[]>(ioLength)),
      throughIO(false) {}

template <typename I, typename O>
inline UserInput<I, O>::UserInput(const std::string &description,
                                  const std::string &mainMessage,
                                  const std::string &failMessage)
    : Description(to_wstring(description)),
      MainMessage(to_wstring(mainMessage)),
      FailMessage(to_wstring(failMessage)),
      ioCount{0},
      ioLength(0),
      inputs(std::make_unique<I[]>(1)),
      outputs(std::make_unique<O[]>(1)),
      throughIO(true) {}

template <typename I, typename O>
inline UserInput<I, O>::UserInput() : inputs(std::make_unique<I[]>(1)), outputs(std::make_unique<O[]>(1)), throughIO(true) {}

template <typename I, typename O>
inline O UserInput<I, O>::IOcin()
{
    if (throughIO)
    {
        throw "Multiple input is disabled. Use IOcinThrough() method.";
    }
    // Множественный ввод
    I userInput;
    do
    {
        if (!Description.empty())
        {
            std::wcout << Description << std::endl;
        }
        std::wcout << MainMessage;
        std::wcin >> userInput;
        std::wcin.clear();
        while (std::wcin.get() != '\n')
        {
        }

        for (int i{0}; i < ioLength; i++)
        {
            if (userInput == inputs[i])
            {
                return outputs[i];
            }
        }
        std::wcout << FailMessage << std::endl;
    } while (1);
}

template <typename I, typename O>
inline I UserInput<I, O>::getThroughIO()
{
    // Сквозной ввод
    I throughInput;
    std::wcout << MainMessage;
    std::wcin >> throughInput;
    std::wcin.clear();
    while (std::wcin.get() != '\n')
    {
    }
    return throughInput;
}

/*На Windows getline работает не очень корректно с кириллицей. Для обратной совместимости используется std::wcin*/

template <typename I, typename O>
inline O UserInput<I, O>::getMultipleIO()
{
    if (throughIO)
    {
        throw "Multiple input is disabled. Use IOgetlineThrough() method.";
    }
    // Множественный ввод
    std::wstring userInput;
    do
    {
        if (!Description.empty())
        {
            std::wcout << Description << std::endl;
        }
        std::wcout << MainMessage;
        std::wcin >> userInput;
        std::wcin.clear();
        wchar_t t = L'\0';
        while (t != L'\n')
        {
            t = std::wcin.get();
            if (t != L'\n')
            {
                userInput.push_back(t);
            }
        }

        for (int i{0}; i < ioLength; i++)
        {
            if (to_string(userInput) == inputs[i])
            {
                return outputs[i];
            }
        }
        std::wcout << FailMessage << std::endl;
    } while (1);
}

template <typename I, typename O>
inline std::string UserInput<I, O>::getStringIO(bool denyEmpty, bool no_pass)
{
    // Сквозной ввод
    std::wstring throughInput;
    do
    {
        if (!Description.empty())
        {
            std::wcout << Description << std::endl;
        }
        std::wcout << MainMessage;
        std::wcin >> throughInput;
        std::wcin.clear();
        wchar_t t = L'\0';
        while (t != L'\n')
        {
            t = std::wcin.get();
            if (t != L'\n')
            {
                throughInput.push_back(t);
            }
        }
        if (denyEmpty && throughInput.empty())
        {
            std::wcout << FailMessage << std::endl;
        }
        else
        {
            denyEmpty = false;
        }
    } while (denyEmpty);
    return to_string(throughInput);
}

template <typename I, typename O>
inline void UserInput<I, O>::setDescription(const std::wstring &newText)
{
    Description = newText;
}

template <typename I, typename O>
inline void UserInput<I, O>::setMainMessage(const std::wstring &newText)
{
    MainMessage = newText;
}

template <typename I, typename O>
inline void UserInput<I, O>::setFailMessage(const std::wstring &newText)
{
    FailMessage = newText;
}

template <typename I, typename O>
inline std::wstring UserInput<I, O>::to_wstring(std::string const &str)
{
    std::wstring wstr = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.from_bytes(str.data());
    return wstr;
}

template <typename I, typename O>
inline std::string UserInput<I, O>::to_string(const std::wstring &wstr)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv1;
    std::string str = conv1.to_bytes(wstr);
    return str;
}

template <typename I, typename O>
template <typename IO, typename... Args>
inline void UserInput<I, O>::addInputs(IO value, Args... args)
{
    if (ioCount >= ioLength)
        throw std::out_of_range("Out of range array: inputs");
    inputs[ioCount++] = value;
    addInputs(args...);
    ioCount = 0;
}

template <typename I, typename O>
template <typename IO, typename... Args>
inline void UserInput<I, O>::addOutputs(IO value, Args... args)
{
    if (ioCount >= ioLength)
        throw std::out_of_range("Out of range array: outputs");
    outputs[ioCount++] = value;
    addOutputs(args...);
    ioCount = 0;
}

template <typename I, typename O>
template <typename IO, typename... Args>
inline void UserInput<I, O>::addIO(IO value, Args... args)
{
    if (ioCount >= ioLength)
        throw std::out_of_range("Out of range arrays: inputs or outputs");
    inputs[ioCount] = value;
    outputs[ioCount++] = value;
    addIO(args...);
    ioCount = 0;
}