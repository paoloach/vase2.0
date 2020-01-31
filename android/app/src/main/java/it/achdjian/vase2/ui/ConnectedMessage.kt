package it.achdjian.vase2.ui

import androidx.compose.Composable
import androidx.ui.core.Text
import androidx.ui.layout.*
import androidx.ui.tooling.preview.Preview

@Composable
fun ConnectMessage() {
    Row(arrangement = Arrangement.SpaceBetween) {
        Text(text = "Connected")
        Container() {
            RefreshButton()
        }
    }
}

@Preview
@Composable
fun previewCconnectMessage() {
    ConnectMessage()
}