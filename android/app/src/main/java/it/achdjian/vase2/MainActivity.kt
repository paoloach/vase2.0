package it.achdjian.vase2

import android.content.Intent
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import androidx.compose.Composable
import androidx.compose.Model
import androidx.compose.ambient
import androidx.compose.state
import androidx.ui.core.ContextAmbient
import androidx.ui.core.Text
import androidx.ui.core.setContent
import androidx.ui.foundation.Border
import androidx.ui.foundation.shape.RectangleShape
import androidx.ui.graphics.Color
import androidx.ui.graphics.imageFromResource
import androidx.ui.layout.*
import androidx.ui.material.AppBarIcon
import androidx.ui.material.MaterialTheme
import androidx.ui.material.TopAppBar
import androidx.ui.tooling.preview.Preview
import androidx.ui.unit.dp
import it.achdjian.vase2.ui.ActualStatus
import it.achdjian.vase2.ui.ConnectionMessage
import it.achdjian.vase2.ui.PeriodPicker
import it.achdjian.vase2.ui.graphics.Graphics
import it.achdjian.vase2.ui.graphics.Samples
import it.achdjian.vase2.ui.graphics.test

private const val TAG = "Main"

@Model
class Status {
    var connected = false
    var actualSoil = 123
}


@Model
object resumed {
    var triggered = true
}

@Model
object update {
    var soil = false
    fun updateAll() {
        soil = true
    }
}

val status = Status()

class MainActivity : AppCompatActivity() {
    private var composed = false

    val showPreferences = {
        val intent = Intent(this, SettingsActivity::class.java)
        startActivity(intent)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        registerToFirebase()
        setContent {
            about()
            mainContent(showPreferences, resumed)
            composed = true

        }

    }

    override fun onResume() {
        super.onResume()
        resumed.triggered = true
    }

}

@Composable
fun mainContent(
    showPreferences: () -> Unit,
    resumed: resumed
) {
    if (resumed.triggered) {
        about()
        resumed.triggered = false
    }
    if (update.soil) {
        actualSoil()
        update.soil = false
    }
    mainView(showPreferences)

}

@Composable
fun mainView(
    showPreferences: () -> Unit
) {
    MaterialTheme {

        Column() {
            topBar(showPreferences = showPreferences)
            Padding(top = 16.dp) {

                ConnectionMessage()
            }
            if (status.connected) {
                ActualStatus()
                PeriodPicker()
                Row(){
                    Text(modifier = LayoutPadding(16.dp),text = "Element size: ")
                    Text(modifier = LayoutPadding(16.dp), text="${Samples.data.size}")
                }
                Row( Border(shape = RectangleShape, width = 1.dp, color = Color.Black)+LayoutWidth.Fill + LayoutHeight.Fill){
                    val data = Samples.data
                    val soilData = data.map { it.ts to it.soil }.toMap()
                    Graphics(soilData)
                }

            }
        }
    }
}


@Composable
fun topBar(showPreferences: () -> Unit) {
    val context = ambient(ContextAmbient)
    val navigationImage by lazy {
        {
            imageFromResource(
                context.resources,
                R.mipmap.baseline_settings_applications_black_18dp
            )
        }
    }
    Container(LayoutHeight.Max(30.dp)) {
        TopAppBar(
            title = { Text("Vase2..0") },
            actionData = listOf(navigationImage())
        ) { action ->
            AppBarIcon(action) {
                showPreferences()
                Log.i(TAG, "Click")
            }
        }
    }
}
//
//@Composable
//fun ButtonRegToken(navigationImage: () -> Image) {
//
//    Column() {
//
//        Padding(padding = 8.dp) {
//            FlowRow() {
//                Text(text = "Firebase token")
//                Text(text = token.token ?: "NOT AVAILABLE")
//            }
//        }
//    }
//}


@Preview
@Composable
fun DefaultPreview() {
    status.connected = true
    mainView({})
}
