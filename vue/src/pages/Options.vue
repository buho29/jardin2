<template>
    <q-page>

      <div class="q-ma-lg q-mx-auto text-center items-center bg-white"
        style="min-width: 300px; max-width: 600px; width: 90%">
        
        <q-tabs
          v-model="tab"
          class="bg-primary text-white shadow-2" dense
          indicator-color="accent" align="justify" narrow-indicator
        >
          <q-tab name="browser" :label="$t('config.browser')"/>
          <q-tab name="time" :label="$t('config.time')"/>
          <q-tab name="weather" :label="$t('config.weather')"/>
          <q-tab name="user" :label="$t('config.user')"/>
          <q-tab name="wifi" label="Wifi"/>
          <q-tab name="files" :label="$t('config.files')"/>
          <q-tab name="info" :label="$t('config.info')"/>
          <q-tab name="icons" label="Icons"/>
        </q-tabs>

        <q-separator/>

        <q-tab-panels v-model="tab" animated keep-alive>

          <q-tab-panel name="browser">
            <p v-html="$t('config.helpRouting')"/>
            <q-toggle label= "routing server" :value="routingServer" 
              @input="setRoutingServer(!routingServer)">
            </q-toggle>
            <q-select v-model="lang" :options="langs" label="Language" dense filled
              borderless emit-value map-options options-dense />
          </q-tab-panel>

          <q-tab-panel name="time">
            <b-tz/>
          </q-tab-panel>

          <q-tab-panel name="weather">
            <b-config-weather/>
          </q-tab-panel>

          <q-tab-panel name="user">
            <b-config-admin/>
          </q-tab-panel>

          <q-tab-panel name="wifi">
            <b-config-wifi/> 
          </q-tab-panel>

          <q-tab-panel name="files">
            <b-files/>
          </q-tab-panel>

          <q-tab-panel name="info">
            <div  v-for="(value,key) in system" class="row q-mx-auto " :key="key" style="width:60%;" >
              <div class="col text-bold text-capitalize text-left"> {{key}}</div>
              <div class="col text-right"> {{value}}</div>
            </div>
            
            <q-btn color="primary" class="q-ma-sm"
              @click="restart()" >
              {{$t('config.resetEsp')}}
            </q-btn>
          </q-tab-panel>

          <q-tab-panel name="icons">
            <div class="text-purple q-pa-md row items-start">
            <div v-for="(item,id) in icons" :key="id"  style="width: 110px;">
              <q-icon :name="item" style="font-size: 2em;"></q-icon>  
              <p>{{item}}</p>    
            </div>
          </div>
          </q-tab-panel>

        </q-tab-panels>
      </div>

    </q-page>
</template>

<script>
import {mapActions, mapMutations,mapState} from 'vuex'

import BContainer from 'src/components/bContainer.vue';
import mixinRequiresAuth from 'src/components/mixin/mixinRequiresAuth';
import BFiles from 'src/components/options/bFiles.vue';
import BIcon from 'src/components/bIcon.vue';
import BConfigWeather from 'src/components/options/bConfigWeather.vue';
import BConfigAdmin from 'src/components/options/bConfigAdmin.vue';
import BConfigWifi from 'src/components/options/bConfigWifi.vue';
import BTz from 'src/components/options/bTz.vue';

export default {
  components: {BContainer, BFiles, BIcon, BConfigWeather, BConfigAdmin, BConfigWifi, BTz },
  name:"Options",
  mixins: [mixinRequiresAuth],
  data() {
    return {
      langs: [
        { label: 'Español', value: 'es-es' },
        { label: 'English', value: 'en-us' },
      ],
      tab:"browser",
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