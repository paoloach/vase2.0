package it.achdjian.paolo.vase20

import android.annotation.SuppressLint
import android.graphics.Color
import android.os.Bundle
import android.preference.PreferenceManager
import android.util.Log
import android.view.Menu
import android.view.MenuItem
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.content.edit
import com.github.mikephil.charting.components.XAxis
import com.github.mikephil.charting.data.Entry
import com.github.mikephil.charting.data.LineData
import com.github.mikephil.charting.data.LineDataSet
import it.achdjian.paolo.vase20.Rest.SensorData
import kotlinx.android.synthetic.main.activity_main.*
import kotlinx.android.synthetic.main.content_main.*
import java.text.SimpleDateFormat
import java.util.*


class MainActivity : AppCompatActivity() {
    companion object {
        const val TAG = "MainActivity"
        const val IP_KEY = "IP"
        @SuppressLint("SimpleDateFormat")
        val DATE_FORMAT = SimpleDateFormat("hh:mm")
        val X_AXIS_DATE_FORMATTER = AxisDateFormatter()
    }

    private lateinit var request: RestRequest
    private val listsData = TreeMap<Date, SensorData>()
    private var lastDate: Date?=null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        request = RestRequest(PreferenceManager.getDefaultSharedPreferences(this), this)
        setSupportActionBar(toolbar)
        btIP.setOnClickListener {
            PreferenceManager.getDefaultSharedPreferences(this).edit {
                putString(IP_KEY, editIP.text.toString())
                request.requestWhoAreYou()
            }
        }
        light_switch.setOnCheckedChangeListener { _, isChechked ->
            if (isChechked)
                request.setOn()
            else
                request.setOff()
        }
        applyTimeButton.setOnClickListener {
            var dawn: Date? = null
            try {
                dawn = DATE_FORMAT.parse(dawnTime.text.toString())
            } catch (e: Exception) {
                Log.i(TAG, "error reading dawnTime widget: ", e)
                Toast.makeText(this, "Invalid dawn time: should be in the format hh:mm", Toast.LENGTH_LONG).show()
                dawnTime.requestFocus()
            }
            dawn?.let {

                try {
                    val sunset = DATE_FORMAT.parse(sunsetTime.text.toString())
                    request.setTime(dawn, sunset)
                } catch (e: Exception) {
                    Log.i(TAG, "error reading sunsetTime widget: ", e)
                    Toast.makeText(this, "Invalid sunset time: should be in the format hh:mm", Toast.LENGTH_LONG).show()
                    sunsetTime.requestFocus()
                }
            }
        }
        updateButton.setOnClickListener {
            listsData.clear()
            lastDate=null
            request.updateDate(0, 50)
        }

        graph.xAxis.valueFormatter = X_AXIS_DATE_FORMATTER
        graph.xAxis.granularity = 5*60f // granularity at 5 minutes
        graph.xAxis.position = XAxis.XAxisPosition.BOTTOM
    }

    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        // Inflate the menu; this adds items to the action bar if it is present.
        menuInflater.inflate(R.menu.menu_main, menu)
        return true
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        return when (item.itemId) {
            R.id.action_settings -> true
            else -> super.onOptionsItemSelected(item)
        }
    }

    fun connected() {
        connectedImage.setImageResource(android.R.drawable.presence_online)
        light_switch.isEnabled = true
        dawnTime.isEnabled = true
        sunsetTime.isEnabled = true
        applyTimeButton.isEnabled=true
        updateButton.isEnabled=true
        graph.isEnabled=true
        request.requestStatus()
        dawnTime.isEnabled = true
    }

    fun disconnected() {
        connectedImage.setImageResource(android.R.drawable.presence_offline)
        light_switch.isEnabled = false
        dawnTime.isEnabled = false
        sunsetTime.isEnabled = false
        applyTimeButton.isEnabled=false
        updateButton.isEnabled=false
        graph.isEnabled=false
    }

    fun lightOn() {
        light_switch.isChecked = true
    }

    fun lightOff() {
        light_switch.isChecked = false
    }

    @SuppressLint("SetTextI18n")
    fun setDawn(hour: Int, minute: Int) {
        dawnTime.setText("$hour:$minute", TextView.BufferType.NORMAL)
    }

    @SuppressLint("SetTextI18n")
    fun setSunset(hour: Int, minute: Int) {
        sunsetTime.setText("$hour:$minute", TextView.BufferType.NORMAL)
    }

    fun updateData(list: List<SensorData>) {
        Log.i(TAG,"Got ${list.size} data: first is ${list.first().date},  last is ${list.last().date}")
        list.filter { it.date.time > 1560000000  && it.temperature < 800 && it.humidity <= 1000 && it.soil < 1024 }.forEach { listsData[it.date] = it }
        lastDate?.let {
            val diff = it.time - list.first().date.time
            Log.i(TAG,"diff: $diff")
            if (diff > 60){
                request.updateDate(listsData.size, 50)
                lastDate = list.first().date
            }
        }?:updateLastDate(list)
        displayData(listsData)

    }

    private fun updateLastDate(list: List<SensorData>) {
        lastDate=list.first().date
        request.updateDate(listsData.size, 50)
    }

    fun displayData(list: Map<Date,SensorData>){
        Log.i(TAG,"Using ${list.size} data")
        Log.i(TAG,"Using ${list.size} data: first is ${list.keys.first()},  last is ${list.keys.last()}")
        val temperatures = list.map{Entry(it.key.time.toFloat(), it.value.temperature.toFloat()/10)}.toList()
        val soil = list.map{Entry(it.key.time.toFloat(), it.value.soil.toFloat())}.toList()
        val humidities = list.map{Entry(it.key.time.toFloat(), it.value.humidity.toFloat()/10)}.toList()
        val dataSetTemperatures = LineDataSet(temperatures, "Temperatures")
        val dataSetSoil = LineDataSet(soil, "soil")
        val dataHumidities = LineDataSet(humidities, "humidity")


        dataSetTemperatures.setDrawCircles(false)
        dataSetSoil.setDrawCircles(false)
        dataHumidities.setDrawCircles(false)

        dataSetTemperatures.color = Color.DKGRAY
        dataSetSoil.color = Color.GREEN
        dataHumidities.color = Color.BLUE

        val lineData = LineData(dataSetTemperatures)
        graph.data = lineData
        graph.invalidate()
    }
}
