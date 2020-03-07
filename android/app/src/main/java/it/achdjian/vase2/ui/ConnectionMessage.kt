package it.achdjian.vase2.ui

import androidx.compose.Composable
import androidx.ui.layout.Column
import androidx.ui.tooling.preview.Preview
import it.achdjian.vase2.status

@Composable
fun ConnectionMessage() {

    if (status.connected)
        ConnectMessage()
    else
        DisconnectMessage()
}

@Preview
@Composable
fun previewConnectionMessage() {
        Column{
                status.connected = true
                ConnectionMessage()
                status.connected = false
                ConnectionMessage()
        }
}