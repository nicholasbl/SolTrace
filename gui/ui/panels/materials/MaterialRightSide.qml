import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import SolTraceProto

import QtQuick3D as Q3D
import QtQuick3D.Helpers as Q3DH

Item {
    id: right_side
    onHeightChanged: {
        console.log("LH CHANGED", height)
    }
    
    Q3D.View3D {
        id: preview_3d
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: width
        
        environment: Q3DH.ExtendedSceneEnvironment {
            antialiasingMode: Q3D.SceneEnvironment.MSAA
            antialiasingQuality: Q3D.SceneEnvironment.VeryHigh
            
            probeExposure: 1.2
            
            tonemapMode: Q3D.SceneEnvironment.TonemapModeAces
            
            depthOfFieldEnabled: true
            depthOfFieldBlurAmount: 4
            depthOfFieldFocusDistance: camera.position.length()
            //depthOfFieldFocusRange: Math.max(2, preview_3d.geom_bb_max_span * 10)
            
            depthOfFieldFocusRange: 100
            
            onDepthOfFieldFocusRangeChanged: {
                console.log("FOCUS", depthOfFieldFocusRange)
            }
            
            backgroundMode: Q3D.SceneEnvironment.SkyBox
            lightProbe: Q3D.Texture {
                textureData: Q3DH.ProceduralSkyTextureData {
                    sunColor: Qt.rgba(0, 0, 0, 0)
                    skyTopColor: Qt.rgba(0.2, 0.35, 0.6, 1.0)
                    skyHorizonColor: Qt.rgba(0.55, 0.65, 0.75, 1.0)
                    groundHorizonColor: Qt.rgba(0.55, 0.65, 0.75, 1.0)
                    groundBottomColor: Qt.rgba(0.45, 0.55, 0.65, 1.0)
                    
                }
                mappingMode: Q3D.Texture.LightProbe
            }
            
            Q3DH.InfiniteGrid {
                visible: true
                gridInterval: 1
            }
        }
        
        property var geom_bb : App.materials.group_edit.surface_geometry.bounding_box
        property vector3d geom_center : geom_bb.max.minus(geom_bb.min).times(.5).plus(geom_bb.min)
        
        property real geom_bb_max_span : {
            let span = geom_bb.max.minus(geom_bb.min)
            return Math.max(span.x, Math.max(span.y, span.z))
        }
        
        property real geom_distance : Math.max(geom_bb_max_span/2.0, 2)
        
        Q3D.Node {
            id: camera_host
            
            Q3D.PerspectiveCamera {
                id: camera
                position: Qt.vector3d(preview_3d.geom_distance, preview_3d.geom_distance, 0.0)
                //eulerRotation: Qt.vector3d(10, 45, 0)
                
                Component.onCompleted: {
                    lookAt(Qt.vector3d(0,0,0))
                }
                
                clipNear: .01
                
                clipFar : 100
                
                Behavior on position {
                    Vector3dAnimation {
                        duration: 2000
                    }
                }
                
                
            }
            
            NumberAnimation on eulerRotation.y {
                from: 0
                to: 360
                duration: 24000
                loops: Animation.Infinite
                running: true
            }
        }
        
        Q3D.DirectionalLight {
            eulerRotation.x: -45
            eulerRotation.y: 45
        }
        
        Q3D.Node {
            rotation: Q3D.Quaternion.fromEulerAngles(-90,0,0)
            Q3D.Model {
                id: surface_geometry
                geometry: App.materials.group_edit.surface_geometry
                
                materials : [
                    Q3D.PrincipledMaterial {
                        metalness: 1
                        roughness: 0
                        baseColor: "white"
                        cullMode: Q3D.Material.NoCulling
                    }
                ]
                
                
            }
        }
    }
}
