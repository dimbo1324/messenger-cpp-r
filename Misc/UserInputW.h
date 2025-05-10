#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <exception>
#include <codecvt>
#include <cstdint>
#include <locale>
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
    bool throughIO = false;
    bool matchedIO = false;
    void addInputs() {};
    void addOutputs() {};

public:
    UserInput(const std::string &description, const std::string &mainMessage, const std::string &failMessage, int ioCapacity);
    UserInput(const std::string &description, const std::string &mainMessage, const std::string &failMessage);
    UserInput();
    ~UserInput() = default;
    template <typename IO, typename... Args>
    void addInputs(IO value, Args... args);
    template <typename IO, typename... Args>
    void addOutputs(IO value, Args... args);
    void addIO() {};
    template <typename IO, typename... Args>
    void addIO(IO value, Args... args);
    O IOcin();
    I getThroughIO();
    O getMultipleIO();
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
    I throughInput;
    std::wcout << MainMessage;
    std::wcin >> throughInput;
    std::wcin.clear();
    while (std::wcin.get() != '\n')
    {
    }
    return throughInput;
}
template <typename I, typename O>
inline O UserInput<I, O>::getMultipleIO()
{
    if (throughIO)
    {
        throw "Multiple input is disabled. Use IOgetlineThrough() method.";
    }
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