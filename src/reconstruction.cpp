#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include "csv_parser.hpp"

// --- Helper function to compare MBP levels ---
// This makes the main loop cleaner.
bool has_book_changed(const std::vector<MbpLevel> &bids_a, const std::vector<MbpLevel> &asks_a,
                      const std::vector<MbpLevel> &bids_b, const std::vector<MbpLevel> &asks_b)
{
    // If the number of levels is different, the book has changed.
    if (bids_a.size() != bids_b.size() || asks_a.size() != asks_b.size())
    {
        return true;
    }
    // Check bids
    for (size_t i = 0; i < bids_a.size(); ++i)
    {
        if (bids_a[i] != bids_b[i])
        {
            return true;
        }
    }
    // Check asks
    for (size_t i = 0; i < asks_a.size(); ++i)
    {
        if (asks_a[i] != asks_b[i])
        {
            return true;
        }
    }
    return false;
}

// --- Printing Logic (Warning Fixed) ---
void print_mbp_row(const std::string &ts_event, const std::vector<MbpLevel> &bids, const std::vector<MbpLevel> &asks)
{
    std::cout << std::fixed << std::setprecision(2);
    std::cout << ts_event << ",";
    // FIX: Changed loop variable to size_t to match .size() type and remove warnings.
    for (size_t i = 0; i < 10; ++i)
    {
        if (i < bids.size())
        {
            std::cout << static_cast<double>(bids[i].price) / 100.0 << "," << bids[i].size << ",";
        }
        else
        {
            std::cout << "0.00,0,";
        }
        if (i < asks.size())
        {
            std::cout << static_cast<double>(asks[i].price) / 100.0 << "," << asks[i].size;
        }
        else
        {
            std::cout << "0.00,0";
        }
        if (i < 9)
            std::cout << ",";
    }
    std::cout << std::endl;
}

// --- Main Program Logic (Corrected) ---
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Error: Please provide the MBO data file as an argument." << std::endl;
        return 1;
    }

    // Redirect stdout to the output file
    std::streambuf *coutbuf = std::cout.rdbuf();
    std::ofstream out("output.csv");
    std::cout.rdbuf(out.rdbuf());

    std::ifstream mbo_file(argv[1]);
    if (!mbo_file.is_open())
    {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    OrderBook order_book;
    std::string line;
    MboMsg msg;

    // Print the CSV header
    std::cout << "ts_event,bid_px_00,bid_sz_00,ask_px_00,ask_sz_00,bid_px_01,bid_sz_01,ask_px_01,ask_sz_01,bid_px_02,bid_sz_02,ask_px_02,ask_sz_02,bid_px_03,bid_sz_03,ask_px_03,ask_sz_03,bid_px_04,bid_sz_04,ask_px_04,ask_sz_04,bid_px_05,bid_sz_05,ask_px_05,ask_sz_05,bid_px_06,bid_sz_06,ask_px_06,ask_sz_06,bid_px_07,bid_sz_07,ask_px_07,ask_sz_07,bid_px_08,bid_sz_08,ask_px_08,ask_sz_08,bid_px_09,bid_sz_09,ask_px_09,ask_sz_09" << std::endl;

    // FIX: Only skip the header row, not the first data row.
    std::getline(mbo_file, line);

    // State tracking variables
    std::vector<MbpLevel> last_bids;
    std::vector<MbpLevel> last_asks;

    // The first event should always be printed.
    bool is_first_event = true;

    while (std::getline(mbo_file, line))
    {
        if (parse_mbo_line(line, msg))
        {
            // 1. Process the message to update the internal order book state.
            //    This only handles A, M, C actions as per your order_book.cpp
            order_book.process_message(msg);

            // 2. Get the current top 10 levels of the book.
            std::vector<MbpLevel> current_bids;
            std::vector<MbpLevel> current_asks;
            order_book.get_mbp_levels(current_bids, current_asks);

            // 3. Check if the book has changed since the last printed row.
            //    The first event should always be printed.
            if (is_first_event || has_book_changed(current_bids, current_asks, last_bids, last_asks))
            {
                // 4. If it has changed, print the new state and update the last known state.
                print_mbp_row(msg.ts_event, current_bids, current_asks);
                last_bids = current_bids;
                last_asks = current_asks;
                is_first_event = false;
            }
        }
    }

    // Restore stdout and print completion message to console
    std::cout.rdbuf(coutbuf);
    std::cout << "Processing complete. Output written to output.csv" << std::endl;
    return 0;
}
