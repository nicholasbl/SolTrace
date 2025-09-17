# Unit Tests Reorganization Summary

## Overview
The unit tests are organized to mirror the directory structure found in the `coretrace` directory, creating separate test executables for each major component.

## Directory Structure

```
google-tests/unit-tests/
├── CMakeLists.txt                      # Main CMake file that includes subdirectories
├── common/                             # Shared source utilities
│   ├── common.cpp                     # Shared source helper functions
│   └── common.hpp                     # Shared source helper declarations
├── simulation_data/                    # Tests for coretrace/simulation_data
│   ├── CMakeLists.txt                 # Builds SimulationDataUnitTests executable
│   ├── aperture_test.cpp              # Tests for aperture classes
│   ├── surface_test.cpp               # Tests for surface classes
│   ├── container_test.cpp             # Tests for container template
│   ├── element_test.cpp               # Tests for element classes
│   ├── simulation_data_test.cpp       # Tests for main SimulationData class
│   ├── linear_algebra_test.cpp        # Tests for Vector3d/Matrix3d classes
│   └── cst-templates/                 # CST template tests (subdirectory)
│       ├── CMakeLists.txt             # Builds CSTUnitTests executable
│       ├── heliostat_test.cpp
│       ├── linear_fresnel_test.cpp
│       ├── parabolic_dish_test.cpp
│       └── parabolic_trough_test.cpp
├── simulation_results/                 # Tests for coretrace/simulation_results
│   ├── CMakeLists.txt                 # Builds SimulationResultsUnitTests executable
│   └── simulation_result_test.cpp
└── simulation_runner/                  # Tests for coretrace/simulation_runner
    ├── CMakeLists.txt                 # Builds SimulationRunnerUnitTests executable
    ├── flat_calculator_test.cpp       # Calculator tests
    ├── newton_calculator_test.cpp
    ├── parabola_calculator_test.cpp
    ├── sphere_calculator_test.cpp
    ├── cylinder_calculator_test.cpp
    ├── calculator_factory_test.cpp
    ├── native_runner_test.cpp         # Runner tests
    ├── tower_demo.cpp                 # Integration demos
    └── gpu_tower_demo.cpp             # Builds when Optix enabled
```

## Test Executables

### 1. SimulationDataUnitTests
- **Location**: `google-tests/unit-tests/simulation_data/SimulationDataUnitTests`
- **Tests**: All components from `coretrace/simulation_data`
- **Dependencies**: `simdata`, `GTest::gtest_main`
- **Test Count**: ~25 test suites covering apertures, surfaces, elements, containers, and linear algebra

### 2. CSTUnitTests  
- **Location**: `google-tests/unit-tests/simulation_data/cst-templates/CSTUnitTests`
- **Tests**: CST template classes (Heliostat, LinearFresnel, ParabolicDish, ParabolicTrough)
- **Dependencies**: `simdata`, `native_runner`, `GTest::gtest_main`
- **Test Count**: ~15 test suites covering template construction and tracing

### 3. SimulationResultsUnitTests
- **Location**: `google-tests/unit-tests/simulation_results/SimulationResultsUnitTests`
- **Tests**: Components from `coretrace/simulation_results`
- **Dependencies**: `simdata`, `simresult`, `GTest::gtest_main`
- **Test Count**: Tests for simulation result handling

### 4. SimulationRunnerUnitTests
- **Location**: `google-tests/unit-tests/simulation_runner/SimulationRunnerUnitTests`
- **Tests**: Components from `coretrace/simulation_runner` (native runner only)
- **Dependencies**: `simdata`, `simresult`, `native_runner`, `GTest::gtest_main`
- **Test Count**: ~30+ test suites covering calculators, runners, and integration demos
- **Note**: OptIX-related tests are currently disabled

## Benefits

1. **Modular Testing**: Each component can be tested independently
2. **Faster Builds**: Only rebuild tests for the component being modified
3. **Clear Organization**: Test structure mirrors source code structure
4. **Better Maintainability**: Easy to add new tests to the appropriate category
5. **Parallel Development**: Different teams can work on different test suites
6. **Shared Utilities**: Common test functions are maintained in a single location

## Running Tests

### Build Individual Test Executables
```bash
make SimulationDataUnitTests
make SimulationResultsUnitTests
make SimulationRunnerUnitTests
make CSTUnitTests
```

### Run Individual Test Suites
```bash
./google-tests/unit-tests/simulation_data/SimulationDataUnitTests
./google-tests/unit-tests/simulation_results/SimulationResultsUnitTests
./google-tests/unit-tests/simulation_runner/SimulationRunnerUnitTests
./google-tests/unit-tests/simulation_data/cst-templates/CSTUnitTests
```

### Run Specific Tests
```bash
./google-tests/unit-tests/simulation_data/SimulationDataUnitTests --gtest_filter="Aperture.*"
```

## Migration Notes

- The original `UnitTests` executable has been removed
- Common test utilities (`common.cpp`, `common.hpp`) have been moved to a shared `common/` directory
- All test files have been moved to appropriate subdirectories based on the coretrace structure
- CMake structure updated to build separate executables with shared common files
- Fixed regression test CMake file that incorrectly referenced old target names
