package it.achdjian.vase2.ui

import android.util.Log
import androidx.compose.Composable
import androidx.compose.state
import androidx.ui.core.Text
import androidx.ui.layout.*
import androidx.ui.material.RadioButton
import androidx.ui.material.RadioGroup
import androidx.ui.tooling.preview.Preview
import it.achdjian.vase2.getData
import java.time.LocalDateTime


private const val TAG="PeriodPicker"

abstract class PeriodPicked {
    abstract fun pickerName(): String
    abstract fun fromTime(): LocalDateTime
    abstract fun toTime(): LocalDateTime
}

class OneDay : PeriodPicked() {
    override fun pickerName(): String = "One day"
    override fun fromTime(): LocalDateTime = LocalDateTime.now().minusDays(1)
    override fun toTime(): LocalDateTime = LocalDateTime.now()
}

class OneWeek : PeriodPicked() {
    override fun pickerName(): String = "One Week"
    override fun fromTime(): LocalDateTime = LocalDateTime.now().minusWeeks(1)
    override fun toTime(): LocalDateTime = LocalDateTime.now()
}

class TwoWeek : PeriodPicked() {
    override fun pickerName(): String = "Two Week"
    override fun fromTime(): LocalDateTime = LocalDateTime.now().minusWeeks(2)
    override fun toTime(): LocalDateTime = LocalDateTime.now()
}


@Composable
fun PeriodPicker() {
    val radioOptions = listOf(OneDay(), OneWeek(), TwoWeek())
    val (selectedOption, onOptionSelected) = state { radioOptions[1] }

    RadioGroup {
        Row(modifier = LayoutWidth.Fill, arrangement = Arrangement.SpaceEvenly) {
            radioOptions.forEach { picker ->
                val selected = picker.pickerName() == selectedOption.pickerName()
                Log.i(TAG, "Selected option: ${selectedOption.pickerName()}, selected=$selected")
                RadioGroupItem(selected = selected, onSelect = {
                    onOptionSelected(picker)
                    getData(picker.fromTime(), picker.toTime())
                }) {
                    Column {
                        Text(text = picker.pickerName())
                        Row(modifier = LayoutGravity.Center) {
                            RadioButton(
                                selected = selected,
                                onSelect = {
                                    onOptionSelected(picker)
                                    getData(picker.fromTime(), picker.toTime())
                                })
                        }
                    }
                }
            }
        }
    }
}

@Preview
@Composable
fun previewPeriodPicker() {
    PeriodPicker()
}