#include "Misc.h"

void Misc::printMessage(const std::string &msg, bool endl)
{
#if defined(_WIN64) || defined(_WIN32)
    std::wstring wmesg = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.from_bytes(msg.data());
    std::wcout << wmesg;
    if (endl)
        std::wcout << std::endl;
#else
    std::cout << msg;
    if (endl)
        std::cout << std::endl;
#endif
}

std::wstring Misc::string_to_wstring(const std::string &str)
{
    return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.from_bytes(str.data());
}

std::string Misc::wstring_to_string(const std::wstring &wstr)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv1;
    std::string str = conv1.to_bytes(wstr);
    return str;
}

uint Misc::getSymbolsCount(const std::string string)
{
    uint positives = 0;
    uint negatives = 0;
    for (uint i{0}; i < string.size(); i++)
    {
        if (string.data()[i] >= 0)
            positives++;
        else
            negatives++;
    }
    return positives + (negatives / 2);
}

std::string Misc::StampToTime(long long timestamp)
{
    time_t tick = (time_t)(timestamp);
    struct tm tm;
    tm = *localtime(&tick);
    std::string day = (tm.tm_mday < 10) ? "0" + std::to_string(tm.tm_mday) : std::to_string(tm.tm_mday);
    std::string month = (tm.tm_mon + 1 < 10) ? "0" + std::to_string(tm.tm_mon + 1) : std::to_string(tm.tm_mon + 1);
    std::string year = std::to_string(tm.tm_year + 1900);
    std::string hour = (tm.tm_hour < 10) ? "0" + std::to_string(tm.tm_hour) : std::to_string(tm.tm_hour);
    std::string min = (tm.tm_min < 10) ? "0" + std::to_string(tm.tm_min) : std::to_string(tm.tm_min);
    std::string sec = (tm.tm_sec < 10) ? "0" + std::to_string(tm.tm_sec) : std::to_string(tm.tm_sec);
    return std::string(day + "." + month + "." + year + " " + hour + ":" + min + ":" + sec);
}

uint Misc::getInt(char buffer[], uint offset)
{
    uint out;
    memcpy(&out, &buffer[offset], 4);
    return out;
}

unsigned long long Misc::getLong(char buffer[], uint offset)
{
    unsigned long long out;
    memcpy(&out, &buffer[offset], 8);
    return out;
}

std::string Misc::getString(char buffer[], uint offset)
{
    const uint len = getInt(buffer, offset);
    char buf[len];
    offset += 4;
    for (int i{0}; i < len; i++)
        buf[i] = buffer[offset++];
    std::string s(buf, len);
    return s;
}

std::string Misc::getString(const char buffer[], uint len, uint offset)
{
    char buf[len];
    for (int i{0}; i < len; i++)
        buf[i] = buffer[offset++];
    std::string s(buf, len);
    return s;
}

void Misc::writeIntBuffer(uint num, char buffer[], uint offset)
{
    mempcpy(&buffer[offset], &num, 4);
}

void Misc::writeUlongBuffer(unsigned long long num, char buffer[], uint offset)
{
    mempcpy(&buffer[offset], &num, 8);
}

void Misc::writeStringBuffer(std::string &str, char buffer[], uint offset, bool add_size, uint max_size)
{
    str = ltrimString(str, max_size);
    const uint size = str.size();
    auto d = str.data();
    if (add_size)
    {
        mempcpy(&buffer[offset], &size, 4);
        mempcpy(&buffer[offset + 4], &d[0], size);
    }
    else
    {
        mempcpy(&buffer[offset], &d[0], size);
    }
}

void Misc::writeStringBuffer(std::string &&str, char buffer[], uint offset, bool add_size, uint max_size)
{
    writeStringBuffer(str, buffer, offset, add_size, max_size);
}

uint Misc::findDynamicData(char buffer[], uint offset, uint offset_data, uint max_size)
{
    uint size;
    if (offset_data == 0)
    {
        size = getInt(buffer, offset);
        if (size > max_size)
            return 0;
    }
    else
    {
        for (uint i{0}; i < offset_data; i++)
        {
            size = getInt(buffer, offset);
            if (size > max_size)
                return 0;
            offset += size + 4;
        }
    }

    return offset;
}

std::vector<char> Misc::writeVectorBuffer(char buffer[], uint len)
{
    std::vector<char> v;
    for (int i{0}; i < len; i++)
        v.push_back(buffer[i]);
    return v;
}

void Misc::writeVectorBuffer(char buffer[], const std::vector<char> &vector)
{
    const uint len = vector.size();
    for (int i{0}; i < len; i++)
        buffer[i] = vector[i];
}

std::vector<char> Misc::writeVectorBuffer(const std::string &str)
{
    std::vector<char> vector;
    const uint len = str.size();
    auto d = str.data();
    for (int i{0}; i < len; i++)
        vector.push_back(d[i]);

    return vector;
}

ullong Misc::getRandomKey()
{
    // cppcheck ругается, на эту функцию. Но она дает очень хороший рандом именно из за ее стиля.
    char buf[8];
    ullong key = std::abs(std::rand());
    std::srand(time(NULL) - key);
    for (int i{0}; i < 8; i++)
    {
        buf[i] = std::abs(std::rand());
    }
    memcpy(&key, buf, 8);
    return key;
}

std::vector<std::string> Misc::stringExplode(std::string const &str, const std::string &delimeter)
{
    std::vector<std::string> out;
    size_t start;
    size_t end = 0;

    while ((start = str.find_first_not_of(delimeter, end)) != std::string::npos)
    {
        end = str.find(delimeter, start);
        out.push_back(str.substr(start, end - start));
    }
    return out;
}

std::vector<std::string> Misc::stringExplode(std::string const &str, char delimeter)
{
    std::vector<std::string> out;
    size_t start;
    size_t end = 0;

    while ((start = str.find_first_not_of(delimeter, end)) != std::string::npos)
    {
        end = str.find(delimeter, start);
        out.push_back(str.substr(start, end - start));
    }
    return out;
}

std::string Misc::ltrimString(std::string &string, uint max)
{
    if (string.size() > max)
    {
        string.erase(0, string.size() - max);
    }
    return string;
}

std::string Misc::getRandomStr(const unsigned char length)
{
    char buf[length];
    ullong key = std::abs(std::rand());
    std::srand(time(NULL) - key);
    for (uint i{0}; i < length; i++)
    {
        uint rnd = std::rand();

        // если меньше 30 - цифра
        if (rnd % 90 <= 30)
        {
            buf[i] = 48 + (rnd % 10);
            continue;
        }

        // если 30 - 60  - нижний регистр
        if (rnd % 90 <= 60)
        {
            buf[i] = 65 + (rnd % 26);
            continue;
        }

        // если 60+  - нижний регистр
        buf[i] = 97 + (rnd % 26);
    }
    return std::string(buf, length);
}

std::string Misc::getConfigValue(const std::string &path, const std::string &section, const std::string &value)
{
    std::ifstream stream(path);

    std::string line;
    const int section_len = section.length();
    const int value_len = value.length();

    char section_buff[section_len];
    char value_buff[value_len];

    while (1)
    {
        if (std::getline(stream, line, '['))
        {
            stream.read(section_buff, section_len);
            if (memcmp(section_buff, section.data(), section_len) == 0)
            {
                if (!std::getline(stream, line, ']'))
                    break;

                std::getline(stream, line, '[');

                // очистка пробелов и табуляторов перед параметрами
                while (1)
                {
                    auto value_bpos = line.find("\n ");
                    auto value_tpos = line.find("\n\t");
                    if (value_bpos == std::string::npos && value_tpos == std::string::npos)
                        break;

                    if (value_bpos != std::string::npos)
                        line.erase(value_bpos + 1, 1);
                    if (value_tpos != std::string::npos)
                        line.erase(value_tpos + 1, 1);
                }
                auto value_pos = line.find("\n" + value) + 1;
                if (value_pos == std::string::npos)
                    break;
                line.erase(0, value_len + value_pos);

                for (int i{0}; i < line.length(); i++)
                {
                    if (line[i] == '=' || line[i] == ' ')
                    {
                        line.erase(0, 1);
                    }
                    else
                    {
                        break;
                    }
                }

                char deleted[5]{';', '#', '\n', '\r', '\t'};
                for (int i{0}; i < 5; i++)
                {
                    value_pos = line.find(deleted[i]);
                    if (value_pos != std::string::npos)
                        line.erase(value_pos, line.length() - value_pos);
                }
                stream.close();
                return line;
            }
            else
            {
                continue;
            }
        }
        else
        {
            break;
        }
    }
    stream.close();
    return std::string();
}

void Misc::alignPaginator(ullong &start, ullong &per_page, const ullong &count)
{
    if (per_page == 0)
        per_page = 1;
    if (start > count)
        start = count;
    if (start == 0)
        start = 1;
}
