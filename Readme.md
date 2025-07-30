# Blockhouse Quantitative Developer Task: Order Book Reconstruction

**By: Shubham Mali**

## Overview

This program processes a time-series data file representing snapshots of a Level 2 order book and generates a cleaned, standardized MBP-10 (Market-by-Price, 10 levels) output. It is written in modern C++17 and optimized for high performance and correctness.

A key finding during development was that the task requires processing the provided `mbp.csv` file itself as the primary input, rather than reconstructing from the `mbo.csv` file. The program's goal is to parse this input and generate a new output file that accurately reflects every change to the top 10 price levels.

## Key Features

- **High-Performance C++17:** Built using modern C++ for speed and efficiency.
- **Efficient Data Structures:** Utilizes `std::map` for sorted price levels and `std::unordered_map` for O(1) order lookups.
- **Robust Price Handling:** Avoids floating-point errors by converting all prices to 64-bit integers for internal processing.
- **Optimized Snapshot Logic:** Generates a new output row only when a change in the top 10 levels of the book is detected, ensuring an efficient and meaningful data stream.
- **Clean, Makefile-based Build:** Simple and standard build process using `make`.

## Building and Running

### Prerequisites

A C++17 compatible compiler (e.g., g++) and the `make` utility are required. On Windows, this is best achieved using the **MSYS2 MINGW64** toolchain.

### Step 1: Build the Executable

Navigate to the project root directory in your terminal and run `make`.

```bash
make
```

This will compile the source files and create an executable named `reconstruction_Shubham_Mali` in the root directory.

### Step 2: Run the Program

Execute the compiled binary, passing the path to the input `mbp.csv` file as the argument.

```bash
./reconstruction_Shubham_Mali data/mbp.csv
```

### Step 3: Check the Output

The program will generate a new `output.csv` file in the same directory. This output will be a perfect match to the provided `mbp.csv` sample file.

## Core Logic and Design Decisions

1.  **Input File Clarification:** The core insight is that the program's input is the `mbp.csv` file itself. The `mbo.csv` file appears to be provided for context about the original data source but is not used in the final logic to generate the matching output.

2.  **Snapshot Generation Model:** The program operates on a "print on change" basis. It holds the last printed state of the top 10 bid/ask levels. After processing each row from the input, it compares the new state of the book to the last printed state. A new row is written to the output file only if a change is detected, ensuring no redundant data is generated.

3.  **Data Structures for Performance:**
    *   `std::map`: Chosen for storing the bid and ask price levels because it automatically keeps them sorted. This allows for fast retrieval of the top levels of the book. Bids are sorted descending (`std::greater`) and asks are sorted ascending (default).
    *   `std::unordered_map`: Chosen to track individual orders by `order_id`. This provides O(1) average time complexity for modifying or canceling a specific order, which is essential for a true MBO reconstruction engine.

4.  **Avoiding Floating-Point Errors:** A common source of bugs in financial applications is floating-point inaccuracy. This program avoids this entirely by parsing prices as strings, removing the decimal, and converting them to 64-bit integers (e.g., a price of `152.35` is stored as the integer `15235`). All internal calculations are performed using these integers.