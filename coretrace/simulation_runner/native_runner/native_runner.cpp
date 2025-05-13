
#include "native_runner.hpp"

NativeRunner::NativeRunner() : SimulationRunner(),
                               simdata(nullptr)
{
}

NativeRunner::~NativeRunner()
{
    if (this->simdata != nullptr)
    {
        this->simdata = nullptr;
    }
}

RunnerStatus NativeRunner::initialize()
{
    return RunnerStatus::SUCCESS;
}

RunnerStatus NativeRunner::setup_simulation(const SimulationData *data)
{
    this->simdata = data;
    return RunnerStatus::SUCCESS;
}

RunnerStatus NativeRunner::update_simulation(const SimulationData *data)
{
    this->setup_simulation(data);
    return RunnerStatus::SUCCESS;
}

RunnerStatus NativeRunner::run_simulation()
{
    return RunnerStatus::SUCCESS;
}

RunnerStatus NativeRunner::report_simulation(SimulationResult *result,
                                             int level)
{
    return RunnerStatus::SUCCESS;
}
