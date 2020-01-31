package it.achdjian.vase2

import android.util.Log
import androidx.compose.Model
import com.google.android.gms.tasks.OnCompleteListener
import com.google.firebase.iid.FirebaseInstanceId

private const val TAG = "FirebaseToken";

@Model
class FirebaseToken(var token:String? =null)

var token = FirebaseToken()

fun registerToFirebase() {
    FirebaseInstanceId.getInstance().instanceId
        .addOnCompleteListener(OnCompleteListener { task ->
            if (!task.isSuccessful) {
                Log.w(TAG, "getInstanceId failed", task.exception)
                return@OnCompleteListener

            }

            // Get new Instance ID token
            token.token = task.result?.token
            token.token?.let { Log.d(TAG, token.token) }

        })
}