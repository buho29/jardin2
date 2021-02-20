
//console.log(Foo());
//router
const pagesRoute = [
  { name: 'home', path: '/', component: () => import('./p-home.js') ,
    meta: {title: 'Jardin',hideNew:true}},
  { name: 'chart', path: '/chart', component:() => import('./p-chart.js')  ,
    meta: {title: 'Historial',hideNew:true}},
  { name: 'zones', path: '/zones/:id',component: () => import('./p-zones.js') ,
    meta: {title: 'Zonas',hideNew:false}},
  { name: 'zone', path: '/zone/:id', component: () => import('./p-zone.js') ,
    meta: {title: 'Zona',hideNew:false}},
  { name: 'taps', path: '/taps', component: () => import('./p-taps.js') ,
    meta: {title: 'Grifos',hideNew:true}},
  { name: 'login', path: '/login', component: () => import('./p-login.js') ,
    meta: {title: 'Login',hideNew:true}},
  { name: 'opciones', path: '/opciones', component:  () => import('./p-option.js'),
    meta: {title: 'Opciones',hideNew:true}},
  { name: 'not found', path: "*", component: { template: '<p>Page Not Found</p>' }}
]

//menu leftdata
const menu = [
  { title: 'Home', show :true, icon: 'icon-sun-cloud', path: '/' },
  { title: 'Gráfico', show :true, icon: 'icon-stats-dots', path: '/chart' },
  { title: 'Zonas', show :true, icon: 'icon-watering-can', path: '/zones/0' },
  { title: 'Grifos', show :true, icon: 'icon-water-pump', path: '/taps' },
  { title: 'Login', show :true, icon: 'icon-cloud-check', path: '/login' },
  { title: 'Opciones', show :true, icon: 'icon-cog', path: '/opciones' },
]

const router = new VueRouter({
  // mode: 'history', 
  routes: pagesRoute 
})

Vue.prototype.$window = window;

//aplication
const app = new Vue({
  el: '#app',
  router,
  store,
  mixins:[mixinNotify],
  data: 
  function () {
    return {
      left: false,
      showMessage: false,
      showLoading:true,
      message:"",
      transitionName : 'slide-right',
      menu: menu,
    }
  },
  computed: {
    ...Vuex.mapState(['loaded','isConnected','authenticate','routingServer']),
    buttonLogin: function () {
      return this.menu[4];
    }
  },
  methods: {
    // importamos acciones
    ...Vuex.mapActions(['connect']),
    ...Vuex.mapMutations(['logout','unload']),
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
    
    //mostramos o escondemos la pagina de login
    this.buttonLogin.show = !this.authenticate;

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
    //cambiamos el estilo de las transiciones 
    // cada vez q se cambia de pagina 
    '$route'(to, from){
      this.transitionName = this.transitionName === 'slide-left'  ? 'slide-right' : 'slide-left';
    },
    //mostrando o escondiendo un "loading..."
    loaded: function (newValue, oldValue) {
      this.showLoading = !newValue && !this.isConnected;
    },
    //idem
    isConnected: function (newValue, oldValue) {
      this.showLoading = !newValue;
    },
    authenticate: function (newValue, oldValue) {
      this.buttonLogin.show = !newValue;
    }
  }
}).$mount('#app')

// cuando se cierra la ventana
// cerrar websockets
window.onbeforeunload = function() {
  store.commit('unload');
};