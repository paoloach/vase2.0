package it.achdjian.vase2.ui

import androidx.compose.Composable
import androidx.ui.graphics.vector.DrawVector
import androidx.ui.material.Button
import androidx.ui.res.vectorResource
import androidx.ui.tooling.preview.Preview
import it.achdjian.vase2.R

@Composable
fun RefreshButton(){
    Button(  onClick = { }) {
        val connectImage = vectorResource(R.drawable.sync)
        DrawVector(vectorImage = connectImage)
    }
}

@Preview
@Composable
fun previewRefreshButton() {
    RefreshButton()
}