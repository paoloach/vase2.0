package it.achdjian.vase2.ui

import androidx.compose.Composable
import androidx.ui.core.FirstBaseline
import androidx.ui.core.Text
import androidx.ui.core.dp
import androidx.ui.layout.*
import androidx.ui.tooling.preview.Preview

@Composable
fun DisconnectMessage() {
    Row(arrangement = Arrangement.SpaceBetween) {
        Text(text = "Disconnected", modifier = Gravity.Center)
        Container(modifier = Gravity.Center) {
            RelinkButton()
        }
    }
}

@Preview
@Composable
fun previewDisconnectMessage() {
    DisconnectMessage()
}