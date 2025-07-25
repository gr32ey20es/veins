#include <vector>
#include <fstream>
#include <iostream>

#include "nlohmann/json.hpp"
#include "ortools/base/init_google.h"
#include "ortools/sat/cp_model.h"


namespace operations_research {
namespace sat {

class Nishida2019
{
private:
    // ENVIRONMENT
    const char* FILENAME = "nishida2019.json";

    // Parameters
    int32_t num_vehicles, num_periods, num_nodes, num_tasks;
    int32_t num_constraints, alpha, beta;

    // System layout
    std::vector <std::vector <int32_t>> edges, reversed_edges;
    std::vector <int32_t> sm, u, g, Pp, Pd;


private:
    // x[m 1][t 0][i 1][j 1];
    std::vector <std::vector <std::vector <std::vector <BoolVar>>>> x;
    // y[m 1][l 1]
    std::vector <std::vector <BoolVar>> y;
    // K,D[l 1][t 0]
    std::vector <std::vector <BoolVar>> K, D;
    // B,C[m 1][l 1][t 0]
    std::vector <std::vector <std::vector <BoolVar>>> B, C;
    // f,e[l 1]
    std::vector <IntVar> f, e;


    // 1 if the constraint is included
    std::vector <bool> is_constraint;
    
    // Model
    CpModelBuilder builder;

private:
    bool is_init = false;

protected:
    void input (const char* filename);
    void initVariables (void);
    void initConstraints (void);
    void initObjFunction (void);
    void getResult (void);

public:
    void run (void);
};


void
Nishida2019::input (const char* filename) {
    std::ifstream fJson (filename);
    std::stringstream buffer;
    buffer << fJson.rdbuf ();

    auto json   = nlohmann::json::parse (buffer.str ());
    auto params = json["params"]; 
    auto arcs   = json["layout"]["arcs"];
    auto startm = json["layout"]["sm"];
    auto ul    = json["tasks"]["ul"];
    auto gl    = json["tasks"]["gl"];
    auto Pl_p  = json["tasks"]["Pl_p"];
    auto Pl_d  = json["tasks"]["Pl_d"];

    // Parameters
    num_vehicles    = params["num_vehicles"];
    num_periods     = params["num_periods"];
    num_nodes       = params["num_nodes"]; 
    num_tasks       = params["num_tasks"];
    num_constraints = params["num_constraints"];
    alpha           = params["alpha"];
    beta            = params["beta"];

    // System layout
    {   
        edges.push_back ({});
        reversed_edges.push_back ({});
    } // i = 0;
    for (int i = 1; i <= num_nodes; ++i)
    {
        edges.push_back ({i});
        reversed_edges.push_back ({i});
    }
    
    for (auto edge : arcs)
    {
        int32_t edge0 = edge[0];
        int32_t edge1 = edge[1];
        edges[edge0].push_back (edge1);
        reversed_edges[edge1].push_back (edge0);
    }

    sm.push_back (-1);
    for (int start : startm)
        sm.push_back (start);

    {
        u.push_back ({});
        g.push_back ({});
        Pp.push_back ({});
        Pd.push_back ({});
    } // l = 0
    for (int l = 1; l <= num_tasks; ++l)
    {
        u.push_back (ul[l-1]);
        g.push_back (gl[l-1]);
        Pp.push_back (Pl_p[l-1]);
        Pd.push_back (Pl_d[l-1]);
    }

    // Init
    is_constraint.push_back (false);
    for (int i = 1; i <= num_constraints; ++i)
        is_constraint.push_back (true);
}

void
Nishida2019::initVariables (void)
{
    char buffer[100];
    int32_t max32 = std::numeric_limits <int32_t>::max();
    
    // x
    for (int m = 0; m <= num_vehicles; ++m)
    {
        x.push_back ({});
        for (int t = 0; t <= num_periods; ++t)
        {
            x[m].push_back ({});
            for (int i = 0; i <= num_nodes; ++i)
            {
                x[m][t].push_back ({});
                for (int j = 0; j <= num_nodes; ++j)
                    x[m][t][i].push_back ({});
            }
        }
    }

    for (int m = 1; m <= num_vehicles; ++m)
        for (int t = 0; t <= num_periods; ++t)
            for (int i = 1; i <= num_nodes; ++i)
                for (int j : edges[i])
                {
                    sprintf (buffer, "x_%d_%d_%d_%d", m, t, i, j);
                    x[m][t][i][j] = builder.NewBoolVar ().WithName (buffer);
                }


    // y
    for (int m = 0; m <= num_vehicles; ++m)
    {
        y.push_back ({});
        for (int l = 0; l <= num_tasks; ++l)
            y[m].push_back ({});
    }
    
    for (int m = 1; m <= num_vehicles; ++m)
        for (int l = 1; l <= num_tasks; ++l)
        {
            sprintf (buffer, "y_%d_%d", m, l);
            y[m][l] = builder.NewBoolVar ().WithName (buffer);
        }


    // K, D
    for (int l = 0; l <= num_tasks; ++l)
    {
        K.push_back ({});
        D.push_back ({});
        for (int t = 0; t <= num_periods; ++t)
        {
            K[l].push_back ({});
            D[l].push_back ({});
        }
    }

    for (int l = 1; l <= num_tasks; ++l)
        for (int t = 0; t <= num_periods; ++t)
        {
            sprintf (buffer, "K_%d_%d", l, t);
            K[l][t] = builder.NewBoolVar ().WithName (buffer);

            sprintf (buffer, "D_%d_%d", l, t);
            D[l][t] = builder.NewBoolVar ().WithName (buffer);
        }


    // B, C
    for (int m = 0; m <= num_vehicles; ++m)
    {
        B.push_back ({});
        C.push_back ({});
        for (int l = 0; l <= num_tasks; ++l)
        {
            B[m].push_back ({});
            C[m].push_back ({});
            for (int t = 0; t <= num_periods; ++t)
            {
                B[m][l].push_back ({});
                C[m][l].push_back ({});
            }
        }
    }

    for (int m = 1; m <= num_vehicles; ++m)
        for (int l = 1; l <= num_tasks; ++l)
            for (int t = 0; t <= num_periods; ++t)
            {
                sprintf (buffer, "B_%d_%d_%d", m, l, t);
                B[m][l][t] = builder.NewBoolVar ().WithName (buffer);

                sprintf (buffer, "C_%d_%d_%d", m, l, t);
                C[m][l][t] = builder.NewBoolVar ().WithName (buffer);
            }
    

    // f, e
    for (int l = 0; l <= num_tasks; ++l)
    {
        f.push_back ({});
        e.push_back ({});
    }
    
    for (int l = 1; l <= num_tasks; ++l)
    {
        sprintf (buffer, "f_%d", l);
        f[l] = builder.NewIntVar ({0, max32/2}).WithName (buffer);

        sprintf (buffer, "e_%d", l);
        e[l] = builder.NewIntVar ({0, max32/2}).WithName (buffer);
    }
}

void
Nishida2019::initConstraints (void)
{   
    int ul, gl;
    LinearExpr expr;
    std::vector <BoolVar> vars, vars_1;
    std::vector <int64_t> coeffs;

    int32_t min32 = std::numeric_limits <int32_t>::min();

    if (is_constraint[2])

    for (int m = 1; m <= num_vehicles; ++m)
        for (int t = 0; t <= num_periods; ++t)
            for (int i = 1; i <= num_nodes; ++i)
            {
                vars.clear ();
                for (int j : edges[i])
                    vars.push_back (x[m][t][i][j]);

                builder.AddLessOrEqual (LinearExpr::Sum (vars), 1);
            }
    LOG (INFO) << 2;


    if (is_constraint[3])

    for (int m = 1; m <= num_vehicles; ++m)
        for (int t = 0; t <= num_periods; ++t)
        {
            vars.clear ();
            for (int i = 1; i <= num_nodes; ++i)
                for (int j : edges[i])
                    vars.push_back (x[m][t][i][j]);

            builder.AddEquality (LinearExpr::Sum (vars), 1);
        }
    LOG (INFO) << 3;


    if (is_constraint[4])

    for (int m = 1; m <= num_vehicles; ++m)
        for (int t = 0; t < num_periods; ++t)
            for (int i = 1; i <= num_nodes; ++i)
            {
                vars.clear ();
                for (int j : reversed_edges[i])
                    vars.push_back (x[m][t][j][i]);

                vars_1.clear ();
                for (int j : edges[i])
                    vars_1.push_back (x[m][t+1][i][j]);

                builder.AddEquality (LinearExpr::Sum (vars), 
                                     LinearExpr::Sum (vars_1));
            }
    LOG (INFO) << 4;


    if (is_constraint[5])

    for (int t = 0; t <= num_periods; ++t)
        for (int i = 1; i <= num_nodes; ++i)
        {
            vars.clear ();
            for (int m = 1; m <= num_vehicles; ++m)
                for (int j : reversed_edges[i])
                    vars.push_back (x[m][t][j][i]);

            builder.AddLessOrEqual (LinearExpr::Sum (vars), 1);
        }
    LOG (INFO) << 5;


    if (is_constraint[6])
    
    for (int t = 0; t <= num_periods; ++t)
        for (int i = 1; i <= num_nodes; ++i)
            for (int j : edges[i])
                if (i != j)
                {
                    vars.clear ();
                    
                    for (int m = 1; m <= num_vehicles; ++m)
                    {
                        vars.push_back (x[m][t][i][j]);
                        
                        // Is a bidirectional edge and only i < j;
                        if (i < j && x[m][t][j][i].index() != min32)
                            vars.push_back (x[m][t][j][i]);
                    }

                    builder.AddLessOrEqual (LinearExpr::Sum (vars), 1);
                }
    LOG (INFO) << 6;
    
    
    if (is_constraint[7])
    
    for (int m = 1; m <= num_vehicles; ++m)
        for (int t = 0; t <= num_periods; ++t)
            for (int i = 1; i <= num_nodes; ++i)
            {
                vars.clear ();

                for (int j : reversed_edges[i])
                    vars.push_back (x[m][t][j][i]);

                for (int m2 = 1; m2 <= num_vehicles; ++m2)
                    for (int j : edges[i])
                        if (m2 != m)
                            vars.push_back (x[m2][t][i][j]);
                
                builder.AddLessOrEqual (LinearExpr::Sum (vars), 1);
            }
    LOG (INFO) << 7;


    if (is_constraint[8])

    for (int m = 1; m <= num_vehicles; ++m)
    {   
        int i = sm[m];

        vars.clear ();
        for (int j : edges[i])
            vars.push_back (x[m][0][i][j]);

        builder.AddEquality (LinearExpr::Sum (vars), 1);
    }
    LOG (INFO) << 8;


    if (is_constraint[9])

    for (int m = 1; m <= num_vehicles; ++m)
    {
        int excluded_i = sm[m];

        vars.clear ();
        for (int i = 1; i <= num_nodes; ++i)
            if (i != excluded_i)
            {
                for (int j : edges[i])
                    vars.push_back (x[m][0][i][j]);
            }

        builder.AddEquality (LinearExpr::Sum (vars), 0);
    }
    LOG (INFO) << 9;


    if (is_constraint[10])

    for (int l = 1; l <= num_tasks; ++l)
        {
            vars.clear ();
            for (int m = 1; m <= num_vehicles; ++m)
                vars.push_back (y[m][l]);

            builder.AddLessOrEqual (LinearExpr::Sum (vars), 1);
        }
    LOG (INFO) << 10;


    if (is_constraint[11])

    for (int m = 1; m <= num_vehicles; ++m)
        {
            vars.clear ();
            for (int l = 1; l <= num_tasks; ++l)
                vars.push_back (y[m][l]);

            builder.AddLessOrEqual (LinearExpr::Sum (vars), 1);
        }
    LOG (INFO) << 11;


    if (is_constraint[12])

    for (int l = 1; l <= num_tasks; ++l)
        for (int t = 0; t < num_periods; ++t)
        {
            vars.clear ();
            for (int m = 1; m <= num_vehicles; ++m)
                vars.push_back (B[m][l][t]);
            vars.push_back (K[l][t+1]);

            builder.AddLessOrEqual (K[l][t], LinearExpr::Sum (vars));
        }
    LOG (INFO) << 12;


    if (is_constraint[13])

    for (int l = 1; l <= num_tasks; ++l)
        for (int t = 0; t < num_periods; ++t)
        {
            vars.clear ();
            for (int m = 1; m <= num_vehicles; ++m)
                vars.push_back (B[m][l][t]);
            vars.push_back (K[l][t+1]);

            builder.AddLessOrEqual (LinearExpr::Sum (vars), 1);
        }
    LOG (INFO) << 13;


    if (is_constraint[14])

    for (int l = 1; l <= num_tasks; ++l)
        for (int t = 0; t < num_periods; ++t)
        {
            vars.clear ();
            for (int m = 1; m <= num_vehicles; ++m)
                vars.push_back (C[m][l][t]);
            vars.push_back (D[l][t+1]);

            builder.AddLessOrEqual (D[l][t], LinearExpr::Sum (vars));
        }
    LOG (INFO) << 14;


    if (is_constraint[15])

    for (int l = 1; l <= num_tasks; ++l)
        for (int t = 0; t < num_periods; ++t)
        {
            vars.clear ();
            for (int m = 1; m <= num_vehicles; ++m)
                vars.push_back (C[m][l][t]);
            vars.push_back (D[l][t+1]);

            builder.AddLessOrEqual (LinearExpr::Sum (vars), 1);
        }
    LOG (INFO) << 15;


    if (is_constraint[16])
        
    for (int l = 1; l <= num_tasks; ++l)
    {
        builder.AddEquality (K[l][0], 1);
        builder.AddEquality (D[l][0], 1);
    }
    LOG (INFO) << 16;


    if (is_constraint[17])

    for (int l = 1; l <= num_tasks; ++l)
    {
        for (int t = 0; t < num_periods; ++t)
        {
            builder.AddLessOrEqual (K[l][t+1], K[l][t]);
            builder.AddLessOrEqual (D[l][t+1], D[l][t]);
            builder.AddLessOrEqual (K[l][t], D[l][t]);
        }
        builder.AddLessOrEqual (K[l][num_periods], D[l][num_periods]);
    }
    LOG (INFO) << 17;


    if (is_constraint[18])

    for (int l = 1; l <= num_tasks; ++l)
    {
        vars.clear ();
        for (int t = 0; t <= num_periods; ++t)
            vars.push_back (D[l][t]);

        builder.AddGreaterOrEqual (LinearExpr::Sum (vars) + f[l], Pd[l]);
        builder.AddLessOrEqual (LinearExpr::Sum (vars), f[l] + Pd[l]);
    }
    LOG (INFO) << 18;


    if (is_constraint[19])

    for (int l = 1; l <= num_tasks; ++l)
    {
        vars.clear ();
        for (int t = 0; t <= num_periods; ++t)
            vars.push_back (K[l][t]);

        builder.AddGreaterOrEqual (LinearExpr::Sum (vars) + e[l], Pp[l]);
        builder.AddLessOrEqual (LinearExpr::Sum (vars), e[l] + Pp[l]);
    }
    LOG (INFO) << 19;


    if (is_constraint[24])
    // 25-30 in the journal version
    for (int m = 1; m <= num_vehicles; ++m)
        for (int l = 1; l <= num_tasks; ++l)
            for (int t = 0; t <= num_periods; ++t)
            {
                ul = u[l]; gl = g[l];


                builder.AddGreaterOrEqual (
                    1 + B[m][l][t], x[m][t][ul][ul] + y[m][l]);
                
                builder.AddGreaterOrEqual (x[m][t][ul][ul], B[m][l][t]);
                builder.AddGreaterOrEqual (y[m][l], B[m][l][t]);


                builder.AddGreaterOrEqual (
                    1 + C[m][l][t], x[m][t][gl][gl] + y[m][l]);

                builder.AddGreaterOrEqual (x[m][t][gl][gl], C[m][l][t]); builder.AddGreaterOrEqual (y[m][l], C[m][l][t]); }
    LOG (INFO) << 24;
}

void
Nishida2019::initObjFunction (void)
{
    DoubleLinearExpr expr;
    std::vector <IntVar> vars;
    std::vector <BoolVar> vars_1;

    vars.clear ();
    for (int l = 1; l <= num_tasks; ++l)
    {   
        vars.push_back (f[l]);
        vars.push_back (e[l]);
    }

    vars_1.clear ();
    for (int l = 1; l <= num_tasks; ++l)
        for (int t = 0; t <= num_periods; ++t)
            vars_1.push_back (D[l][t]);

    expr.AddExpression (LinearExpr::Sum (vars), alpha)
        .AddExpression (LinearExpr::Sum (vars_1), beta);
    
    //builder.Minimize (expr);
    builder.Minimize (LinearExpr::Sum (vars) + LinearExpr::Sum (vars_1));
}

void
Nishida2019::getResult (void)
{
    CpSolverResponse response; 

    response = Solve (builder.Build ());
    if (response.status () == CpSolverStatus::OPTIMAL ||
        response.status () == CpSolverStatus::FEASIBLE)
    {
        LOG (INFO) << "Min: " << response.objective_value ();

        for (int m = 1; m <= num_vehicles; ++m)
        {
            LOG (INFO) << "Vehicle " << m << ":";
            for (int t = 0; t <= num_periods; ++t)
                for (int i = 1; i <= num_nodes; ++i)
                    for (int j : edges[i])
                        if (SolutionBooleanValue (response, x[m][t][i][j]))
                            LOG (INFO) << "t = " << t << ": (" 
                                       << i << ", " << j << ")"; 
        }

        LOG (INFO) << "---------------------------";

        for (int l = 1; l <= num_tasks; ++l)
        {
            LOG (INFO) << "Task " << l << ":";
            for (int t = 0; t <= num_periods; t++)
                LOG (INFO) << SolutionBooleanValue(response, K[l][t])
                           << " " 
                           << SolutionBooleanValue(response, D[l][t]);
        }

        LOG (INFO) << "--------------------------";

        for (int l = 1; l <= num_tasks; ++l)
        {
            LOG (INFO) << u[l] << " " << g[l] << " " << Pp[l] << " " << Pd[l];
        }
    }
    else
        LOG (INFO) << "No solution found";

    LOG(INFO) << "Problem solved in " 
              << response.wall_time() 
              << " milliseconds";
}

void
Nishida2019::run (void)
{
    input (FILENAME); 
    LOG (INFO) << "input: done";
    
    initVariables ();
    LOG (INFO) << "initVariables: done";

    {
        //is_constraint[2]  = false; 
        //is_constraint[3]  = false;
        
        //is_constraint[6]  = false;
         
        //is_constraint[12] = false;
        //is_constraint[14] = false;
        
        //is_constraint[7]  = false;
        
        // Constraints 10 & 11 are relaxed due to no solution;
        // Some constrants are in the journal version and are indexed (i+1)
        initConstraints ();
        LOG (INFO) << "initConstraints: done";
    }

    initObjFunction ();
    LOG (INFO) << "initObjFunction: done";

    getResult ();
    LOG (INFO) << "getResult: done";
}

} // namespace sat
} // namespace operations_research


int
main (int argc, char* argv[])
{
    InitGoogle (argv[0], &argc, &argv, true);
    absl::SetStderrThreshold (absl::LogSeverityAtLeast::kInfo); 
    operations_research::sat::Nishida2019 nishida2019;

    nishida2019.run ();

    return EXIT_SUCCESS;
}
