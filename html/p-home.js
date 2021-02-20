export default {
    computed: {
      ...Vuex.mapState(['weather', 'sensor', 'loaded'])
    },
    data :function () {
      return {
        icon : 1
      }
    },
    methods:{
      getInfo(){
        let w = this.weather;
        return `${w.precipitationProbability}% / ${w.totalLiquid} mm / ${w.hoursOfPrecipitation}h`;
      },
      formatTime(time){
        return new Date(time*1000).toLocaleTimeString("es-ES");
      },
      getFileIcon(id){
        let r = '/asset/';
        if(id<10) r += "0"+id;
        else r += id;
        r +='-m.webp';
        return r;
      }
    },
    template: /*html*/`
    <q-page>
  
      <b-container title="Sensores" v-if="loaded" >
        <div class="row q-gutter-md" >
          <b-sensor prop="Temperatura" :value="sensor.te+'°'"></b-sensor>
          <b-sensor prop="Humedad" :value="sensor.hu+'%'"></b-sensor>
          <b-sensor prop="Presion" :value="sensor.pr"></b-sensor>
        </div>
      </b-container>
      
      <b-container title="Tiempo" v-if="loaded" >
      
        <div class='row '>
          <b-icon :icon="weather.icon1" style="font-size:120px;"/>
          
          <b-timer local="es-ES" :weather="weather" style="flex-grow: 1;"/>
        </div>
  
        <br/>
        <div class="q-mx-auto" style="width:250px;">
          <b-param title="Viento">
            {{weather.winSpeed}}km/h - {{weather.winDirection}}°
          </b-param>
          <b-param title="Nubosidad">{{weather.cloudCover}}%</b-param>
          <b-param title="Temperatura"> min.{{weather.minTemp}}° max.{{weather.maxTemp}}°</b-param>
          <b-param title="Precipitacion">{{getInfo()}}</b-param>
          <b-param title="sol">{{formatTime(weather.sunStart)}} {{formatTime(weather.sunEnd)}}</b-param>
        </div>
        <br/>
        <div class="text-h6">{{weather.phrase}}</div>
      
      </b-container>
    </q-page>`
}

Vue.component('b-sensor', {
    props: ['prop', 'value'],
    template: /*html*/`
    <q-card  class=" text-h6">     
      <q-card-section class="q-pa-none text-primary">
        <div class="q-pa-sm">{{prop}}</div>
        <q-separator/>
      </q-card-section>
      <q-card-section>
        <div class="text-dark ">{{value}}</div>
      </q-card-section>
    </q-card>
    `
})

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
`});