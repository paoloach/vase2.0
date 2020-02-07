package it.achdjian.vase2.ui.graphics

import androidx.compose.Model
import androidx.compose.state

data class Sample(var ts: Long, var temperature: Int, var huidity:Int, var soil:Int)


@Model
object Samples {
    var data = listOf<Sample>()
}