package it.achdjian.vase2

import android.util.Log
import androidx.compose.Composable
import androidx.compose.Model
import androidx.compose.state
import androidx.compose.unaryPlus
import androidx.ui.core.*
import androidx.ui.input.KeyboardType
import androidx.ui.layout.*
import androidx.ui.material.MaterialTheme
import androidx.ui.tooling.preview.Preview

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
            Text(text = ".", modifier = Width(5.dp))
            viewOctet(1)
            Text(text = ".", modifier = Width(5.dp))
            viewOctet(2)
            Text(text = ".", modifier = Width(5.dp))
            viewOctet(3)
        }
    }
}

@Composable
fun viewOctet(index:Int){
    val state  = + state { vaseIp.octects[index]}
    TextField(value = state.value.toString(),
        modifier = Width(30.dp),
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