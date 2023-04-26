#include "inifile.hpp"
#include "spdlog_common.h"
#include "slight.hpp"
#include "utils.hpp"
#include <unordered_map>

// 全局定位map
std::unordered_map<std::string, std::string> g_map = {
    {"gnp_inc-flow_with_loop.ini", "gnp_w_loop_wo_leakage_"},
    {"gnp_inc-flow.ini", "gnp_wo_loop_wo_leakage_"},
    {"gnp_leakage_with_loop.ini", "gnp_w_loop_w_leakage_"},
    {"gnp_leakage.ini", "gnp_wo_loop_w_leakge_"},
    {"gnp-local_inc-flow_with_loop.ini", "gnpLoc_w_loop_wo_leakage_"},
    {"gnp-local_inc-flow.ini", "gnpLoc_wo_loop_wo_leakage_"},
    {"gnp-local_leakage_with_loop.ini", "gnpLoc_w_loop_w_leakage_"},
    {"gnp-local_leakage.ini", "gnpLoc_wo_loop_w_leakage_"},
    {"grid_inc-flow.ini", "grid_wo_loop_wo_leakage_"},
    {"grid_leakage.ini", "grid_wo_loop_w_leakage_"}
};

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
    int convergence_check_period = ini.get_int("convergence_check_period");
    int num_instances = ini.get_int("num_instances");
    int start_seed = ini.get_int("start_seed");
    bool min_leakage = ini.get_bool("min_leakage");
    std::string graph_type = ini.get_string("graph_type");
    bool with_loop = graph_type == "grid" ? false : ini.get_bool("with_loop");
    bool no_lin = ini.get_bool("no_lin");

    int pos = path.find('/', path.find('/') + 1);
    std::string conf_name = path.substr(pos + 1);
    std::string data_prefix = "dataset/data/" + g_map[conf_name];

    if (no_lin) {
        return 0;
    }

    Slight slight(conv_thold, decay, max_iter, print_period, convergence_check_period, start_seed);
    for (int i = 0; i < num_instances; i++) {
        slight.seed++;
        slight.init(data_prefix + std::to_string(i) + ".txt");
        slight.run();
    }

    return 0;
}
