package it.achdjian.vase2.ui

import androidx.compose.Composable
import androidx.ui.core.Text
import androidx.ui.layout.*
import androidx.ui.tooling.preview.Preview

@Composable
fun ConnectMessage() {
    FlexRow(mainAxisAlignment=MainAxisAlignment.SpaceBetween, crossAxisAlignment = CrossAxisAlignment.Center) {
        inflexible {
            Text(text = "Connected")
        }
        inflexible {
            Container() {
                RefreshButton()
            }
        }
    }
}

@Preview
@Composable
fun previewCconnectMessage() {
    ConnectMessage()
}