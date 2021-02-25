<script>
import mixinFormat from '../mixin/mixinFormat.js';
import { Line, mixins } from 'vue-chartjs';
const { reactiveData } = mixins;

export default {
    name: "b-line-chart",
  extends: Line,
  mixins: [reactiveData,mixinFormat],
  props: ['data', 'tag', 'title'],
  data: function () {
    return {
      options: {
        responsive: true, maintainAspectRatio: false,
        scales: { yAxes: [{ ticks: { beginAtZero: false } }] }
      },
    }
  },
  methods: {
    updateChart() {

      let arrData = [];
      let arrTime = [];
      for (let i = 0; i < this.data.length; i++) {
        arrData.push(this.data[i][this.tag]);
        arrTime.push(
          this.formatTime(this.data[i].ti)
        );
      }

      this.chartData = {
        labels: arrTime,
        datasets: [{
          label: this.title,
          data: arrData,
          fill: true,
          borderColor: '#00574b',
          backgroundColor: '#ffbb3355',
          borderWidth: 2, type: 'line',
					pointRadius: 0,lineTension: 0,
					borderWidth: 2
        }]
      }
    }
  },
  watch: {
    data: function (newQuestion, oldQuestion) {
      this.updateChart();
    }
  },
  mounted() {
    this.updateChart();
    this.renderChart(this.chartData, this.options)
  },
}
</script>