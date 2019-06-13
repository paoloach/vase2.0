package it.achdjian.paolo.vase20

import android.content.SharedPreferences
import android.os.Handler
import android.os.HandlerThread
import android.os.Message
import it.achdjian.paolo.vase20.Rest.*
import java.util.*

/**
 * Created by Paolo Achdjian on 2/8/19.
 */
class RestRequest(
    sharedPreferences: SharedPreferences,
    private val mainActivity: MainActivity
) : HandlerThread("Vase2RestEngine"), Handler.Callback {
    private val handler: Handler
    private val restEngine = RestEngine(sharedPreferences)
    private val whoAreYou: WhoAreYou
    private val getStatus: GetStatus
    private val on: On
    private val off: Off

    init {
        start()
        handler = Handler(looper, this)
        whoAreYou = WhoAreYou(restEngine, handler)
        getStatus = GetStatus(restEngine, handler)
        on = On(restEngine)
        off = Off(restEngine)
    }

    fun requestWhoAreYou() =handler.post(whoAreYou)
    fun requestStatus() = handler.post(getStatus)
    fun setOn() = handler.post (on)
    fun setOff() = handler.post(off)
    fun setTime(dawnTime: Date, sunsetTime: Date) = handler.post(PostTime(restEngine, dawnTime, sunsetTime))
    fun updateDate(lastDate: Int, samples: Int) = handler.post(GetData(restEngine, handler, lastDate, samples))

    override fun handleMessage(message: Message?): Boolean {
        message?.let {
            val arg1= it.arg1
            val arg2= it.arg2
            when (it.what) {
                WHO_ARE_YOU_PRESENT -> mainActivity.runOnUiThread { mainActivity.connected() }
                WHO_ARE_YOU_NOT_PRESENT -> mainActivity.runOnUiThread { mainActivity.disconnected() }
                LIGHT_ON -> mainActivity.runOnUiThread { mainActivity.lightOn() }
                LIGHT_OFF -> mainActivity.runOnUiThread { mainActivity.lightOff() }
                DAWN_TIME -> mainActivity.runOnUiThread { mainActivity.setDawn(arg1, arg2) }
                SUNSET_TIME -> mainActivity.runOnUiThread { mainActivity.setSunset(arg1, arg2) }
                SENSOR_DATA -> {
                    if (it.obj is List<*>) {
                        val data = it.obj as List<SensorData>
                        mainActivity.runOnUiThread { mainActivity.updateData(data.asReversed()) }
                    }
                }
            }
        }
        return true
    }
}