#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <exception>

/// @brief Создает станицу обработки ввода пользователя
/// @tparam I - Тип данных ввода
/// @tparam O - Тип данных вывода
template <typename I, typename O>
class UserInput
{
private:
    std::string Description;
    std::string MainMessage;
    std::string FailMessage;
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

    /// @brief Отображает страницу обработки множественного ввода пользователя. Использует std::cin
    /// @return
    O IOcin();

    /// @brief Отображает страницу обработки сквозного ввода пользователя output = input. Использует std::cin
    /// @return
    I getThroughIO();

    /// @brief Отображает страницу обработки множественного ввода пользователя. Использует std::getline
    /// @return
    O getMultipleIO();
    /// @brief Отображает страницу обработки сквозного ввода пользователя output = input. Использует std::getline
    /// @param denyEmpty true - запретить пустой ввод, по умолчанию false
    /// @return
    std::string getStringIO(bool denyEmpty = false);

    void setDescription(const std::string &newText);
    void setMainMessage(const std::string &newText);
    void setFailMessage(const std::string &newText);
};

template <typename I, typename O>
inline UserInput<I, O>::UserInput(const std::string &description,
                                  const std::string &mainMessage,
                                  const std::string &failMessage,
                                  int ioCapacity) : Description(description),
                                                    MainMessage(mainMessage),
                                                    FailMessage(failMessage),
                                                    ioCount{0},
                                                    ioLength(ioCapacity),
                                                    inputs(std::make_unique<I[]>(ioLength)),
                                                    outputs(std::make_unique<O[]>(ioLength)),
                                                    throughIO(false) {}

template <typename I, typename O>
inline UserInput<I, O>::UserInput(const std::string &description,
                                  const std::string &mainMessage,
                                  const std::string &failMessage) : Description(description),
                                                                    MainMessage(mainMessage),
                                                                    FailMessage(failMessage),
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
            std::cout << Description << std::endl;
        }
        std::cout << MainMessage;
        std::cin >> userInput;
        std::cin.clear();
        while (std::cin.get() != '\n')
        {
        }

        for (int i{0}; i < ioLength; i++)
        {
            if (userInput == inputs[i])
            {
                return outputs[i];
            }
        }
        std::cout << FailMessage << std::endl;
    } while (1);
}

template <typename I, typename O>
inline I UserInput<I, O>::getThroughIO()
{
    // Сквозной ввод
    I throughInput;
    std::cout << MainMessage;
    std::cin >> throughInput;
    std::cin.clear();
    while (std::cin.get() != '\n')
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
    // Множественный ввод
    std::string userInput;
    do
    {
        if (!Description.empty())
        {
            std::cout << Description << std::endl;
        }
        std::cout << MainMessage;
        std::getline(std::cin, userInput);

        for (int i{0}; i < ioLength; i++)
        {
            if (userInput == inputs[i])
            {
                return outputs[i];
            }
        }
        std::cout << FailMessage << std::endl;
    } while (1);
}

template <typename I, typename O>
inline std::string UserInput<I, O>::getStringIO(bool denyEmpty)
{
    // Сквозной ввод
    std::string throughInput;
    do
    {
        std::cout << MainMessage;
        std::getline(std::cin, throughInput);
        if (denyEmpty && throughInput.empty())
        {
            std::cout << FailMessage << std::endl;
        }
        else
        {
            denyEmpty = false;
        }
    } while (denyEmpty);
    return throughInput;
}

template <typename I, typename O>
inline void UserInput<I, O>::setDescription(const std::string &newText)
{
    Description = newText;
}

template <typename I, typename O>
inline void UserInput<I, O>::setMainMessage(const std::string &newText)
{
    MainMessage = newText;
}

template <typename I, typename O>
inline void UserInput<I, O>::setFailMessage(const std::string &newText)
{
    FailMessage = newText;
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