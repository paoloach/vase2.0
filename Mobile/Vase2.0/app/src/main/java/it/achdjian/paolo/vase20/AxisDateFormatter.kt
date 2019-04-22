package it.achdjian.paolo.vase20

import android.util.Log
import com.github.mikephil.charting.components.AxisBase
import com.github.mikephil.charting.formatter.IAxisValueFormatter
import java.text.SimpleDateFormat
import java.util.*

/**
 * Created by Paolo Achdjian on 2/15/19.
 */
class AxisDateFormatter  :IAxisValueFormatter {
    /**
     * Called when a value from an axis is to be formatted
     * before being drawn. For performance reasons, avoid excessive calculations
     * and memory allocations inside this method.
     *
     * @param value the value to be formatted
     * @param axis  the axis the value belongs to
     * @return
     */
    override fun getFormattedValue(value: Float, axis: AxisBase?): String {
        val formatter = SimpleDateFormat("dd HH:mm")
        val date = Date(value.toLong())
        return formatter.format(date)
    }
}