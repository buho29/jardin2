<template>
    <q-form @submit="onSubmit" @reset="onReset" class="q-gutter-sm" >
      
      <b-search-city @input="searchResult"/>

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
</template>
<script>


import {mapActions} from 'vuex';
import BBtnSend from '../bBtnSend.vue';
import mixinOption from '../mixin/mixinOption';
import BSearchCity from './bSearchCity.vue';

export default {
  components: { BBtnSend, BSearchCity },
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
        },
        searchResult(obj){
          this.cityName = obj.label;
          this.cityID = obj.cityID;
        }
    },
}
</script>