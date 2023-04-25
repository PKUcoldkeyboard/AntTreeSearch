#include "inifile.hpp"
#include "spdlog_common.h"
#include "baseline.hpp"
#include "utils.hpp"

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        SPDLOG_ERROR("Usage: {} <config file>", argv[0]);
        exit(-1);
    }
    std::string path = argv[1];
    // 读取配置文件
    auto ini = util::inifile(path);
    if (!ini.good()) {
        SPDLOG_ERROR(ini.error());
        exit(-1);
    }

    ini.show();
    
    double decay = ini.get_double("decay");
    double conv_thold = ini.get_double("conv_thold");
    int max_iter = ini.get_int("max_iter");
    int print_period = ini.get_int("print_period");

    Baseline baseline(conv_thold, decay, max_iter, print_period);
    baseline.init("dataset/data/gnpLoc_wo_loop_wo_leakage_55.txt");
    // baseline.init("test.txt");
    baseline.run();

    return 0;
}