#include "logic.h"
#include <chrono>
#include <sstream>

namespace {
//    const int MAX_EARLY_ELEMENTS_NUMBER = 5000;
//    const int MAX_LATE_ELEMENTS_NUMBER = 5000;
    const int MAX_EARLY_ELEMENTS_NUMBER = 2;
    const int MAX_LATE_ELEMENTS_NUMBER = 2;
    const int ALL_NUMBERS_MAX_NUMBER = MAX_LATE_ELEMENTS_NUMBER + MAX_EARLY_ELEMENTS_NUMBER + 1;
}

Logic::Logic() {
    log_file.open("/tmp/example.log");
    if (log_file.fail()){
        std::cerr << "[ERROR]: Can't open /tmp/example.log, is the directory writable?" << std::endl;
    }
}

Logic::~Logic() {
    if (log_file.is_open()) {
        log_file.close();
    }
}

void Logic::add_number(int16_t new_element) {
    /*
     * I've decided to use two queues here. As it will be
     * more effective to manage middle element that way.
     *
     * Whole data will be represented as:
     * |----5000---|      median     | ----5000----|   <--new num inserted from that tail
     * m_late_nums    m_median_elem   m_early_nums
     *
     * first nums here                 last nums here (insert here)
     */
    if (m_all_numbers_counter > MAX_EARLY_ELEMENTS_NUMBER) {
        m_late_nums.push(m_median_elem);
    }
    if (m_all_numbers_counter >= MAX_EARLY_ELEMENTS_NUMBER) {
        m_median_elem = m_early_nums.front();
        m_early_nums.pop();
    }
    if (m_all_numbers_counter >= ALL_NUMBERS_MAX_NUMBER) {
        m_current_row_sum -= m_late_nums.front();
//        std::cout << "[LOGIC] Popped number: " << m_late_nums.front() << std::endl;
        m_late_nums.pop();
    }
    m_early_nums.push(new_element);
    m_current_row_sum += new_element;

//    std::cout << "[LOGIC] New insert! early size: " << m_early_nums.size() << ", median num: "
//              << m_median_elem << ", late size: " << m_late_nums.size() << std::endl;
    m_all_numbers_counter++;
    recalculate_average();
    check_necessity_and_log();
}

void Logic::recalculate_average() {
    // Average makes sense only if we have enough numbers inserted
    if ( m_all_numbers_counter >= ALL_NUMBERS_MAX_NUMBER) {
        m_current_row_average = static_cast<float>(m_current_row_sum) / ALL_NUMBERS_MAX_NUMBER;
        std::cout << "[LOGIC] Average recalculated: " << m_current_row_sum << " " << m_current_row_average << std::endl;
    }
}

void Logic::check_necessity_and_log() {
    if (m_all_numbers_counter < ALL_NUMBERS_MAX_NUMBER){
        // not enough numbers to check, we need all 10001 to get the avg
        return;
    }

    if (m_median_elem > (m_current_row_average)) {  // 5*average(x[max_pos-5000]...x[max_pos+5000])
        unsigned long long log_data_position_since_start = m_all_numbers_counter - MAX_EARLY_ELEMENTS_NUMBER;
        int16_t log_data_data_maximum_value = m_median_elem;
        // x[max_pos] / average(x[max_pos-5000]...x[max_pos+5000])
        float log_data_comparison =
                (m_median_elem ? static_cast<float>(m_median_elem) / static_cast<float>(m_current_row_average) : 0);
        unsigned long ms_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        unsigned long seconds_since_epoch = ms_since_epoch / 1000;
        unsigned long ms = ms_since_epoch % 1000;

        std::stringstream ss << "Local maximum is found! Position: " << log_data_position_since_start
        << ", Value: " << log_data_data_maximum_value
        << ", In comparison to avg: " << log_data_comparison
        << ", Seconds since epoch start: " << seconds_since_epoch << "." << ms;
        std::string log_string = ss.str();

        if (log_file.is_open()){
            std::cout << "Writing to log" << std::endl;
            log_file << log_string << std::endl;
        }
        std::cout << log_string << std::endl;
    }
}
