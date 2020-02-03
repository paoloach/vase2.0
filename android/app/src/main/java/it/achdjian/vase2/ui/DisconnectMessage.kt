package it.achdjian.vase2.ui

import androidx.compose.Composable
import androidx.ui.core.Text
import androidx.ui.layout.*
import androidx.ui.tooling.preview.Preview

@Composable
fun DisconnectMessage() {
    Row(arrangement = Arrangement.SpaceBetween, modifier = LayoutWidth.Fill) {
        Text(text = "Disconnected", modifier = LayoutGravity.Center)
        Container(modifier = LayoutGravity.Center) {
            RelinkButton()
        }
    }
}

@Preview
@Composable
fun previewDisconnectMessage() {
    DisconnectMessage()
}