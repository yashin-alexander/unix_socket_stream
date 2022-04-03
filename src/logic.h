#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include <queue>

class Logic {
public:
    Logic();
    ~Logic();
    void add_number(const int16_t new_number);

private:
    std::ofstream log_file;
    void check_necessity_and_log();
    void recalculate_average();

    long long m_all_numbers_counter = 0;
    long long m_current_row_sum = 0;
    float m_current_row_average = 0.0;

    std::queue<int16_t> m_early_nums {};
    std::queue<int16_t> m_late_nums {};
    int16_t m_median_elem = 0;
};