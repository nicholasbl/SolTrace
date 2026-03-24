import QtQuick

Item {
    id: root
    width: 320
    height: 220

    property real reflectance: 0.85   // ρ
    property real transmittance: 0.10 // τ
    property real nFront: 1.0
    property real nBack: 1.5
    property real slopeErrorMrad: 8.0
    property real specularityErrorMrad: 3.0

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            const ctx = getContext("2d")
            ctx.reset()

            const w = width
            const h = height
            const cx = w * 0.52
            const cy = h * 0.52

            const surfaceHalf = w * 0.34
            const rayLen = 78
            const incidentAngleDeg = 35
            const incidentAngle = incidentAngleDeg * Math.PI / 180

            const slopeErrDeg = root.slopeErrorMrad * 0.0572958
            const specErrDeg = root.specularityErrorMrad * 0.0572958
            const slopeErr = slopeErrDeg * Math.PI / 180
            const specErr = specErrDeg * Math.PI / 180

            function line(x1, y1, x2, y2, color, width, dash) {
                ctx.save()
                ctx.beginPath()
                ctx.strokeStyle = color
                ctx.lineWidth = width || 1
                ctx.setLineDash(dash || [])
                ctx.moveTo(x1, y1)
                ctx.lineTo(x2, y2)
                ctx.stroke()
                ctx.restore()
            }

            function arrow(x1, y1, x2, y2, color, width) {
                const head = 8
                const ang = Math.atan2(y2 - y1, x2 - x1)

                line(x1, y1, x2, y2, color, width || 2)

                ctx.save()
                ctx.beginPath()
                ctx.fillStyle = color
                ctx.moveTo(x2, y2)
                ctx.lineTo(
                    x2 - head * Math.cos(ang - Math.PI / 6),
                    y2 - head * Math.sin(ang - Math.PI / 6)
                )
                ctx.lineTo(
                    x2 - head * Math.cos(ang + Math.PI / 6),
                    y2 - head * Math.sin(ang + Math.PI / 6)
                )
                ctx.closePath()
                ctx.fill()
                ctx.restore()
            }

            function text(txt, x, y, color, font, align) {
                ctx.save()
                ctx.fillStyle = color || "#d8dee9"
                ctx.font = font || "12px sans-serif"
                ctx.textAlign = align || "left"
                ctx.textBaseline = "middle"
                ctx.fillText(txt, x, y)
                ctx.restore()
            }

            function polarLine(x, y, len, ang, color, width, dash) {
                const x2 = x + len * Math.cos(ang)
                const y2 = y + len * Math.sin(ang)
                line(x, y, x2, y2, color, width, dash)
                return { x: x2, y: y2 }
            }

            function polarArrow(x, y, len, ang, color, width) {
                const x2 = x + len * Math.cos(ang)
                const y2 = y + len * Math.sin(ang)
                arrow(x, y, x2, y2, color, width)
                return { x: x2, y: y2 }
            }

            function arcFan(x, y, r, a0, a1, color, width) {
                ctx.save()
                ctx.beginPath()
                ctx.strokeStyle = color
                ctx.lineWidth = width || 1
                ctx.arc(x, y, r, a0, a1, false)
                ctx.stroke()
                ctx.restore()
            }

            // front/back media tint
            ctx.fillStyle = "rgba(120, 160, 220, 0.08)"
            ctx.fillRect(0, 0, w, cy)
            ctx.fillStyle = "rgba(120, 220, 160, 0.08)"
            ctx.fillRect(0, cy, w, h - cy)

            // surface
            line(cx - surfaceHalf, cy, cx + surfaceHalf, cy, "#e5e9f0", 3)

            // hit point
            ctx.beginPath()
            ctx.fillStyle = "#ffffff"
            ctx.arc(cx, cy, 3, 0, 2 * Math.PI)
            ctx.fill()

            // ideal normal
            polarLine(cx, cy, 62, -Math.PI / 2, "#aaaaaa", 1.5, [5, 4])
            polarLine(cx, cy, 62,  Math.PI / 2, "#aaaaaa", 1.5, [5, 4])
            text("ideal normal", cx + 10, cy - 52, "#aaaaaa", "11px sans-serif")

            // slope error normals
            polarLine(cx, cy, 48, -Math.PI / 2 - slopeErr, "#ffcc66", 1, [2, 3])
            polarLine(cx, cy, 48, -Math.PI / 2 + slopeErr, "#ffcc66", 1, [2, 3])
            text("σ_slope", cx - 58, cy - 58, "#ffcc66", "12px sans-serif")

            // incident ray
            const inStartX = cx - rayLen * Math.cos(incidentAngle)
            const inStartY = cy - rayLen * Math.sin(incidentAngle)
            arrow(inStartX, inStartY, cx, cy, "#f6ad55", 2.5)
            text("incident ray", inStartX - 8, inStartY - 8, "#f6ad55", "12px sans-serif", "left")

            // reflected ray (ideal)
            const reflectedAngle = -incidentAngle
            const refl = polarArrow(cx, cy, rayLen, reflectedAngle, "#63b3ed", 2.5)
            text("ρ", cx + 38, cy - 36, "#63b3ed", "bold 14px sans-serif")
            text(root.reflectance.toFixed(2), refl.x + 8, refl.y - 2, "#63b3ed", "11px sans-serif")

            // transmitted ray
            // simplified visual only, not exact Snell calculation
            const transmitAngle = incidentAngle * 0.72
            const trans = polarArrow(cx, cy, rayLen, transmitAngle, "#68d391", 2.5)
            text("τ", cx + 34, cy + 34, "#68d391", "bold 14px sans-serif")
            text(root.transmittance.toFixed(2), trans.x + 8, trans.y + 2, "#68d391", "11px sans-serif")

            // specularity fan around reflected direction
            arcFan(cx, cy, 34, reflectedAngle - specErr, reflectedAngle + specErr, "#f687b3", 1.5)
            polarLine(cx, cy, 40, reflectedAngle - specErr, "#f687b3", 1, [2, 2])
            polarLine(cx, cy, 40, reflectedAngle + specErr, "#f687b3", 1, [2, 2])
            text("σ_spec", cx + 8, cy - 18, "#f687b3", "12px sans-serif")

            // labels for indices of refraction
            text("n front = " + root.nFront.toFixed(2), 18, cy - 26, "#cbd5e0", "12px sans-serif")
            text("n back = " + root.nBack.toFixed(2), 18, cy + 26, "#cbd5e0", "12px sans-serif")

            // surface label
            text("surface", cx + surfaceHalf - 12, cy - 10, "#e5e9f0", "11px sans-serif", "right")
        }
    }
}
