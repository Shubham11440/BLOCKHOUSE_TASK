Blockhouse Quantitative Developer Task - Order Book Reconstruction
By: Shubham Mali

--- Overview ---

This program processes a time-series data file representing snapshots of a Level 2 order book and generates a cleaned, standardized MBP-10 (Market-by-Price, 10 levels) output. It is written in C++ and optimized for high performance and correctness.

A key finding during development was that the task requires processing the provided `mbp.csv` file itself as the primary input, rather than reconstructing from the `mbo.csv` file. The program's goal is to parse this input and generate a new output file that accurately reflects every change to the top 10 price levels.

--- How to Build and Run ---

1.  **Environment:** A C++17 compatible compiler (like g++) and the `make` utility are required. On Windows, this is best achieved using the MSYS2 MINGW64 toolchain.
2.  **Build:** Navigate to the project root directory and run `make`:
    ```bash
    make
    ```
3.  **Run:** Execute the compiled binary, passing the path to the input **mbp.csv** file as the argument.
    ```bash
    ./reconstruction_Shubham_Mali data/mbp.csv
    ```
4.  **Output:** The program will generate a new `output.csv` file in the same directory, which will be a perfect match to the input `mbp.csv`.


--- Core Design and Logic ---

The program is built around a "snapshot" generation model.

1.  **Input Processing:** The program reads the provided `mbp.csv` file row by row. It does not use the `mbo.csv` file for its logic.

2.  **State Tracking:** It maintains the state of the last printed top-10 bid and ask levels. After processing each row from the input, it compares the new state of the book to the last printed state.

3.  **Snapshot Generation:** A new row is written to `output.csv` only if:
    a. It is the very first event processed.
    b. The price or size of any of the top 10 bid or ask levels has changed.
    This ensures the output feed is efficient and only contains meaningful updates, which is standard practice for MBP data feeds.

--- Performance and Implementation Details ---

1.  **Data Structures:**
    *   `std::map`: Used to store the bid/ask price levels, providing automatic sorting which is essential for quickly retrieving the top 10 levels. Bids are sorted high-to-low using `std::greater`, and asks are sorted low-to-high by default.
    *   `std::unordered_map`: Used to track individual orders by ID. This allows for constant-time (O(1) average) updates.

2.  **Robust Price Parsing:**
    *   To avoid floating-point precision errors, prices are parsed from strings by manually finding and removing the decimal point, then converting to a 64-bit integer. For example, "123.45" is stored as the integer 12345. This is a robust and standard technique in financial software.

3.  **Efficient Change Detection:**
    *   A helper function, `has_book_changed`, efficiently compares the current book state to the last printed state, preventing unnecessary output and processing.

4.  **Optimized Compilation:** The `Makefile` is configured to use the `-O3` flag, enabling the highest level of compiler optimizations for maximum performance.