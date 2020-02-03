package it.achdjian.vase2.ui.graphics

import androidx.compose.Composable
import androidx.ui.core.dp
import androidx.ui.foundation.shape.border.Border
import androidx.ui.graphics.*
import androidx.ui.graphics.vector.*
import androidx.ui.layout.Center
import androidx.ui.layout.Container
import androidx.ui.tooling.preview.Preview


val  nodes = PathBuilder().moveTo(10f,10f).lineTo(50f, 50f).close().getNodes()

@Composable
fun Graphics(){

        DrawVector(
            defaultWidth = 100.dp,
            defaultHeight = 100.dp,
            tintColor = Color.Black,
            tintBlendMode = BlendMode.color
        ) { viewportWidth, viewportHeight ->
            Group(
                scaleX = 0.75f,
                scaleY = 0.75f,
                pivotX = (viewportWidth / 2),
                pivotY = (viewportHeight / 2)
            ) {
                Path(pathData = nodes, name = "test")
                Triangle()
            }

        }
}

@Composable
fun VectorScope.Triangle() {
    val length = 150.0f
    Path(
        fill = RadialGradient(
            listOf(
                Color(0xFF000080),
                Color(0xFF808000),
                Color(0xFF008080)
            ),
            centerX = length / 2.0f,
            centerY = length / 2.0f,
            radius = length / 2.0f,
            tileMode = TileMode.Repeated
        ),
        pathData = PathData {
            verticalLineTo(length)
            horizontalLineTo(length)
            close()
        }
    )
}

@Preview
@Composable
fun previewGraphics() {
    Center {
        val width = 120.dp
        val height = 120.dp
        Container(width = width, height = height) {
            Graphics()
        }
    }
}