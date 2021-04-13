<template>
    <q-page>

      <q-card class="q-ma-lg q-mx-auto text-center items-center bg-white"
        style="min-width: 300px; max-width: 600px; width: 90%" v-if="authenticate">
        
        <q-tabs
          class="bg-primary text-white shadow-1" dense
          indicator-color="accent" align="justify" narrow-indicator
          v-model="tab"
        >
          <q-tab name="general" :label="$t('config.general')"/>
          <q-tab name="zones" label="Zonas"/>
          <q-tab name="user" :label="$t('config.user')"/>
          <q-tab name="system" :label="$t('config.system')"/>
          <q-tab name="log" :label="$t('config.history')"/>
          <q-tab name="icons" label="Icons"/>
        </q-tabs>

        <q-separator/>

        <q-tab-panels v-model="tab" animated keep-alive>

          <q-tab-panel name="general"  class="q-gutter-y-md">
            <b-card :title="$t('config.browser')">
              <p v-html="$t('config.helpRouting')"/>
              <q-toggle label= "routing server" :value="routingServer" 
                @input="setRoutingServer(!routingServer)">
              </q-toggle>
              <q-select v-model="lang" :options="langs" label="Language" dense filled
                borderless emit-value map-options options-dense />              
            </b-card>

            <b-card :title="$t('config.time')">
              <b-tz/>
            </b-card>
            
            <b-card :title="$t('config.weather')">
              <b-config-weather/>
            </b-card>

          </q-tab-panel>
          
          <q-tab-panel name="zones" class="q-gutter-y-md">
            <b-time-line/>
          </q-tab-panel>

          <q-tab-panel name="user" class="q-gutter-y-md">
            
            <b-card :title="$t('config.user')">
              <b-config-admin/>
            </b-card>

            <b-card title="Wifi">
              <b-config-wifi/> 
            </b-card>
            
          </q-tab-panel>

          <q-tab-panel name="system"  class="q-gutter-y-md">
            
            <b-card :title="$t('config.info')">
              <div  v-for="(value,key) in system" class="row q-mx-auto " :key="key" style="width:60%;" >
                <div class="col text-bold text-capitalize text-left"> {{key}}</div>
                <div class="col text-right"> {{value}}</div>
              </div>
              
              <q-btn color="primary" class="q-ma-sm"
                @click="restart()" >
                {{$t('config.resetEsp')}}
              </q-btn>
            </b-card>

            <b-card :title="$t('config.files')">
              <b-files/>
            </b-card>
            
          </q-tab-panel>
          
          <q-tab-panel name="log" class="q-gutter-y-md">
            <b-log/>
          </q-tab-panel>

          <q-tab-panel name="icons">
            <b-card title="Icons">
              <div class="text-purple q-pa-md row items-start">
                <div v-for="(item,id) in icons" :key="id"  style="width: 70px;">
                  <q-icon :name="item" style="font-size: 2em;"></q-icon>  
                  <p>{{item}}</p>    
                </div>
              </div>
            </b-card>
            <b-card title="Meteo">
              <div class="q-pa-md row items-start">
                <b-icon  v-for="(item) in meteo" :key="item"
                  :icon="item"  style="font-size:70px;"/>
              </div>
            </b-card>
          </q-tab-panel>

        </q-tab-panels>
      </q-card>

    </q-page>
</template>

<script>
import {mapActions, mapMutations,mapState} from 'vuex'

import mixinRequiresAuth from 'src/components/mixin/mixinRequiresAuth';
import BFiles from 'src/components/options/bFiles.vue';
import BIcon from 'src/components/bIcon.vue';
import BConfigWeather from 'src/components/options/bConfigWeather.vue';
import BConfigAdmin from 'src/components/options/bConfigAdmin.vue';
import BConfigWifi from 'src/components/options/bConfigWifi.vue';
import BTz from 'src/components/options/bTz.vue';
import BCard from 'src/components/bCard.vue';
import BLog from 'src/components/options/bLog.vue';
import BTimeLine from 'src/components/options/bTimeLine.vue';

export default {
  components: {BFiles, BIcon, BConfigWeather, BConfigAdmin,
   BConfigWifi, BTz, BCard, BLog, BTimeLine },
  name:"Options",
  mixins: [mixinRequiresAuth],
  data() {
    return {
      langs: [
        { label: 'Español', value: 'es-es' },
        { label: 'English', value: 'en-us' },
      ],
      tab:"general",
      lang: this.$i18n.locale,
      icons:["icon-alarm" , "icon-alert-circle" , "icon-chevron-down" , 
        "icon-chevron-up" , "icon-cloud-check" , "icon-cog" , "icon-delete" , 
        "icon-menu" , "icon-menu-down" , "icon-moon" , "icon-pencil" , "icon-plus" ,
        "icon-stats-dots" , "icon-sun" , "icon-sun-cloud" , "icon-thermometer",
        "icon-watering-can" , "icon-water-pump","icon-eye-off", "icon-eye",
        "icon-cloud-upload", "icon-cloud-download" , "icon-file-outline",
        "icon-folder", "icon-folder-multiple","icon-help","icon-magnify",
        "icon-chevron-right","icon-chevron-right"
      ],
      meteo:[1,2,3,4,5,6,7,8,11,12,13,14,15,16,17,
        18,19,20,21,22,23,24,25, 26,29,30,31,32,33,34,35,
        36,37,38,39,40,41,42,43,44,],
    }
  },
  computed: {
    //importamos datos
    ...mapState(['system','authenticate',
          'routingServer','loaded','isConnected']),
  },
  methods: {
    // importamos acciones
    ...mapActions(['restart','registerAuth']),
    ...mapMutations(['setRoutingServer'])
  }, 
  mounted() {
    if(this.authenticate && this.isConnected)
      //nos registramos a eventos auth al servidor 
      this.registerAuth(true);
  },
  beforeDestroy() {
    this.registerAuth(false);
  },
  watch: {
    //si abren directamente la pagina opciones
    // y despues q se cargo to el tinglado
    loaded: function (newValue, oldValue) {
      //nos registramos a eventos auth al servidor
      this.registerAuth(true);
    },
    lang(lang) {
      this.$i18n.locale = lang;
      localStorage.setItem('lang',lang);
    },
  },
}
</script>