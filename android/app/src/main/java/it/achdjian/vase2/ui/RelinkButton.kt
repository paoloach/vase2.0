package it.achdjian.vase2.ui

import androidx.compose.Composable
import androidx.ui.graphics.vector.DrawVector
import androidx.ui.material.Button
import androidx.ui.res.vectorResource
import androidx.ui.tooling.preview.Preview
import it.achdjian.vase2.R
import it.achdjian.vase2.resumed

@Composable
fun RelinkButton() {
    Button(  onClick = { resumed.triggered=true }) {
        val connectImage = vectorResource(R.drawable.connect)
        DrawVector(vectorImage = connectImage)
    }
}

@Preview
@Composable
fun previewRelinkButton() {
        RelinkButton()
}