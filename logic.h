#pragma once

#include <stdint.h>
#include <iostream>
#include <fstream>
#include <queue>


class Logic{
public:
    Logic();
    ~Logic();
    void add_number(int16_t new_number);
    void recalculate_average();

private:
    std::ofstream log_file;
    void check_necessity_and_log();

    long long m_all_numbers_counter = 0;
    long long m_current_row_sum = 0;
    float m_current_row_average = 0.0;

    std::queue<int16_t> m_early_nums{};
    std::queue<int16_t> m_late_nums{};
    int16_t m_median_elem = 0;
};