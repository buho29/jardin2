<script>
import { Line, mixins } from 'vue-chartjs';
const { reactiveData } = mixins;
import moment from 'moment'

export default {
  name: "b-line-chart",
  extends: Line,
  mixins: [reactiveData],
  props: ['data', 'tags', 'title','labels','format'],
  data() {
    return {
      options: {
        responsive: true, maintainAspectRatio: false,
        scales: { yAxes: [{ ticks: { beginAtZero: false } }] }
      },
      colors :['#31CCEC','#008577','#C10015']
    }
  },
  methods: {
    update(){

      let arrData = [];
      for (let j = 0; j < this.labels.length; j++) {
        arrData[j] = [];
      }
      let arrTime = [];
      for (let i = 0; i < this.data.length; i++) {
        for (let j = 0; j < this.labels.length; j++) {
          arrData[j].push(this.data[i][this.tags[j]]);
        }
        arrTime.push(
          this.formatDate(this.data[i].ti)
        );
      }

      let datasets = [];
      for (let j = 0; j < this.labels.length; j++) {
        datasets.push(
          this.makeDataSet(this.labels[j],arrData[j],j)
        )
      }

      this.chartData = {
        labels: arrTime,
        datasets: datasets
      }
    },
    makeDataSet (label,data,indexColor){
      return {
        label: label,
        data: data,
        fill: true,
        borderColor: this.colors[indexColor],
        backgroundColor: '#00000000',
        borderWidth: 2, type: 'line',
        pointRadius: 2, lineTension: 0,
        borderWidth: 2
      }
    },
    formatDate(date){
      return moment.utc(date*1000).format(this.format);
    }
  },
  watch: {
    data: function (newQuestion, oldQuestion) {
      this.update();
    }
  },
  mounted() {
    this.update();
    this.renderChart(this.chartData, this.options)
  },
}
</script>