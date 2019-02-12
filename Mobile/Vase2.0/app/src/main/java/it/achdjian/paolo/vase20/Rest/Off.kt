package it.achdjian.paolo.vase20.Rest

/**
 * Created by Paolo Achdjian on 2/10/19.
 */
class Off(val restEngine: RestEngine): Runnable {
    override fun run() =  restEngine.postMethod("/off")
}