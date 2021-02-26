<template>
    <b-container :title="$t('config.time')">
      <q-form class="q-gutter-sm row" @submit="onSubmit" @reset="onReset">

        <q-input :value="getTz()" label="Time zone" filled hint="Offset(hours)" style="max-width: 110px;" 
          v-model.number="tz" type="number" min="-12" max="12" lazy-rules  :rules="[
            val => val !== null && val !== '' || 'Esta mal...',
            val => val > -13 && val < 13 || 'Escribe entre -12 y +12',
          ]"></q-input>

        
        <q-input label="Daylight saving time"  filled style="flex-grow: 1;" hint="irregularity, startDay, startDay, endDay, endMonth, startHour, endHour" 
          v-model="dst" mask=" # , # , ## , # , ## , # , #">
          <template v-slot:append>
            <q-icon name="icon-help" @click="$window.open('https://github.com/kosme/flex_DST','_blank')"/>
          </template>
        </q-input>
        
        <b-btn-send/>
      </q-form>
    </b-container>
</template>
<script>

import {mapState,mapActions} from 'Vuex';
import BBtnSend from '../bBtnSend.vue';
import bContainer from '../bContainer.vue';
export default {
  components: { bContainer, BBtnSend },
    name:"b-config-tz",  
    data() {
        return {
            tz: 0,
            dst: "0, 0, 00, 0, 00, 0, 0 ",
        };
    },
    computed: {
      //importamos datos
      ...mapState(['config']),
    },
    watch: {
      // los datos cambiaron 
      config: function (newValue, oldValue) {
          //volver a seleccionar los selects con los index
          this.tz = newValue.tz;
          this.dst = newValue.dst;
      }
    },
    methods: {
      ...mapActions(['editConfig']),
      onSubmit() {
          this.editConfig({ tz: this.tz, dst: this.dst }); 
      },
      onReset() {
          this.tz = null;
          this.dst = null;
      },
      getTz(){
        let t = this.config.tz;
        console.log(t);
        return t;
      }
    },
}
</script>