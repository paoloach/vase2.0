package it.achdjian.vase2.ui

import androidx.compose.Composable
import androidx.compose.unaryPlus
import androidx.ui.graphics.vector.DrawVector
import androidx.ui.material.Button
import androidx.ui.res.vectorResource
import androidx.ui.tooling.preview.Preview
import it.achdjian.vase2.R
import it.achdjian.vase2.about
import it.achdjian.vase2.update

@Composable
fun RefreshButton(){
    Button(  onClick = {update.updateAll() }) {
        val connectImage = +vectorResource(R.drawable.sync)
        DrawVector(vectorImage = connectImage)
    }
}

@Preview
@Composable
fun previewRefreshButton() {
    RefreshButton()
}