#ifndef CONTRIB_MP_H
#define CONTRIB_MP_H

/*!
  # This is where the user exports the initial master formulation.
  # WARNING: Numbering of the master variables should match the copied variables
  in subproblems.
  # WARNING: recourse variables must be name with "theta_{subproblem id}"
  # WARNING: Exported model must be named as MP.sav
*/

void ExportMasterModel(const std::unique_ptr<Data> &data,
                       const std::shared_ptr<spdlog::logger> console,
                       const std::string &export_path) {
  IloEnv env;
  IloModel model;
  IloCplex cplex;
  IloObjective objective;
  IloNumVarArray y_var, theta;
  IloRangeArray constraints;
  model = IloModel(env);
  cplex = IloCplex(model);
  objective = IloMinimize(env);
  constraints = IloRangeArray(env);

  y_var = IloNumVarArray(env, data->num_arcs, 0, 1);
  for (const auto &arc : data->arcs_info) {
    const int arc_id = arc.second.arc_id;
    y_var[arc_id].setName(("y_" + std::to_string(arc_id)).c_str());
  }
  theta = IloNumVarArray(env, data->num_scenario, 0, IloInfinity);
  for (int sp_id = 0; sp_id < data->num_scenario; sp_id++) {
    theta[sp_id].setName(("theta_" + std::to_string(sp_id)).c_str());
  }
  model.add(y_var);
  model.add(theta);

  console->info("    Adding master objective...");
  {
    IloExpr expr(env);
    for (const auto &it : data->arcs_info) {
      const int arc_id = it.second.arc_id;
      const double fixed_cost = it.second.fixed_cost;
      assert(arc_id < data->arcs_info.size());
      expr += fixed_cost * y_var[arc_id];
    }
    for (int sp_id = 0; sp_id < data->num_scenario; sp_id++) {
      expr += data->scenario_probability[sp_id] * theta[sp_id];
    }
    objective.setExpr(expr);
    (model).add(objective);
    expr.end();
  }
  console->info("    No master constraint to add!");
  console->info("    Exporting formulation for master problem...");
  cplex.setWarning(env.getNullStream());
  cplex.exportModel((export_path + "/MP.sav").c_str());
  console->info("    Done!");
}
#endif
