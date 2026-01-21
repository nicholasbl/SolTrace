pragma Singleton

import QtQuick 2.15

QtObject {
    id: docs
    readonly property string placeholderText1: "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi accumsan sed lectus id lacinia. Donec dui massa, eleifend non ipsum in, venenatis pulvinar arcu. Integer rutrum malesuada dictum. Sed tempor, arcu ac sodales pretium, nunc tellus vulputate purus, ut venenatis sapien arcu id turpis. Maecenas ornare tortor nibh, a porttitor dui finibus non."
    readonly property string placeholderText2: "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi accumsan sed lectus id lacinia. Donec dui massa, eleifend non ipsum in, venenatis pulvinar arcu. Integer rutrum malesuada dictum. Sed tempor, arcu ac sodales pretium, nunc tellus vulputate purus, ut venenatis sapien arcu id turpis. Maecenas ornare tortor nibh, a porttitor dui finibus non. <br><br> Proin lobortis felis ipsum. Mauris et sodales orci. Fusce iaculis rutrum dui in sodales. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Mauris gravida odio sed volutpat efficitur. Cras rutrum, felis eu vehicula mollis, nisl diam viverra nunc, vel suscipit nulla tellus quis justo. Sed scelerisque nulla quis enim ullamcorper porta. Fusce aliquet ante non erat lacinia, feugiat eleifend lorem pulvinar."
    readonly property string placeholderText3: "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi accumsan sed lectus id lacinia. Donec dui massa, eleifend non ipsum in, venenatis pulvinar arcu."

    readonly property QtObject sun: QtObject {
        property QtObject headers: QtObject {
            property string section: "Sun"
            property string sunTypes: "1. Sun Type & Direction"
            property string directionalSun: "1.1. Directional Sun"
            property string pointSourceSun: "1.2. Point Source Sun"

            property string calculators: "1.1.1. Directional Sun Position Calculators"
            property string legacyCalculator: "1.1.1.1. Legacy Calculator"
            property string duffieCalculator: "1.1.1.2. Duffie & Beckman Method"
            property string solposCalculator: "1.1.1.3. Solar Position (SOLPOS) Calculator"
            property string spaCalculator: "1.1.1.4. Solar Position Algorithm (SPA)"

            property string sunShapes: "2. Sun Shapes"
            property string gaussianProfile: "2.1 Gaussian Emission Profile"
            property string pillboxProfile: "2.2 Pillbox Emission Profile"
            property string buieProfile: "2.3 Buie Emission Profile"
            property string customProfile: "2.4 Custom Emission Profile"
        }

        property string intro: docs.placeholderText1

        property string sunTypes: docs.placeholderText1
        property string directionalSun: docs.placeholderText1
        property string pointSourceSun: docs.placeholderText2
        property string directionalSunDiagramCaption: "Infinite-distance source emitting exactly parallel rays"
        property string pointSourceSunDiagramCaption: "Finite-distance source emitting divergent rays"

        property string calculators: docs.placeholderText2
        property string legacyCalculator: docs.placeholderText1
        property string duffieCalculator: docs.placeholderText1
        property string solposCalculator: docs.placeholderText1
        property string spaCalculator: docs.placeholderText1

        property string sunShapes: docs.placeholderText2
        property string gaussianProfile: docs.placeholderText1
        property string pillboxProfile: docs.placeholderText1
        property string buieProfile: docs.placeholderText1
        property string customProfile: docs.placeholderText1
    }

    readonly property QtObject materials: QtObject {
        property QtObject headers: QtObject {
            property string materials: "Materials"
        }

        property string materials: docs.placeholderText2
        property string reflectance: docs.placeholderText3
        property string transmittance: docs.placeholderText3
        property string refractiveIndex: docs.placeholderText3
        property string slopeError: docs.placeholderText3
        property string specularityError: docs.placeholderText3
        property string errorType: docs.placeholderText3
        property string angularReflectance: docs.placeholderText3
        property string angularTransmittance: docs.placeholderText3
    }

    readonly property QtObject staging: QtObject {
        property QtObject headers: QtObject {
            property string staging: "Staging"
        }

        property string staging: docs.placeholderText1

    }

    readonly property QtObject tracing: QtObject {
        property QtObject headers: QtObject {
            property string tracing: "Tracing"
            property string tracer: "1. Ray Tracer"
            property string originalTracer: "1.1. Original Tracer"
            property string refactorTracer: "1.2. Refactor Tracer"
            property string gpuTracer: "1.3. GPU Tracer"

            property string engine: "2. Engine Parameters"
            property string maxIntersections: "2.1. Desired Number of Ray Intersections"
            property string maxRays: "2.2. Maximum Number of Generated Sun Rays"
            property string sunShape: "2.3. Sun Shape"

            property string execution: "3. Execution Parameters"
            property string cpuCores: "3.1. Number of CPU Cores"
            property string seed: "3.2. Seed Value"

            property string optimization: "4. Optimizations & Errors"
            property string opticalErrors: "4.1. Optical Errors"
            property string pointFocusSystem: "4.2. Point Focus System"
        }

        property string tracing: docs.placeholderText1

        property string tracer: docs.placeholderText1
        property string originalTracer: docs.placeholderText1
        property string refactorTracer: docs.placeholderText1
        property string gpuTracer: docs.placeholderText1

        property string engine: docs.placeholderText1
        property string maxIntersections: docs.placeholderText1
        property string maxRays: docs.placeholderText1
        property string sunShape: docs.placeholderText1

        property string execution: docs.placeholderText1
        property string cpuCores: docs.placeholderText1
        property string seed: docs.placeholderText1

        property string optimization: docs.placeholderText2
        property string opticalErrors: docs.placeholderText1
        property string pointFocusSystem: docs.placeholderText1

    }
}
