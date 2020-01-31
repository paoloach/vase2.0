package it.achdjian.vase2

import android.util.Log
import androidx.compose.Composable
import androidx.compose.ambient
import androidx.compose.unaryPlus
import androidx.preference.PreferenceManager
import androidx.ui.core.ContextAmbient
import kotlinx.coroutines.*
import okhttp3.ResponseBody
import retrofit2.Call
import retrofit2.Retrofit
import retrofit2.converter.gson.GsonConverterFactory
import retrofit2.http.GET


private const val TAG = "VaseService"

interface VaseService {
    @GET("about")
    fun about(): Call<ResponseBody>
    @GET("soil")
    fun soil(): Call<ResponseBody>
}

var vaseService: VaseService? = null

val handler =
    CoroutineExceptionHandler { _, throwable -> Log.e(TAG, "error: ", throwable) }

@Composable
fun about() {
    val context = +ambient(ContextAmbient)
    val sharedPreferences = PreferenceManager.getDefaultSharedPreferences(context)
    val address = sharedPreferences.getString("vase_address", "192.168.1.160")
    val url = "http://$address"

    GlobalScope.launch(handler) {
        withContext(Dispatchers.Main) {

            Log.i(TAG, "Check service at $url")
            vaseService = checkService(url)
            actualSoil()
            status.connected = vaseService != null
        }

    }
}

@Composable
suspend fun actualSoil() {
    vaseService?.let {service->
        withContext(Dispatchers.IO) {
            val response = service.soil().execute()
            if (response.isSuccessful){
                val responseBody = response.body()?.string()
                Log.i(TAG,"soil response: $responseBody")
                responseBody?.let {
                    withContext(Dispatchers.Main) { 
                        status.actualSoil = it.toInt()
                    }
                }
            } else {
                vaseService=null
            }
        }
    }
}

suspend fun checkService(url: String): VaseService? {
    return withContext(Dispatchers.IO) {
        Log.i(TAG, "Check service at $url")
        val retrofit = Retrofit.Builder()
            .baseUrl(url)
            .addConverterFactory(GsonConverterFactory.create())
            .build()
        val service = retrofit.create(VaseService::class.java)
        val response = service.about().execute()
        if (response.isSuccessful) {
            val responseBody = response.body()?.string()
            Log.i(TAG, "About response: $responseBody ")
            val rightServer = responseBody?.let { it == "Vase 2.0\n" } ?: false
            if (rightServer) {
                Log.i(TAG, "Right server")
                service
            }else
                null
        } else {
            Log.i(TAG, "response unsuccessful")
            null
        }
    }
}