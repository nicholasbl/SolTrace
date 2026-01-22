pragma Singleton
import QtQuick 2.15

// Very rough sketch of session data, will replace indices with enums later
QtObject {
    property QtObject core: QtObject {
        property int workflowIndex: 0 // Configure, Simulate, Analyze, etc
        property int sectionIndex: 0 // Section of the workflow phase (example: Materials)

            
        property QtObject sun: QtObject {
            property int sunTypeIndex: 0 // Directional or point source sun
            property int calculatorIndex: 0 // Position calculator (legacy, SOLPOS, etc)
            property int sunShapeIndex: 0 // Emission profile type (Gaussian, Pillbox, etc)

            property QtObject directionalSun: QtObject {
                // Location
                property real latitude: 35.0
                property real longitude: -105.0

                // Date
                property int year: 2026
                property int month: 12
                property int day: 25

                // Time
                property int hour: 14
                property int minute: 30
                property int second: 0
                property int timezone: -7

                // SOLPOS specific
                property int interval: 1  // seconds

                // SPA specific (optional parameters)
                property real deltaUt1: 0.0  // seconds, range: -1 to 1
                property real altitude: 1000.0  // meters, elevation above sea level
                property real pressure: 1013.25  // mbar (standard atmospheric pressure)
                property real temperature: 20.0  // Celsius
            }

            property QtObject pointSourceSun: QtObject {
                property real x: 1000.0
                property real y: 1000.0
                property real z: 1000.0
            }

            property QtObject shape: QtObject {
                property QtObject gaussian: QtObject {
                    property real stdValue: 5.18
                }
                property QtObject pillbox: QtObject {
                    property real halfWidthValue: 4.65
                }
                property QtObject buie: QtObject {
                    property real circumsolarRatioValue: 2.0
                }
                property QtObject custom: QtObject {
                    property int numberOfPoints: 1
                }
            }
        }

        property QtObject tracing: QtObject {
            property int tracerIndex: 0

            // Engine properties
            property real maxIntersections: 3.5e6
            property real maxRays: 1e6
            property bool sunShape: false

            // Execution properties
            property int cpuCores: 4
            property int seedValue: 12345

            // Optimization properties
            property bool opticalErrors: false
            property bool pointFocusSystem: false
        }
    }
}
