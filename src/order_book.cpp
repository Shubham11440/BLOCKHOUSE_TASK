#include "order_book.hpp"

void OrderBook::process_message(const MboMsg &msg)
{
    switch (msg.action)
    {
    case 'A':
    { // Add
        orders_[msg.order_id] = {msg.price, msg.size, msg.side};
        if (msg.side == 'B')
        {
            bids_[msg.price] += msg.size;
        }
        else
        {
            asks_[msg.price] += msg.size;
        }
        break;
    }
    case 'C':
    { // Cancel
        if (orders_.count(msg.order_id))
        {
            const auto &order_to_cancel = orders_.at(msg.order_id);
            if (order_to_cancel.side == 'B')
            {
                if (bids_.count(order_to_cancel.price))
                {
                    bids_[order_to_cancel.price] -= msg.size;
                    if (bids_[order_to_cancel.price] <= 0)
                    {
                        bids_.erase(order_to_cancel.price);
                    }
                }
            }
            else
            { // Side is 'A'
                if (asks_.count(order_to_cancel.price))
                {
                    asks_[order_to_cancel.price] -= msg.size;
                    if (asks_[order_to_cancel.price] <= 0)
                    {
                        asks_.erase(order_to_cancel.price);
                    }
                }
            }
            // A cancel for a size of the full order means the order is gone
            if (msg.size >= order_to_cancel.size)
            {
                orders_.erase(msg.order_id);
            }
            else
            {
                // This is a partial cancel (like a fill), reduce the tracked order size
                orders_[msg.order_id].size -= msg.size;
            }
        }
        break;
    }
    case 'M':
    { // Modify
        // Remove old state
        if (orders_.count(msg.order_id))
        {
            const auto &old_order = orders_.at(msg.order_id);
            if (old_order.side == 'B')
            {
                if (bids_.count(old_order.price))
                {
                    bids_[old_order.price] -= old_order.size;
                    if (bids_[old_order.price] <= 0)
                    {
                        bids_.erase(old_order.price);
                    }
                }
            }
            else
            {
                if (asks_.count(old_order.price))
                {
                    asks_[old_order.price] -= old_order.size;
                    if (asks_[old_order.price] <= 0)
                    {
                        asks_.erase(old_order.price);
                    }
                }
            }
        }
        // Add new state
        orders_[msg.order_id] = {msg.price, msg.size, msg.side};
        if (msg.side == 'B')
        {
            bids_[msg.price] += msg.size;
        }
        else
        {
            asks_[msg.price] += msg.size;
        }
        break;
    }

    // Per instructions, T and F do not affect the resting order book.
    // They are signals handled by the main loop's printing logic.
    case 'T':
    case 'F':
        break;
    }
}

// get_mbp_levels function is unchanged and remains correct.
void OrderBook::get_mbp_levels(std::vector<MbpLevel> &bids, std::vector<MbpLevel> &asks) const
{
    bids.clear();
    asks.clear();
    bids.reserve(10);
    asks.reserve(10);

    auto it_bid = bids_.begin();
    for (int i = 0; i < 10 && it_bid != bids_.end(); ++i, ++it_bid)
    {
        bids.push_back({it_bid->first, it_bid->second});
    }

    auto it_ask = asks_.begin();
    for (int i = 0; i < 10 && it_ask != asks_.end(); ++i, ++it_ask)
    {
        asks.push_back({it_ask->first, it_ask->second});
    }
}