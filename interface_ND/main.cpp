/** @file main.cpp
@author   Ragheb Rahmaniani
@email    ragheb.rahmaniani@gmail.com
@version  02.01.0isr
@date     8/10/2018
@brief    This part creates and export the models that the solver ingests.
*/

#include <chrono>
#include <ilcplex/ilocplex.h>
#include <map>
#include <memory>
#include <set>

#include "util/docopt/docopt.h"
#include "util/pair_hasher.h"
#include "util/spdlog/spdlog.h"

#include "contrib/data.h"
#include "contrib/master_formulation.h"
#include "contrib/subproblem_formulation.h"

static const char USAGE[] =
    R"(Exports a .sav model for the master and each subproblem.
      Usage:
        ./main --num_nodes NODES --num_od OD --num_arcs ARCS --num_scenario SCENARIO --file_path FILE_PATH --scenario_path SCENARIO_PARTH --export_path=OUTPUT_DIR
        ./main (-h | --help)
        ./main --version
      Options:
        -h --help                       Show this screen.
        --version                       Show version.
        --num_nodes NODES               Specifies number of nodes in the graph.
        --num_od OD                     Specifies number of origin-destination pairs.
        --num_arcs ARCS                 Specifies number of arcs in the graph.
        --num_scenario SCENARIO         Gives number of scenarios.
        --file_path FILE_PATH           Path to network file.
        --scenario_path SCENARIO_PARTH  Path to scenario file.
        --export_path OUTPUT_DIR        Path to the export folder dir.
)";

int main(int argc, char *argv[]) {
  const auto console = spdlog::stdout_logger_mt("stdout");
  console->info(" -Parsing the args...");
  const std::map<std::string, docopt::value> args =
      docopt::docopt(USAGE, {argv + 1, argv + argc}, true, "02.01.0isr");

  console->info("-Loading the data...");
  std::unique_ptr<Data> data = std::make_unique<Data>();
  {
    data->num_scenario = std::stoi(args.at("--num_scenario").asString());
    data->num_nodes = std::stoi(args.at("--num_nodes").asString());
    data->num_arcs = std::stoi(args.at("--num_arcs").asString());
    data->num_od = std::stoi(args.at("--num_od").asString());
    GraphFileReader(args.at("--file_path").asString(), data, console);
    ScenarioFileReader(args.at("--scenario_path").asString(), data, console);
  }
  console->info("-Exporting the models...");
  {
    const std::string export_path = args.at("--export_path").asString();
    ExportMasterModel(data, console, export_path);
    ExportSubproblemModels(data, console, export_path);
  }
  console->info("-Done exporting!");
  return 0;
} // end main
