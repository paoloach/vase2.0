package it.achdjian.vase2

import android.util.Log
import androidx.compose.Composable
import androidx.compose.ambient
import androidx.preference.PreferenceManager
import androidx.ui.core.ContextAmbient
import com.google.gson.Gson
import com.google.gson.GsonBuilder
import com.google.gson.reflect.TypeToken
import it.achdjian.vase2.ui.graphics.Sample
import it.achdjian.vase2.ui.graphics.Samples
import kotlinx.coroutines.*
import okhttp3.ResponseBody
import retrofit2.Call
import retrofit2.Retrofit
import retrofit2.converter.gson.GsonConverterFactory
import retrofit2.http.GET
import retrofit2.http.Query
import java.time.LocalDateTime
import java.time.ZoneOffset


private const val TAG = "VaseService"



interface VaseService {
    @GET("about")
    fun about(): Call<ResponseBody>

    @GET("soil")
    fun soil(): Call<ResponseBody>

    @GET("data")
    fun data(@Query("start") start: Int, @Query("end") end: Int): Call<List<Sample>>
}

var vaseService: VaseService? = null

val handler =
    CoroutineExceptionHandler { _, throwable -> Log.e(TAG, "error: ", throwable) }

@Composable
fun about() {
    val context = ambient(ContextAmbient)
    val sharedPreferences = PreferenceManager.getDefaultSharedPreferences(context)
    val address = sharedPreferences.getString("vase_address", "192.168.1.160")
    val url = "http://$address"

    GlobalScope.launch(handler) {
        withContext(Dispatchers.Main) {

            Log.i(TAG, "Check service at $url")
            vaseService = checkService(url)
            update.soil = true
            status.connected = vaseService != null
        }

    }
}

@Composable
fun actualSoil() {
    vaseService?.let { service ->
        GlobalScope.launch(handler) {
            withContext(Dispatchers.IO) {
                val response = service.soil().execute()
                if (response.isSuccessful) {
                    val responseBody = response.body()?.string()
                    Log.i(TAG, "soil response: $responseBody")
                    responseBody?.let {
                        withContext(Dispatchers.Main) {
                            if (it.startsWith("soilMoisture:")) {
                                status.actualSoil =
                                    it.substring(14).filter { c -> c.isDigit() }.toInt()
                            }
                        }
                    }
                } else {
                    vaseService = null
                }
            }
        }
    }
}

fun getData(from: LocalDateTime, to: LocalDateTime) {
    vaseService?.let { service ->
        GlobalScope.launch(handler) {
            withContext(Dispatchers.Main) {
                Samples.data =withContext<List<Sample>>(Dispatchers.IO) {
                    val start = from.toEpochSecond(ZoneOffset.UTC).toInt()
                    val end = to.toEpochSecond(ZoneOffset.UTC).toInt()
                    Log.i(TAG, "Call data from $start to $end")
                    val response = service.data(start, end).execute()
                    if (response.isSuccessful) {
                        response.body() ?: emptyList()
                    } else
                        emptyList()
                }
                Log.i(TAG, "get data: ${Samples.data.size} samples")

            }
        }
    }

}

val samplesType = object : TypeToken<List<Sample>>() {}.type

fun transformToList(body: ResponseBody): List<Sample> {
    val gson = Gson()
    val samples : List<Sample> = gson.fromJson(body.charStream(), samplesType)
    return samples
}


suspend fun checkService(url: String): VaseService? {
    return withContext(Dispatchers.IO) {
        Log.i(TAG, "Check service at $url")
        val gson = GsonBuilder()
            .setLenient()
            .create()
        val retrofit = Retrofit.Builder()
            .baseUrl(url)
            .addConverterFactory(GsonConverterFactory.create(gson))
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
            } else
                null
        } else {
            Log.i(TAG, "response unsuccessful")
            null
        }
    }
}