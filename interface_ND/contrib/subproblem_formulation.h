#ifndef CONTRIB_SP_H
#define CONTRIB_SP_H
/*!
  This is where the user exports the subproblem formulations.
    NOTE:The model should be the same as the determinstic problem
    NOTE: A model should be exported for each scenario!

    WARNING: Exported model must be named as SP_{s}.sav
            NOTE: Even if there is one subproblem, it must be named as SP_0.sav

    WARNING: Try to keep the structure of the subproblem formulations the same (
            i.e., there is one-to-one mapping between the constraints),
            otherwise some internal accelerations might get turned off

    WARNING: Constraints must be named as Reg_{number},

    WARNING: Subproblem must be a   minimization (if it is not, convert it)

    WARNING: copied master variables must be INTEGRAL

    WARNING: Copied variables MUST be named with 'z' and "z_{variable number}"

    WARNING: do not multiply to the scenario probability, because it is taken
            care of in the master formulation

    WARNING: The SP formulation must be equal to  the determistic version of the
            problem under scenario s, i.e., it will have objective of fTy +cTx
*/

typedef IloArray<IloNumVarArray> IloNumVarArray2;

void ExportSubproblemModels(const std::unique_ptr<Data> &data,
                            const std::shared_ptr<spdlog::logger> console,
                            const std::string &export_path) {
  for (int s = 0; s < data->num_scenario; ++s) {
    IloEnv env;
    IloModel model;
    IloCplex cplex;
    IloObjective objective;
    IloNumVarArray2 x_var;
    IloNumVarArray z_var;
    IloRangeArray constraints;
    model = IloModel(env);
    cplex = IloCplex(model);
    constraints = IloRangeArray(env);
    objective = IloMinimize(env);
    {
      z_var = IloNumVarArray(env, data->arcs_info.size(), 0, 1, ILOINT);
      (model).add(z_var);
      x_var = IloNumVarArray2(env, data->arcs_info.size());
      for (int a = 0; a < data->arcs_info.size(); ++a) {
        x_var[a] = IloNumVarArray(env, data->num_od, 0, IloInfinity);
        (model).add(x_var[a]);
      }
    }
    console->info(" Naming copied master variables...");
    {
      for (const auto &arc : data->arcs_info) {
        const int arc_id = arc.second.arc_id;
        z_var[arc_id].setName(("z_" + std::to_string(arc_id)).c_str());
      }
    }
    console->info(" Adding objective function...");
    {
      int num_arcs = 0;
      IloExpr expr(env);
      for (const auto &arc : data->arcs_info) {
        ++num_arcs;
        const int arc_id = arc.second.arc_id;
        const double flow_cost = std::abs(arc.second.flow_cost);
        const double fixed_cost = std::abs(arc.second.fixed_cost);
        expr += fixed_cost * z_var[arc_id];
        for (int k = 0; k < data->num_od; ++k) {
          expr += flow_cost * x_var[arc_id][k];
        }
      }
      objective.setExpr(expr);
      (model).add(objective);
      expr.end();
      assert(num_arcs == data->arcs_info.size());
    }
    console->info(" Adding flow conservation constraints...");
    {
      for (const auto &od : data->od_pair) {
        const int k = od.first;
        const int origin_node_id = od.second.first;
        const int destination_node_id = od.second.second;
        const double demand = std::fabs(data->demands[s][k]);
        assert(origin_node_id != destination_node_id);
        for (const auto &node : data->node_id_to_head_neighbors) {
          const int node_id = node.first;
          const bool is_origin = (node_id + 1 == origin_node_id) ? true : false;
          const bool is_destination =
              (node_id + 1 == destination_node_id) ? true : false;
          assert(is_destination == false || false == is_origin);
          IloExpr expr_exiting_edges(env);
          IloExpr expr_entering_edges(env);
          for (const int head : data->node_id_to_head_neighbors.at(node_id)) {
            const int arc_id =
                data->arcs_info.at(std::make_pair(node_id, head)).arc_id;
            expr_exiting_edges += x_var[arc_id][k];
          }
          for (const int tail : data->node_id_to_tail_neighbors.at(node_id)) {
            const int arc_id =
                data->arcs_info.at(std::make_pair(tail, node_id)).arc_id;
            expr_entering_edges += x_var[arc_id][k];
          }
          if (is_origin) {
            constraints.add(IloRange(
                env, demand, expr_exiting_edges - expr_entering_edges, demand));
          } else if (is_destination) {
            constraints.add(IloRange(
                env, demand, expr_entering_edges - expr_exiting_edges, demand));
          } else if (!is_destination && !is_origin) {
            constraints.add(
                IloRange(env, 0, expr_exiting_edges - expr_entering_edges, 0));
          } else {
            assert(false);
          }
          expr_exiting_edges.end();
          expr_entering_edges.end();
        }
      }
    }
    console->info(" Adding capacity constraints...");
    {
      for (const auto &arc : data->arcs_info) {
        const int arc_id = arc.second.arc_id;
        const double capacity = arc.second.capacity;
        assert(capacity >= 0);
        IloExpr expr(env);
        for (int k = 0; k < data->num_od; ++k) {
          expr += x_var[arc_id][k];
        }
        expr -= capacity * z_var[arc_id];
        constraints.add(IloRange(env, -IloInfinity, expr, 0));
        expr.end();
      }
    }
    console->info(" Adding string constraints...");
    {
      if (true) {
        for (const auto &arc : data->arcs_info) {
          const int arc_id = arc.second.arc_id;
          for (int k = 0; k < data->num_od; ++k) {
            constraints.add(
                IloRange(env, -IloInfinity,
                         x_var[arc_id][k] -
                             std::fabs(data->demands[s][k]) * z_var[arc_id],
                         0));
          }
        }
      }
    }
    console->info(" Naming subproblem constraints...");
    {  // naming constraints
      for (uint64_t i = 0; i < constraints.getSize(); ++i) {
        constraints[i].setName(("Reg_" + std::to_string(i)).c_str());
      }
    }
    model.add(constraints);

    console->info(" Exporting formulation for subproblem " + std::to_string(s) +
                  "...");
    {
      cplex.setWarning(env.getNullStream());
      cplex.exportModel(
          (export_path + "/SP_" + std::to_string(s) + ".sav").c_str());
    }
    console->info(" Checking if the subproblem is feasible....");
    {
      cplex.setOut(env.getNullStream());
      // this is only to faster check the model
      cplex.setParam(IloCplex::Param::MIP::Tolerances::MIPGap, 1.0);
      if (!cplex.solve()) {
        console->error(" Subproblem is infeasible!");
        cplex.exportModel("SP.lp");
        exit(911);
      }
    }
  }
}
#endif
