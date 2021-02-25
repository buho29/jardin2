<template>
    <b-container title="Administrador">
        <q-form @submit="onSubmit" @reset="onReset" class="q-gutter-sm" >
          <p>Cambiar login</p>
          <q-input filled v-model="name" label="Tu nombre *" dense
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
            <q-btn label="Enviar" type="submit" color="primary" />
            <q-btn label="Reset" type="reset" color="primary" flat class="q-ml-sm" />
          </div>
        </q-form>   
      </b-container>
</template>
<script>

import {mapActions, mapMutations,mapState,mapGetters} from 'vuex'
import bContainer from '../bContainer.vue';
export default {
  components: { bContainer },
    name:"b-config-admin",
    data: function () {
        return {
            name: null,
            pass: null,
            isPwd: true
        };
    },
    methods: {
        ...mapActions(['editConfig']),
        onSubmit() {
            this.editConfig({www_user : this.name, www_pass: this.pass });
        },
        onReset() {
            this.name = null;
            this.pass = null;
        }
    },
}
</script>