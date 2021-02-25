<template>
    <b-container title="Wifi">
      <q-form @submit="onSubmit" @reset="onReset" class="q-gutter-sm" >
      
        <q-input filled v-model="name" label="Tu SSID(wifi) *" dense
          lazy-rules :rules="[ val => val && val.length > 0 || 'Please type something']">
        </q-input>
        
        <q-input v-model="pass" filled dense
          lazy-rules :rules="[ val => val && val.length > 0 || 'Please type something']"
          :type="isPwd ? 'password' : 'text'" label="Tu password *">
          <template v-slot:append>
            <q-icon
              :name="isPwd ? 'icon-eye-off' : 'icon-eye'"
              class="cursor-pointer"
              @click="isPwd = !isPwd"
            />
          </template>
        </q-input>
  
        <div>
          <q-btn label="Enviar" type="submit" color="primary"/>
          <q-btn label="Reset" type="reset" color="primary" flat class="q-ml-sm" />
        </div>
      </q-form>   
    </b-container>
</template>
<script>

import {mapActions} from 'vuex'
import bContainer from '../bContainer.vue';
import mixinOption from '../mixin/mixinOption';
export default {
  components: { bContainer },
    name:"b-config-wifi",   
     data () {
        return {
            name: null,
            pass: null,
            isPwd: true
        };
    },
    mixins: [mixinOption],
    methods: {
        ...mapActions(['editConfig']),
        onSubmit() {
            this.editConfig({ wifi_ssid: this.name, wifi_pass: this.pass });
        },
        onReset() {
            this.name = null;
            this.pass = null;
        },
        update(){
          this.name = this.config.wifi_ssid;
        }
    },
}
</script>