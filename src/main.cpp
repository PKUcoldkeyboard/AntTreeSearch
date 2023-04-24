#include "inifile.hpp"
#include "spdlog_common.h"
#include "baseline.hpp"
#include "utils.hpp"

int main() {
    // 读取配置文件
    auto ini = util::inifile("conf/linear/base.ini");
    if (!ini.good()) {
        SPDLOG_ERROR(ini.error());
        exit(-1);
    }
    
    double decay = ini.get_double("decay");
    double conv_thold = ini.get_double("conv_thold");
    int max_iter = ini.get_int("max_iter");
    int print_period = ini.get_int("print_period");

    Baseline baseline(conv_thold, decay, max_iter, print_period);
    baseline.init("dataset/data/gnp_w_loop_w_leakage_0.txt");
    baseline.run();

    return 0;
}