package it.achdjian.vase2.ui

import androidx.compose.Composable
import androidx.ui.core.Text
import androidx.ui.layout.*
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
        Text(modifier = LayoutPadding(right = 16.dp), text = "Soil")
        Text(text = status.actualSoil.toString())
    }
}

@Preview
@Composable
fun PreviewActualStatus() {
    ActualStatus()
}