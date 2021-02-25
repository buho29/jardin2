<template>
    <q-page>
      
        <b-config-tz/>
        <b-config-weather/>

        <b-container title="Navegador">
          <p>permite al servidor redireccionarte a una pagina <br/> 
            por ejemplo cuando se riega una zona</p>
          <q-toggle label= "routing server" :value="routingServer" 
            @input="setRoutingServer(!routingServer)">
          </q-toggle>
        </b-container>

        <b-config-admin/>
        <b-config-wifi/> 
        <b-files/>
          
        <b-container title="System">
          
          <div  v-for="(value,key) in system" class="row q-mx-auto " :key="key" style="width:60%;" >
            <div class="col text-bold text-capitalize text-left"> {{key}}</div>
            <div class="col text-right"> {{value}}</div>
          </div>
          
          <q-btn color="primary" @click="restart()" class="q-ma-sm">restar Esp32</q-btn>
  
        </b-container>  
  
        <b-container title="Icons">
          <div class="text-purple q-pa-md row items-start">
            <div v-for="(item,id) in icons" :key="id"  style="width: 110px;">
              <q-icon :name="item" style="font-size: 2em;"></q-icon>  
              <p>{{item}}</p>    
            </div>
          </div>
        </b-container>
  
        <b-container title="Meteo">
          <div class="q-pa-md row items-start">
            <b-icon  v-for="(item) in meteo" :key="item"
              :icon="item"  style="font-size:80px;"/>
          </div>
        </b-container>
  
      </q-page>
</template>

<script>
import {mapActions, mapMutations,mapState} from 'vuex'

import BContainer from 'src/components/bContainer.vue';
import mixinRequiresAuth from 'src/components/mixin/mixinRequiresAuth';
import BFiles from 'src/components/options/bFiles.vue';
import BIcon from 'src/components/bIcon.vue';
import BConfigTz from 'src/components/options/bConfigTz.vue';
import BConfigWeather from 'src/components/options/bConfigWeather.vue';
import BConfigAdmin from 'src/components/options/bConfigAdmin.vue';
import BConfigWifi from 'src/components/options/bConfigWifi.vue';

export default {
  components: {BContainer, BFiles, BIcon, BConfigTz, BConfigWeather, BConfigAdmin, BConfigWifi },
    name:"Options",
    mixins: [mixinRequiresAuth],
    data() {
      return {
        icons:["icon-alarm" , "icon-alert-circle" , "icon-chevron-down" , 
          "icon-chevron-up" , "icon-cloud-check" , "icon-cog" , "icon-delete" , 
          "icon-menu" , "icon-menu-down" , "icon-moon" , "icon-pencil" , "icon-plus" ,
          "icon-stats-dots" , "icon-sun" , "icon-sun-cloud" , "icon-thermometer",
          "icon-watering-can" , "icon-water-pump","icon-eye-off", "icon-eye",
          "icon-cloud-upload", "icon-cloud-download" , "icon-file-outline",
          "icon-folder", "icon-folder-multiple","icon-help",
        ],
        meteo:[1,2,3,4,5,6,7,8,11,12,13,14,15,16,17,
          18,19,20,21,22,23,24,25, 26,29,30,31,32,33,34,35,
          36,37,38,39,40,41,42,43,44,
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
    }
}
</script>