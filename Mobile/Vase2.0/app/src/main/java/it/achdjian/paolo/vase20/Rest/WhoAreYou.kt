package it.achdjian.paolo.vase20.Rest

import android.os.Handler
import android.util.Log

/**
 * Created by Paolo Achdjian on 2/8/19.
 */
class WhoAreYou(val restEngine: RestEngine, val handler: Handler) : Runnable {
    companion object {
        const val TAG="WhoAreYou"
    }
    override fun run() {
        Log.i(TAG,"get status")
        val response = restEngine.getMethod("/who_are_you")
        Log.i(TAG,"response: " + response)
        if (response == "I am Vase 2.0\n"){
            Log.i(TAG,"Found")
            handler.sendMessage(handler.obtainMessage(WHO_ARE_YOU_PRESENT))
        } else {
            Log.i(TAG,"NOT Found")
            handler.sendMessage(handler.obtainMessage(WHO_ARE_YOU_NOT_PRESENT))
        }
    }
}