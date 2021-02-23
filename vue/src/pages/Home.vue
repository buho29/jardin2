<template>
  <q-page class="flex flex-center">
    <img
      alt="Quasar logo"
      src="~assets/quasar-logo-full.svg"
    >
    <b-icon icon="10"></b-icon>
    
        <b-container title="Icons">
          <div class="text-purple q-pa-md row items-start">
            <div v-for="(item,id) in icons" :key="id"  style="width: 110px;">
              <q-icon :name="item" style="font-size: 2em;"></q-icon>  
              <p>{{item}}</p>    
            </div>
          </div>
        </b-container>
  </q-page>
</template>

<script>
import bSensor from 'src/components/bSensor.vue'
import BTimer from 'src/components/bTimer.vue'
import mixinNotify from 'src/components/mixin/mixinNotify.vue'
import BIcon from 'src/components/bIcon.vue'
import BContainer from 'src/components/bContainer.vue'
export default {
  components: { bSensor, BTimer, BIcon, BContainer },
  mixins:[mixinNotify],
  name: 'Home',
  data () {
    return {
        icons:["icon-alarm" , "icon-alert-circle" , "icon-chevron-down" , 
          "icon-chevron-up" , "icon-cloud-check" , "icon-cog" , "icon-delete" , 
          "icon-menu" , "icon-menu-down" , "icon-moon" , "icon-pencil" , "icon-plus" ,
          "icon-stats-dots" , "icon-sun" , "icon-sun-cloud" , "icon-thermometer",
          "icon-watering-can" , "icon-water-pump","icon-eye-off", "icon-eye",
          "icon-cloud-upload", "icon-cloud-download" , "icon-file-outline",
          "icon-folder", "icon-folder-multiple","icon-help",
        ],
    }
  },
  created (){
    console.log("sqsd");
    this.notify("sqsdqsd");
  }
}
</script>
