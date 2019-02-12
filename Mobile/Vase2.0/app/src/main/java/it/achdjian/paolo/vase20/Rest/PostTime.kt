package it.achdjian.paolo.vase20.Rest

import android.annotation.SuppressLint
import java.text.SimpleDateFormat
import java.util.*

/**
 * Created by Paolo Achdjian on 2/11/19.
 */
class PostTime(val restEngine: RestEngine, val dawn: Date, val sunset: Date): Runnable {
    companion object {
        @SuppressLint("SimpleDateFormat")
        val DATE_FORMAT = SimpleDateFormat("HH:mm")

    }

    override fun run() {
        val dawnString = DATE_FORMAT.format(dawn)
        restEngine.postMethod("/onTime", dawnString)
        val sunset = DATE_FORMAT.format(sunset)
        restEngine.postMethod("/offTime", sunset)
    }
}