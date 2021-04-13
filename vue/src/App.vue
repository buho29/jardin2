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
    getMsg(pl) {
      if (pl) {
        let args = pl.split("|");
        if(args.length>0){
          return this.$t(`server[${args[0]}]`, [args[1]]);
        }
      }
      return this.$t(`server[${pl}]`);
    },
  },
  beforeCreate() { 
    //cargamos localStorage (token,lang)
    this.$store.commit('loadLocal');
  },
  mounted() {
    this.$q.iconSet.field.error = 'icon-alert-circle';
    this.$q.iconSet.arrow.dropdown = 'icon-menu-down';
    this.$q.iconSet.tabs.right = 'icon-chevron-right';
    this.$q.iconSet.tabs.left = 'icon-chevron-left';
    
    this.$q.iconSet.datetime.now = 'icon-alarm';
    //console.log(this.$q.iconSet);
  },
  created() {
    
    this.connect();

    // console.log('Updating mutation', action.type,action.payload);
    // nos registrammos a las llamadas de acciones en vuex
    this.unsubscribe = this.$store.subscribeAction((action, state) => {
      
      switch (action.type) {
          case "onClose":
            this.notifyW(this.$t('app.disconnected'));
            break;
          case "onOpen":
            this.notify(this.$t('app.connected'));
            break;
          case "onError":
            this.notifyW(this.$t('app.error'));
            break;
          case "message":
            let pl = action.payload;
            let msg = this.getMsg(pl.content);
            //ok
            if(pl.type ===0) this.notify(msg);
            //error
            else if(pl.type ===1){
              this.message = msg;
              this.showMessage = true;
            }// warn
            else if(pl.type == 2){
              this.notifyW(msg);
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
