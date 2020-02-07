package it.achdjian.vase2.ui.graphics

import androidx.compose.Composable
import androidx.ui.core.WithConstraints
import androidx.ui.core.ambientDensity
import androidx.ui.foundation.Border
import androidx.ui.foundation.shape.RectangleShape
import androidx.ui.graphics.BlendMode
import androidx.ui.graphics.Color
import androidx.ui.graphics.SolidColor
import androidx.ui.graphics.vector.DrawVector
import androidx.ui.graphics.vector.Path
import androidx.ui.graphics.vector.PathBuilder
import androidx.ui.layout.*
import androidx.ui.tooling.preview.Preview
import androidx.ui.unit.DensityScope
import androidx.ui.unit.dp


val nodes = PathBuilder()
    .lineTo(0f, 150f)
    .lineToRelative(110f, 0f)
    .lineToRelative(0f, -140f)
    .getNodes()
val nodes2 = PathBuilder().verticalLineTo(150f).horizontalLineTo(10f).close().getNodes()

@Composable
fun Graphics(data: Map<Long, Int>) {

    WithConstraints {
        if (!data.isEmpty()) {
            val entries = data.entries
            val max = data.values.max() ?: 4096
            val min = data.values.min() ?: 0
            val yRange = (max - min).toFloat()
            val xRange = entries.last().key - entries.first().key + 1
            val xStart = entries.first().key
            val pathBuilder = PathBuilder().moveTo(0f, 0f)
            data.forEach { t, v -> pathBuilder.lineTo((t - xStart).toFloat(), (max - v).toFloat()) }

            DensityScope(ambientDensity()).run { it.maxWidth.toDp() }
            val dpConstraints = DensityScope(ambientDensity()).DpConstraints(it)
            Container(width = dpConstraints.maxWidth, height = dpConstraints.maxHeight) {
                DrawVector(
                    defaultHeight = dpConstraints.maxHeight,
                    defaultWidth = dpConstraints.maxWidth,
                    viewportWidth = xRange.toFloat(),
                    viewportHeight = yRange + yRange / 10,
                    tintColor = Color.Black,
                    tintBlendMode = BlendMode.color
                ) { w, h ->

                    Path(
                        stroke = SolidColor(Color.Red),
                        strokeLineWidth = 0f,
                        pathData = pathBuilder.getNodes(),
                        name = "test"
                    )
                }
            }
        }
    }
}


@Preview
@Composable
fun previevGraphics() {
    test()
}

@Composable
fun test() {
    val testData = mapOf<Long, Int>(1580852887L to 1895, 1580860291L to 1893, 1580861074L to 1892)
    Center {
        Row(modifier = Border(shape = RectangleShape, width = 1.dp, color = Color.Black)) {
            Graphics(testData)
        }
    }
}