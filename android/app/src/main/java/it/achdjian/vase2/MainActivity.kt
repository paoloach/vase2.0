package it.achdjian.vase2

import android.content.ContentValues.TAG
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import androidx.compose.Composable
import androidx.ui.core.Text
import androidx.ui.core.dp
import androidx.ui.core.setContent
import androidx.ui.layout.*
import androidx.ui.material.Button
import androidx.ui.material.MaterialTheme
import androidx.ui.tooling.preview.Preview
import com.google.android.gms.tasks.OnCompleteListener
import com.google.firebase.iid.FirebaseInstanceId

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        registerToFirebase()
        setContent {
            MaterialTheme {
                Greeting()
                ButtonRegToken()
            }
        }
    }
}

@Composable
fun ButtonRegToken() {
    Column() {

        Padding(padding = 8.dp) {
            FlowRow() {
                Text(text = "Firebase token")
                Text(text = token.token ?: "NOT AVAILABLE")
            }
        }
    }
}

@Composable
fun Connection() {
    Padding(padding = 8.dp) {
        FlowRow() {

            Text("Vase IP:")
            Ip()
        }
    }
    Button(text = "Connect")

}

@Composable
fun Greeting() {
    Text(text = "Vase 2.0")
}

@Preview
@Composable
fun DefaultPreview() {
    MaterialTheme {
        Column(arrangement = Arrangement.Begin) {
            Greeting()
            ButtonRegToken()
            Connection()
        }
    }
}
