package it.achdjian.vase2.ui.graphics

import androidx.compose.Composable
import androidx.ui.core.*
import androidx.ui.layout.*
import androidx.ui.tooling.preview.Preview
import androidx.ui.unit.*
import kotlin.ranges.coerceIn

@Composable
fun Rotate(degree: Float, children: @Composable() () -> Unit) {
    Container(modifier = RotateLayout(degree = degree)) {

        Draw(children = children) { canvas, parentSize ->
            canvas.save()
            //canvas.translate(50f, 0f)
            canvas.rotate(degrees = degree)
            drawChildren()
            canvas.restore()
        }
    }
}

data class RotateLayout(val degree: Float) : LayoutModifier {
    val rad = degree * Math.PI / 180.0
    val sin = Math.sin(rad)
    val cos = Math.cos(rad)
    override fun DensityScope.modifyConstraints(
        constraints: Constraints
    ): Constraints {
        constraints.offset(
            horizontal = constraints.maxHeight * sin + constraints.maxWidth * cos,
            vertical = constraints.maxHeight * sin + constraints.maxWidth * cos
        )
        return constraints
    }

    override fun DensityScope.modifySize(
        constraints: Constraints,
        childSize: IntPxSize
    ) = IntPxSize(
        childSize.height.coerceIn(constraints.minWidth, constraints.maxWidth),
        childSize.width.coerceIn(constraints.minHeight, constraints.maxHeight)
    )

    override fun DensityScope.modifyPosition(
        childSize: IntPxSize,
        containerSize: IntPxSize
    ): IntPxPosition {
        return IntPxPosition(childSize.height * sin, 0.ipx)
    }
}

@Preview
@Composable
fun previewRotate() {
    Column {
        Row(modifier = LayoutAlign.CenterHorizontally) {
            Text("TOP")
        }
        Row() {
            Rotate(90f) {
                Text("test")
            }
        }
        Row(modifier = LayoutAlign.CenterHorizontally) {
            Rotate(45f) {
                Text("test")
            }
        }
        Row {
            Text("BOTTOM")
        }
    }
}