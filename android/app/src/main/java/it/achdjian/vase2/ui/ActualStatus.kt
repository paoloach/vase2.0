package it.achdjian.vase2.ui

import androidx.compose.Composable
import androidx.ui.core.Text
import androidx.ui.layout.Arrangement
import androidx.ui.layout.LayoutWidth
import androidx.ui.layout.Padding
import androidx.ui.layout.Row
import androidx.ui.tooling.preview.Preview
import androidx.ui.unit.dp
import it.achdjian.vase2.status

@Composable
fun ActualStatus() {
    Row(modifier = LayoutWidth.Fill,arrangement = Arrangement.SpaceEvenly) {
            ActualSoil()
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