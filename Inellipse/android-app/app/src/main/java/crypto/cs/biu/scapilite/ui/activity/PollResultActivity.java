package crypto.cs.biu.scapilite.ui.activity;

import android.annotation.TargetApi;
import android.os.Build;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.TextView;

import com.github.mikephil.charting.charts.BarChart;
import com.github.mikephil.charting.components.XAxis;
import com.github.mikephil.charting.data.BarData;
import com.github.mikephil.charting.data.BarDataSet;
import com.github.mikephil.charting.data.BarEntry;

import java.util.ArrayList;
import java.util.List;

import crypto.cs.biu.scapilite.R;
import crypto.cs.biu.scapilite.model.Poll;
import crypto.cs.biu.scapilite.util.constants.AppConstants;

/**
 * Created by Blagojco on 11/04/2018- 11:05
 */

public class PollResultActivity extends AppCompatActivity {

    private Poll nextPoll;
    private String pollResult;
    private TextView poll_result_title;
    private TextView poll_result_desc;
    private TextView poll_result_average_salary;
    private BarChart poll_results_chart;

    @TargetApi(Build.VERSION_CODES.M)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_poll_results);

        getSupportActionBar().setDisplayShowHomeEnabled(true);
        getSupportActionBar().setIcon(R.mipmap.ic_launcher);

        init();

        poll_result_title.setText(nextPoll.getTitle());
        poll_result_desc.setText(nextPoll.getDescription());
        if (nextPoll.getResultType() == null || nextPoll.getResultType().equals(Poll.POOL_RESULT_TYPE_BASIC)) {
            showBasicResult();
        } else {
            showHistogramResult();

        }
    }

    private void showHistogramResult() {
        List<String> xValues = new ArrayList<>(); // "Denmark", "Finland", ...
        xValues.add("100K+ Euro");
        xValues.add("80-100K Euro");
        XAxis xAxis = poll_results_chart.getXAxis();
//        xAxis.setValueFormatter(new MyValueFormatter(xValues));

        BarEntry valuesMen = new BarEntry(50, 122);
        BarEntry valuesMen2 = new BarEntry(110, 152);
        BarEntry valuesMen3 = new BarEntry(530, 322);
        BarEntry valuesMen4 = new BarEntry(4, 662);
        List<BarEntry> entries = new ArrayList<>();
        entries.add(valuesMen);
        entries.add(valuesMen2);
        entries.add(valuesMen3);
        entries.add(valuesMen4);
        // create 2 datasets
//        BarDataSet dataset = new BarDataSet(entries,"# of Calls&quot;);

        BarDataSet set1 = new BarDataSet(entries, "Men");
        set1.setColor(getResources().getColor(R.color.colorPrimaryDark));

        BarData data = new BarData(set1);
        poll_results_chart.setData(data);
//        chart.groupBars(...); // available since release v3.0.0
        poll_results_chart.invalidate(); // refresh
    }

    private void showBasicResult() {
        poll_result_average_salary.setText(String.valueOf(pollResult) + " " + getString(R.string.euros));
        poll_result_average_salary.setVisibility(View.VISIBLE);
    }


    private void init() {
        nextPoll = (Poll) getIntent().getSerializableExtra(AppConstants.CHOOSEN_POLL);
        pollResult = getIntent().getStringExtra(AppConstants.POLL_RESULT);

        poll_results_chart = findViewById(R.id.poll_results_chart);
        poll_result_title = findViewById(R.id.poll_result_title);
        poll_result_desc = findViewById(R.id.poll_result_desc);
        poll_result_average_salary = findViewById(R.id.poll_result_average_salary);
    }


}