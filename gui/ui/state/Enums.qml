pragma Singleton
import QtQuick

QtObject {
    enum SunShape {
        Gaussian = 0,
        Pillbox = 1,
        BuieCSR = 2,
        Custom = 3
    }

    enum SunType {
        Directional = 0,
        PointSource = 1
    }

    enum PositionCalculator {
        Legacy = 0,
        Duffie = 1,
        SOLPOS = 2,
        SPA = 3
    }

    enum WorkflowPhase {
        Configure = 0,
        Simulate = 1,
        Analyze = 2
    }

    enum ErrorType {
        Gaussian = 0,
        Pillbox = 1,
        Diffuse = 2
    }
}
