<template>
    <b-container title="Accuweather.com">
      <q-form @submit="onSubmit" @reset="onReset" class="q-gutter-sm" >
      
      <q-input filled v-model="cityName" :label="$t('config.nameCity')" dense
        lazy-rules :rules="[ val => val && val.length > 0 || 'Please type something']">
      </q-input>
      <q-input filled v-model="cityID" label="cityID *" dense
        lazy-rules :rules="[ val => val && val.length > 0 || 'Please type something']">
      </q-input>
      <q-input filled v-model="accuURL" label="extra url *" dense 
      hint="key & lang & metric" 
        lazy-rules :rules="[ val => val && val.length > 0 || 'Please type something']">
      </q-input>
  
        <b-btn-send/>
      </q-form>   
    </b-container>
</template>
<script>


import {mapActions} from 'vuex';
import BBtnSend from '../bBtnSend.vue';
import bContainer from '../bContainer.vue';
import mixinOption from '../mixin/mixinOption';

export default {
  components: { bContainer, BBtnSend },
    name:"b-config-weather",   
    data() {
        return {
          cityName: null,
          cityID: null,
          accuURL: null
        };
    },
    mixins: [mixinOption],
    methods: {
        ...mapActions(['editConfig']),
        onSubmit() {
          this.editConfig({ 
            cityName: this.cityName,
            accuURL:this.accuURL, 
            cityID: this.cityID 
          });
        },
        onReset() {
          this.cityName = null;
          this.cityID = null;
        },
        update(){
          this.cityName = this.config.cityName;
          this.cityID = this.config.cityID;
          this.accuURL = this.config.accuURL;
        }
    },
}
</script>