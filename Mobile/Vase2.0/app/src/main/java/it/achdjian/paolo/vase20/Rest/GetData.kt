package it.achdjian.paolo.vase20.Rest

import android.os.Handler
import android.util.Log
import java.util.*

/**
 * Created by Paolo Achdjian on 2/12/19.
 */
class SensorConvertError(msg: String) : RuntimeException(msg)

class GetData(
    private val restEngine: RestEngine,
    private val handler: Handler,
    private val toSkip: Int,
    private val samples: Int=50
) : Runnable {
    companion object {
        const val TAG = "GetData"
        fun infoLog(msg: String) = Log.i(TAG, msg)
    }

    override fun run() {
        Log.i(GetStatus.TAG, "get status")
        val response = restEngine.getMethod("/data?size=$samples&start=$toSkip")
        val lines = response.lines()
            .map { it.split(" ") }
            .filter { it.size == 4 }
            .map { line ->
                try {
                    val epoch = convertLong(line[0], "epoch")
                    val temperature = convert(line[1], "temperature")
                    val humidity = convert(line[2], "air humidity")
                    val soil = convert(line[3], "soil moisture")
                    SensorData(Date(epoch*1000), temperature, humidity, soil)
                } catch (e: Exception) {
                    infoLog("Invalid time date: $line[0]")
                    SensorData(Date(0), 0, 0, 0)
                }

            }
            .filter { it.date.time != 0L }
            .toList()
        Log.i(TAG, "received ${lines.size} valid data")
        handler.sendMessage(handler.obtainMessage(SENSOR_DATA, lines))
    }
}

private fun convertLong(value: String, fieldName: String): Long {
    try {
        return value.toLong()
    } catch (e: Exception) {
        throw SensorConvertError("Invalid $fieldName: $value")
    }
}

private fun convert(value: String, fieldName: String): Int {
    try {
        return value.toInt()
    } catch (e: Exception) {
        throw SensorConvertError("Invalid $fieldName: $value")
    }
}
