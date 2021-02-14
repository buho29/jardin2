const login = {
    data: function () {
        return {
            name: null,
            pass: null,
            isPwd: null
        };
    },
    methods: {
        ...Vuex.mapActions(['setAuthentication']),
        onSubmit() {
            this.setAuthentication({ name: this.name, pass: this.pass });
        },
        onReset() {
            this.name = null;
            this.pass = null;
        }
    },
    template: /*html*/ `
    <q-page>
      <b-container title="Admin">
        <q-form @submit="onSubmit" @reset="onReset" class="q-gutter-md " >
      
          <q-input filled v-model="name" label="Tu nombre *"
            lazy-rules :rules="[ val => val && val.length > 0 || 'Please type something']">
          </q-input>
          
          <q-input v-model="pass" filled 
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
    </q-page>`
};