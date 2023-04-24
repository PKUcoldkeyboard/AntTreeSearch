#include "inifile.hpp"
#include "spdlog_common.h"
#include "baseline.hpp"

int main() {
    // 读取配置文件
    auto ini = util::inifile("conf/linear/base.ini");
    if (!ini.good()) {
        SPDLOG_ERROR(ini.error());
        exit(-1);
    }
    
    float decay = ini.get_double("decay");
    float conv_thold = ini.get_double("conv_thold");
    int max_iter = ini.get_int("max_iter");

    Baseline baseline(conv_thold, decay, max_iter);
    baseline.init("test.txt");
    baseline.run();

    return 0;
}