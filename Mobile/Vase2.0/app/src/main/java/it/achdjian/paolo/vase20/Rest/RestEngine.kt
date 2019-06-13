package it.achdjian.paolo.vase20.Rest

import android.content.SharedPreferences
import android.util.Log
import it.achdjian.paolo.vase20.MainActivity.Companion.IP_KEY
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody
import java.util.concurrent.TimeUnit

/**
 * Created by Paolo Achdjian on 2/8/19.
 */
class RestEngine(private val sharedPreferences: SharedPreferences) {
    companion object {
        private const val TAG="RestEngine"
        private val TEXT_PLAIN = okhttp3.MediaType.parse("text/plain; charset=utf-8")
    }

    private val client: OkHttpClient = OkHttpClient
        .Builder()
        .connectTimeout(10, TimeUnit.SECONDS)
        .writeTimeout(10, TimeUnit.SECONDS)
        .readTimeout(10, TimeUnit.SECONDS)
        .build()


    fun postMethod(path: String, body:String="" ) {
        val address = sharedPreferences.getString(IP_KEY, "192.168.1.119")
        val url = "http://$address$path"
        Log.i(TAG, url)
        if (body.isNotBlank()){
            Log.i(TAG, "body: $body")
        }
        try {
            val request = Request
                .Builder()
                .url(url)
                .post(
                    RequestBody
                        .create(TEXT_PLAIN, body)
                )
                .build()
            client.newCall(request).execute()
        } catch (e: Exception){
            Log.i(TAG, "Error calling $url", e)
        }
    }


    fun getMethod(path: String ):String{
        val address = sharedPreferences.getString(IP_KEY, "192.168.1.119")
        val url = "http://$address$path"
        Log.i(TAG, url)
        val request = Request.Builder().url(url).get().build()
        try {
            val response = client.newCall(request).execute()
            Log.i(TAG,"response code: " + response.code())
            if (response.code() == 200) {
                return response.body()?.string() ?: ""
            }
            if (response.code() == 204) {
                return ""
            }
            Log.e(TAG, "Error code: " + response.code() + ": msg: " + response.message())
        } catch (ignored: Exception) {
            Log.e(TAG, "error", ignored)
        }
        return ""
    }
}