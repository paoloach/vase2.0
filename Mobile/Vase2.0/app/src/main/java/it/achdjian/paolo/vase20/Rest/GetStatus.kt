package it.achdjian.paolo.vase20.Rest

import android.os.Handler
import android.util.Log
import java.lang.Exception

/**
 * Created by Paolo Achdjian on 2/10/19.
 */
class GetStatus(val restEngine: RestEngine, val handler: Handler) : Runnable {
    companion object {
        const val TAG="WhoAreYou"
    }
    override fun run() {
        Log.i(TAG,"get status")
        val response = restEngine.getMethod("/status")
        Log.i(TAG,"response: " + response)
        val lines = response.lines()
        if (lines[0].startsWith("light ")){
            if (lines[0].contains("on")){
                handler.sendMessage(handler.obtainMessage(LIGHT_ON))
            }
            if (lines[0].contains("off")){
                handler.sendMessage(handler.obtainMessage(LIGHT_OFF))
            }
        }
        if (lines[1].startsWith("start: ")){
            try {
                val startHour = lines[1].substring(7, 9).toInt();
                val startMinute = lines[1].substring(10, 12).toInt()
                Log.i(TAG,"dawn time: $startHour:$startMinute")
                handler.sendMessage(handler.obtainMessage(DAWN_TIME, startHour, startMinute))
            } catch (e:Exception){
                Log.i(TAG,"Unable to read dawn time: ${e.message}")
            }
        }
        if (lines[2].startsWith("end: ")){
            try {
                val endHour = lines[2].substring(5, 7).toInt();
                val endMinute = lines[2].substring(8, 10).toInt()
                Log.i(TAG,"sunset time: $endHour:$endMinute")
                handler.sendMessage(handler.obtainMessage(SUNSET_TIME, endHour, endMinute))
            } catch (e:Exception){
                Log.i(TAG,"Unable to read dawn time: ${e.message}")
            }
        }
    }
}