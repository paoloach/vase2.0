package it.achdjian.vase2.ui.graphics

import android.util.Log
import androidx.compose.Composable
import androidx.ui.core.*
import androidx.ui.foundation.Border
import androidx.ui.foundation.shape.RectangleShape
import androidx.ui.graphics.BlendMode
import androidx.ui.graphics.Color
import androidx.ui.graphics.SolidColor
import androidx.ui.graphics.vector.DrawVector
import androidx.ui.graphics.vector.Group
import androidx.ui.graphics.vector.Path
import androidx.ui.graphics.vector.PathBuilder
import androidx.ui.layout.*
import androidx.ui.tooling.preview.Preview
import androidx.ui.unit.DensityScope
import androidx.ui.unit.dp
import com.google.gson.GsonBuilder
import com.google.gson.reflect.TypeToken
import java.nio.file.Files
import java.nio.file.Paths

const val TAG = "Graphics"

@Composable
fun Graphics(data: Map<Long, Int>) {

    WithConstraints {
        val entries = data.entries
        val max = data.values.max() ?: 4096
        val min = data.values.min() ?: 0
        val yRange = (max - min).toFloat()
        val xRange = entries.last().key - entries.first().key + 1
        Log.i(TAG, "nodes: ${data.size}")
        Log.i(TAG, "min: $min, max: $max")
        Log.i(TAG, "xRange: $xRange, yRange = $yRange")
        if (data.isNotEmpty()) {
            drawLines(data, it, yRange, xRange)
            drawGraph(data, it, yRange,xRange)
        }
    }
}

@Composable
fun drawLines(
    data: Map<Long, Int>,
    constraints: Constraints,
    yRange: Float,
    xRange: Long
){
    val dpConstraints = DensityScope(ambientDensity()).DpConstraints(constraints)
    Container(width = dpConstraints.maxWidth, height = dpConstraints.maxHeight) {
        val numberToSplit=10
        val xDelta = xRange / numberToSplit
        for (i in 1 .. numberToSplit) {
            val x = xDelta * i

            DrawVector(
                defaultHeight = dpConstraints.maxHeight,
                defaultWidth = dpConstraints.maxWidth,
                viewportWidth = xRange.toFloat(),
                viewportHeight = 2*yRange,
                tintColor = Color.Black,
                tintBlendMode = BlendMode.srcATop
            ) { _, _ ->
                Group(translationY = yRange / 2) {

                    Path(
                        stroke = SolidColor(Color.Gray),
                        strokeLineWidth = 0f,
                        pathData = PathBuilder().moveTo(x.toFloat(),0f).verticalLineTo(yRange).getNodes(),
                        name = "test"
                    )
                }
            }
        }

    }
}

@Composable
fun drawGraph(
    data: Map<Long, Int>,
    constraints: Constraints,
    yRange: Float,
    xRange: Long
){
    val entries = data.entries
    val max = data.values.max() ?: 4096
    val xStart = entries.first().key
    val pathBuilder = PathBuilder().moveTo(0f, 0f)
    data.forEach { (t, v) -> pathBuilder.lineTo((t - xStart).toFloat(), (max - v).toFloat()) }

    val dpConstraints = DensityScope(ambientDensity()).DpConstraints(constraints)

    Container(width = dpConstraints.maxWidth, height = dpConstraints.maxHeight) {
        DrawVector(
            defaultHeight = dpConstraints.maxHeight,
            defaultWidth = dpConstraints.maxWidth,
            viewportWidth = xRange.toFloat(),
            viewportHeight = 2 * yRange,
            tintColor = Color.Black,
            tintBlendMode = BlendMode.srcATop
        ) { _, _ ->
            Group(translationY = yRange / 2) {

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


@Preview
@Composable
fun previevGraphics() {
    test()
}

@Composable
fun test() {
    val dataFile = "workspace/vaso2/android/app/dataSample.json"
    val turnsType = object : TypeToken<List<Sample>>() {}.type
    val json = String( Files.readAllBytes(Paths.get(dataFile)))
    val gson = GsonBuilder()
        .setLenient()
        .create()
    val samples = gson.fromJson<List<Sample>>(json, turnsType)
    val data = samples.filter { it.soil < 3000 }.map { it.ts to it.soil }.toMap()
    Center {
        Column {
            Row(modifier = LayoutHeight.Min(100.dp)+ LayoutPadding()) {
                Rotate(90f){
                    Text("test")
                }
            }
            Row(modifier = Border(shape = RectangleShape, width = 1.dp, color = Color.Black)) {
                Graphics(data)
            }
        }
    }
}