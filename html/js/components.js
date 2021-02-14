Vue.component("b-timer", {
  data() {
    return {
      localTime:new Date().toLocaleTimeString('es-ES'),
      currentDate:new Date().toLocaleDateString('es-ES'),
      diff:0,
      serverTime:0
    };
  },
  props: ['weather','local'],
  mounted() {
    this.id = setInterval(this.tick,1000);
  },
  beforeDestroy() {
    clearInterval(this.id);
  },
  methods: {
    tick() {
      let date = new Date(Date.now()-this.weather.diff);
      this.localTime = new Date().toLocaleTimeString(this.local);
      this.serverTime = date.toLocaleTimeString(this.local);
      this.currentDate = date.toLocaleDateString(this.local);
    }
  },  
  template: /*html*/`
  <div class="text-center q-pa-sm">      
    <div class="text-primary text-h5 ">{{currentDate}}</div>
    <div class="text-h5 q-ma-none">
      {{localTime}}<span class="text-body1 text-blue-5"> Local</span>
    </div> 
    <div class="text-h5 q-ma-none">
      {{serverTime}}<span class="text-body1 text-blue-5"> Esp32</span>
    </div>   
  </div>
`
});

Vue.component("b-container", {
  props: ["title"],
  template: /*html*/ `

  <div class="q-ma-lg q-mx-auto text-center items-center bg-white" 
    style="min-width: 400px;max-width: 600px;width: 90%;">
        
    <div class="bg-primary text-white shadow-3 ">
      <div  class="text-h6 q-pa-sm">{{title}} </div>
    </div>
        
    <div class="q-pa-md" >
      <slot></slot>
    </div>

  </div>
`
});

Vue.component("b-param", {
  props: ["title"],
  template: /*html*/`
  <div class="row">
    <div class="col text-bold text-capitalize text-left">{{title}}</div>
    <div class="col text-right"><slot></slot></div>
  </div>
`});

Vue.component('chart', {
  props: ['data', 'tag', 'title'],
  template: /*html*/`
  <q-card  class="q-ma-md">
    <q-card-section class="q-pa-none q-ma-none">
      <div class="bg-primary text-white shadow-3 ">
        <div  class="text-h6 q-pa-sm">{{title}}</div>
      </div>
    </q-card-section>
    <q-card-section>
    <line-chart :data="data" :title="title" :tag="tag" 
      style="height: 300px;"></line-chart>
    </q-card-section>
  </q-card>
`
})

Vue.component('line-chart', {
  extends: VueChartJs.Line,
  mixins: [VueChartJs.mixins.reactiveData,mixinFormat],
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
});

Vue.component('bToggle', {
  model: {
    prop: 'checked',
    event: 'change'
  },
  props: {
    labelOn: String,
    labelOff: String,
    width: { type: String, default: '120px' },
    checked: { type: Boolean, default: false },
    disable: { type: Boolean, default: false },
  },
  computed: {
    styleW() {
      return `width:${this.width}`;
    }
  },
  methods: {
    onClick() {
      this.$emit('change', !this.checked)
    }
  },
  template: /*html*/
    `<div>
    <q-btn v-if="checked" @click="onClick" :disable="disable" 
      class='bg-accent text-dark q-ma-sm' :style="styleW">{{labelOff}}</q-btn>
    <q-btn v-if="!checked" @click="onClick" :disable="disable" 
      class='bg-primary text-white q-ma-sm' :style="styleW">{{labelOn}}</q-btn>
  </div>`
})
