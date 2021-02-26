<template>
  <q-page> 
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
    </q-page>
  </q-page>
</template>

<script>
import {mapState} from 'vuex'

import bSensor from "src/components/home/bSensor.vue";
import BTimer from "src/components/home/bTimer.vue";
import mixinNotify from "src/components/mixin/mixinNotify.js";
import BIcon from "src/components/bIcon.vue";
import BContainer from "src/components/bContainer.vue";
import BParam from 'src/components/bParam.vue';

export default {
  components: { bSensor, BTimer, BIcon, BContainer, BParam },
  mixins: [mixinNotify],
  name: "Home",
  computed: {
    ...mapState(["weather", "sensor", "loaded"]),
  },
  data: function () {
    return {
      icon: 1,
    };
  },
  methods: {
    getInfo() {
      let w = this.weather;
      return `${w.precipitationProbability}% / ${w.totalLiquid} mm / ${w.hoursOfPrecipitation}h`;
    },
    formatTime(time) {
      return new Date(time * 1000).toLocaleTimeString("es-ES");
    },
    getFileIcon(id) {
      let r = "/asset/";
      if (id < 10) r += "0" + id;
      else r += id;
      r += "-m.webp";
      return r;
    },
  },
};
</script>
