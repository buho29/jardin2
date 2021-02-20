// para formatear Time
var mixinFormat = {
  methods: {
      formatTime: function (utc) {


          let s = Math.floor(utc % 60);
          let m = Math.floor((utc / 60) % 60);
          let h = Math.floor((utc / (60 * 60)) % 24);
          let d = Math.floor(utc / (60 * 60 * 24));

          s = s < 10 ? "0" + s : "" + s;
          m = m < 10 ? "0" + m : "" + m;
          h = h < 10 ? "0" + h : "" + h;

          return h + ":" + m + ":" + s;
      },
      getTimeFromFormat: function (str) {
          let i = str.split(":");
          let r = parseInt(i[0]) * 3600;
          r += parseInt(i[1]) * 60;
          if (i.length > 2) r += parseInt(i[2]);
          return r;
      }
  }
}

var mixinNotify = {
  methods: {
      notify: function (msg) {
          //quasar notify
          this.$q.notify({
              color: 'primary',
              textColor: 'white',
              icon: 'icon-cloud-check',
              message: msg
          });
      },
      notifyW: function (msg) {
          this.$q.notify({
              color: 'accent',
              textColor: 'dark',
              icon: 'icon-alert-circle',
              message: msg
          })
      },
  }
}

//restingir acceso a pagina
var mixinRequiresAuth = {
  // importamos un mixin para notificaciones
  mixins: [mixinNotify],
  computed: {
      // importamos de vuex si esta logeado
      ...Vuex.mapState(['authenticate'])
  },
  //interceptando el acceso a la pagina
  beforeRouteEnter(to, from, next) {
      next(vm => {
          // si no esta logueado
          if (!vm.authenticate) {
              // ir a la pagina de login
              vm.$router.push("/login")
              vm.notifyW("Requiere login!");
          }
      })
  }
}


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
`});
Vue.component("b-icon", {
  props: ["icon"],
  template: /*html*/ `
  <div :class="'icon-'+ icon"  style="text-shadow: 2px 2px 4px #999999;">
    <span class="path1"></span><span class="path2"></span><span class="path3"></span>
  </div>
`});

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
`})

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
