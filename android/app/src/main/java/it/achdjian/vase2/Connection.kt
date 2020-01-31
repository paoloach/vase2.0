package it.achdjian.vase2

import android.util.Log
import androidx.compose.Composable
import androidx.compose.Model
import androidx.compose.state
import androidx.ui.core.Text
import androidx.ui.core.TextField
import androidx.ui.input.KeyboardType
import androidx.ui.layout.LayoutWidth
import androidx.ui.layout.Padding
import androidx.ui.layout.Row
import androidx.ui.material.MaterialTheme
import androidx.ui.tooling.preview.Preview
import androidx.ui.unit.dp

@Model
data class IP(var octects: Array<Int> = arrayOf(255, 255, 255, 255))

@Composable
fun connectionTo() {
    Row() {
        Text("Connect to")

    }
}

var vaseIp = IP()

@Composable
fun Ip() {

    Padding(left = 16.dp, right = 16.dp) {
        Row {
            viewOctet(0)
            Text(text = ".", modifier = LayoutWidth.Max(5.dp))
            viewOctet(1)
            Text(text = ".", modifier = LayoutWidth.Max(5.dp))
            viewOctet(2)
            Text(text = ".", modifier = LayoutWidth.Max(5.dp))
            viewOctet(3)
        }
    }
}

@Composable
fun viewOctet(index:Int){
    val state  = state { vaseIp.octects[index]}
    TextField(value = state.value.toString(),
        modifier = LayoutWidth.Max(30.dp),
        onValueChange = {state.value=  octetValidation(it, vaseIp.octects[index])},
        keyboardType= KeyboardType.Number
    )
}

fun octetValidation(value: String, oldValue:Int):Int {
    Log.i("octetValidation", "value: $value");
    val octet = value.toIntOrNull()?.let { it } ?: oldValue
    return if (octet in 0..255)
        octet
    else
        oldValue
}


@Preview
@Composable
fun DefaultPreviewIp() {
    MaterialTheme {
        Ip()
    }
}