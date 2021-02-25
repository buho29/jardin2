<template>
  <div id="q-app">
    <router-view />
    
    <q-dialog v-model="showMessage" persistent>
      <q-card style="min-width: 300px">
        <q-card-section
          class="bg-primary text-h6 text-center text-white shadow-3 q-pa-md"
        >
          Alert
        </q-card-section>
        <q-card-section class="row items-center">
          {{ message }}
        </q-card-section>
        <q-card-actions align="right">
          <q-btn flat label="ok" color="primary" v-close-popup></q-btn>
        </q-card-actions>
      </q-card>
    </q-dialog>
    <q-dialog v-model="showLoading" persistent>
      <q-card>
        <q-card-section class="row items-center">
          <q-spinner color="primary" size="3em" :thickness="10"></q-spinner>
        </q-card-section>
      </q-card>
    </q-dialog>
  </div>
</template>


<script>
import {mapActions, mapMutations,mapState,mapGetters} from 'vuex'
import mixinNotify from './components/mixin/mixinNotify.js';

export default {
  name: 'App',
  mixins:[mixinNotify],
  data () {
    return {
      showMessage: false,
      showLoading:true,
      message:"",
    }
  },
  computed: {
    ...mapState(['loaded','isConnected','routingServer']),
  },
  methods: {
    // importamos acciones
    ...mapActions(['connect']),
    ...mapMutations(['logout','unload']),
  },
  beforeCreate() { 
    //cargamos localStorage (token)
    this.$store.commit('loadLocal');
  },
  mounted() {
    this.$q.iconSet.field.error = 'icon-alert-circle';
    this.$q.iconSet.arrow.dropdown = 'icon-menu-down';
    this.$q.iconSet.datetime.now = 'icon-alarm';
  },
  created() {
    
    this.connect();

    // nos registrammos a las llamadas de acciones en vuex
    this.unsubscribe = this.$store.subscribeAction((action, state) => {
      
      //console.log('Updating mutation', action.type,action.payload);
      switch (action.type) {
          case "onClose":
            this.notifyW("Desconectado !!");
            break;
          case "onOpen":
            this.notify("Connectado !!");
            break;
          case "onError":
            this.notifyW("ha ocurrido un error !!");
            break;
          case "message":
            let msg = action.payload;
            //ok
            if(msg.type ===0) this.notify(msg.content);
            //error
            else if(msg.type ===1){
              this.message = msg.content;
              this.showMessage = true;
            }// warn
            else if(msg.type == 2){
              this.notifyW(msg.content);
            }
            break;
          case "goTo":
            let path = action.payload;
            if(this.$route.path != path && this.routingServer){
              this.$router.push(path);
            }
            break;
        }
      });
  },
  beforeDestroy() {
    this.unsubscribe();
  },
  watch: {
    //mostrando o escondiendo un "loading..."
    loaded: function (newValue, oldValue) {
      this.showLoading = !newValue && !this.isConnected;
    },
    //idem
    isConnected: function (newValue, oldValue) {
      this.showLoading = !newValue;
    },
  }
  
}
</script>
