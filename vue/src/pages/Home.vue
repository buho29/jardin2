<template>
  <q-page> 
    <b-container :title="$t('sensors')" v-if="loaded" >
      <div class="flex q-ma-none" >
        <b-sensor :prop="$t('temp')" :value="sensor.te+'°'"></b-sensor>
        <b-sensor :prop="$t('hum')" :value="sensor.hu+'%'"></b-sensor>
        <b-sensor :prop="$t('press')" :value="sensor.pr"></b-sensor>
      </div>
    </b-container>

    <b-container :title="$t('weather.name')" v-if="loaded && weather.days" >
    
      <div class='row'>
        <b-icon :icon="today.icon" class="q-pa-md" style="font-size:120px;"/>
        <b-timer :time="weather.time" style="flex-grow: 1;"/>
      </div>

      <br/>
      <div class="text-h6 text-primary"> {{today.cityName}} </div>
      <div class="q-mx-auto" style="width:300px;">
        <b-param :title="$t('weather.win')">
          {{today.winSpeed}}km/h - {{today.winDirection}}°
        </b-param>
        <b-param :title="$t('weather.clouds')">{{today.cloudCover}}%</b-param>
        <b-param :title="$t('temp')"> 
          <span class="text-blue">min.{{today.minTemp}}° </span>
          <span class="text-red">max.{{today.maxTemp}}°</span>
        </b-param>
        <b-param :title="$t('weather.precipitation')">{{infoPrecipitation}}</b-param>
        <b-param :title="$t('weather.sun')">
          {{formatDate(today.sunStart,'LTS')}}-{{formatDate(today.sunEnd,'LTS')}}
        </b-param>
      </div>
      <br/>
      <div class="text-h6">{{today.phrase}}</div>
    
    </b-container>
    <b-container :title="$t('weather.name')" v-if="loaded && !weather.days" >
      <div class="text-h6 text-primary"> Error loading data</div>
    </b-container>
 
    <b-container title="Prevision" v-if="loaded && weather.days" >
      <div class="flex">
        <q-card class="q-mx-auto q-pa-sm" v-for="item in weather.days" :key="item.time">
          <div class="text-primary">{{formatDate(item.time,"ddd")}}</div>
          <b-icon :icon="item.icon"  style='font-size:40px;'/>
          <div> 
            <span class="text-blue">{{item.minTemp}}° </span> 
            <span class="text-red">{{item.maxTemp}}°</span>
          </div>
        </q-card>
      </div>
    </b-container>
    
   </q-page>
</template>

<script>

import moment from 'moment'

import {mapState} from 'vuex'

import bSensor from "src/components/home/bSensor.vue";
import BTimer from "src/components/home/bTimer.vue";
import mixinNotify from "src/components/mixin/mixinNotify.js";
import BIcon from "src/components/bIcon.vue";
import BContainer from "src/components/bContainer.vue";
import BParam from 'src/components/bParam.vue';
import BCard from 'src/components/bCard.vue';

export default {
  components: { bSensor, BTimer, BIcon, BContainer, BParam, BCard },
  mixins: [mixinNotify],
  name: "Home",
  computed: {
    ...mapState(["weather", "sensor", "loaded"]),
    infoPrecipitation() {
      let w = this.weather.days[0];
      return `${w.precipitationProbability}% / ${w.totalLiquid} mm / ${w.hoursOfPrecipitation}h`;
    },
    today(){
      return this.weather.days[0];
    },
  },
  data: function () {
    return {
      icon: 1,
    };
  },
  methods: {
    formatDate(date,format){
      return moment.utc(date*1000).locale(this.$t('localeMoment')).format(format);
    }
  },
};
</script>
