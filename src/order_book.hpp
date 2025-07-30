#ifndef ORDER_BOOK_HPP
#define ORDER_BOOK_HPP

#include <cstdint>
#include <map>
#include <unordered_map>
#include <functional>
#include <vector>
#include <string> // For storing the timestamp string

// --- Structs

struct Order
{
    int64_t price;
    int32_t size;
    char side;
};

struct MboMsg
{
    std::string ts_event; // FIX: Store the string timestamp from the file
    uint64_t order_id;
    int64_t price;
    int32_t size;
    char action;
    char side;
};

struct MbpLevel
{
    int64_t price = 0;
    int64_t size = 0;

    // FIX: Add a comparison operator so we can check if the book has changed.
    bool operator!=(const MbpLevel &other) const
    {
        return price != other.price || size != other.size;
    }
};


class OrderBook
{
public:
    void process_message(const MboMsg &msg);
    void get_mbp_levels(std::vector<MbpLevel> &bids, std::vector<MbpLevel> &asks) const;

private:
    std::map<int64_t, int64_t, std::greater<int64_t>> bids_;
    std::map<int64_t, int64_t> asks_;
    std::unordered_map<uint64_t, Order> orders_;
};

#endif 