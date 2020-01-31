package it.achdjian.vase2.ui

import androidx.compose.Composable
import androidx.ui.layout.CrossAxisAlignment
import androidx.ui.layout.FlexColumn
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