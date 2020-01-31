package it.achdjian.vase2.ui

import androidx.compose.Composable
import androidx.ui.core.Text
import androidx.ui.core.dp
import androidx.ui.layout.*
import androidx.ui.tooling.preview.Preview
import it.achdjian.vase2.status

@Composable
fun ActualStatus() {
    FlexRow(mainAxisAlignment = MainAxisAlignment.SpaceEvenly) {
        inflexible {
            ActualSoil()
        }
    }
}

@Composable
fun ActualSoil() {
    Row() {
        Padding(right = 16.dp) {
            Text(text = "Soil")
        }
        Text(text = status.actualSoil.toString())
    }
}

@Preview
@Composable
fun PreviewActualStatus() {
    ActualStatus()
}