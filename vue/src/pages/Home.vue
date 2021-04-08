<template>
  <q-page> 
        <q-page>
  
      <b-container :title="$t('sensors')" v-if="loaded" >
        <div class="row q-gutter-md" >
          <b-sensor :prop="$t('temp')" :value="sensor.te+'°'"></b-sensor>
          <b-sensor :prop="$t('hum')" :value="sensor.hu+'%'"></b-sensor>
          <b-sensor :prop="$t('press')" :value="sensor.pr"></b-sensor>
        </div>
      </b-container>

      <b-container :title="$t('weather.name')" v-if="loaded && weather.cityName" >
      
        <div class='row '>
          <b-icon :icon="weather.icon1" style="font-size:120px;"/>
          <b-timer :diff="weather.diff" style="flex-grow: 1;"/>
        </div>
  
        <br/>
        <div class="text-h6 text-primary"> {{weather.cityName}} </div>
        <div class="q-mx-auto" style="width:250px;">
          <b-param :title="$t('weather.win')">
            {{weather.winSpeed}}km/h - {{weather.winDirection}}°
          </b-param>
          <b-param :title="$t('weather.clouds')">{{weather.cloudCover}}%</b-param>
          <b-param :title="$t('temp')"> min.{{weather.minTemp}}° max.{{weather.maxTemp}}°</b-param>
          <b-param :title="$t('weather.precipitation')">{{infoPrecipitation}}</b-param>
          <b-param title="sol">{{formatTime(weather.sunStart)}}-{{formatTime(weather.sunEnd)}}</b-param>
        </div>
        <br/>
        <div class="text-h6">{{weather.phrase}}</div>
      
      </b-container>
      <b-container :title="$t('weather.name')" v-if="loaded && !weather.cityName" >
  
        <div class="text-h6 text-primary"> Error loading data</div>
      
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
    infoPrecipitation() {
      let w = this.weather;
      return `${w.precipitationProbability}% / ${w.totalLiquid} mm / ${w.hoursOfPrecipitation}h`;
    },
  },
  data: function () {
    return {
      icon: 1,
    };
  },
  methods: {
    formatTime(time) {
      return new Date(time * 1000).toLocaleTimeString(this.$t('localeTime'));
    },
  },
};
</script>
