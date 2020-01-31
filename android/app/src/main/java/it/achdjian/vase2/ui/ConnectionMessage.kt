package it.achdjian.vase2.ui

import androidx.compose.Composable
import androidx.ui.core.Alignment
import androidx.ui.core.Text
import androidx.ui.core.dp
import androidx.ui.layout.*
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
        FlexColumn(crossAxisAlignment= CrossAxisAlignment.Stretch) {
            inflexible {
                status.connected = true
                ConnectionMessage()
            }
            inflexible {
                status.connected = false
                ConnectionMessage()
            }
        }
}