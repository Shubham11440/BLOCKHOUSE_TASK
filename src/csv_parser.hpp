#ifndef CSV_PARSER_HPP
#define CSV_PARSER_HPP

#include <string>
#include <vector>
#include <sstream>
#include "order_book.hpp"

inline std::vector<std::string> split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to parse a single line of the MBO CSV.
inline bool parse_mbo_line(const std::string &line, MboMsg &msg)
{
    auto tokens = split(line, ',');
    if (tokens.size() < 12)
        return false;

    try
    {
        // ts_event is now the third column (index 2) in the raw data
        msg.ts_event = tokens[2];

        msg.action = tokens[5][0];
        msg.side = tokens[6][0];

        std::string price_str = tokens[7];
        size_t dot_pos = price_str.find('.');
        if (dot_pos != std::string::npos)
        {
            price_str.erase(dot_pos, 1);
            // Pad with zeros if necessary, e.g., "123.4" -> "12340"
            if (price_str.length() - dot_pos < 2)
            {
                price_str.append(2 - (price_str.length() - dot_pos), '0');
            }
        }
        else
        {
            // If there's no decimal, it's a whole number, so multiply by 100
            price_str.append("00");
        }
        msg.price = std::stoll(price_str);

        msg.size = std::stoi(tokens[8]);
        msg.order_id = std::stoull(tokens[10]);
    }
    catch (const std::exception &e)
    {
        // It's helpful to know which line failed
        // std::cerr << "Failed to parse line: " << line << std::endl;
        return false;
    }
    return true;
}

#endif 
