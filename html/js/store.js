Vue.use(Vuex);
const store = new Vuex.Store({
  // datos que bindearemos en vue/paginas/componentes
  state: {
    sensor: {},
    weather: {},
    sensors: [],
    zones: [],
    taps: [],
    alarms: [],
    rootFiles: [],
    config: {},
    system: {},
    isConnected: false,
    loaded:false,
    authenticate:false,
    routingServer:null,
  },
  //solo se puede editar el state mediante mutation
  mutations: {
    
    //updates state
    updateSensors(state, array) { state.sensors = array; },
    updateSensor(state, obj) { state.sensor = obj;},

    updateConfig(state, obj) { 
      state.config = obj;
    },

    updateWeather(state, obj) {
      obj.diff = Date.now() - obj.time*1000;
      state.weather = obj;
    
    },

    updateTaps(state, array) { state.taps = array; },
    
    updateZones(state, array) { state.zones = array;},
    //para actualizar solo una zona
    //cuando se rega una zona y tal
    updateZone(state, obj) { 
      let zone = this.getters.getZoneById(obj.id);
      for (const key in obj) {
        zone[key] = obj[key];
      }
    },

    updateAlarms(state, array) { 
      state.alarms = array; 
      //alarmas es el ultimo array q se carga al inicio
      // usado para esconder cosas cuando aun no hay datos
      state.loaded = true;
    },

    

    //login
    authenticated(state,token){
      state.authenticate = true;
      state.token = token;
      localStorage.setItem('token',token);
    },
    
    //system 
    updateSystem(state,obj){
      state.system = obj;
    },
    //rootFiles
    updateRootFiles(state,obj){
      state.rootFiles = obj
    },

    //logout
    logout(state){
      state.authenticate = false;
      state.token = undefined;
      localStorage.removeItem('token');
    },
    
    setRoutingServer(state,bool){
      state.routingServer = bool;
      localStorage.setItem('routingServer',bool);
    },

    //loadLocal
    loadLocal(state){
      let token = localStorage.getItem('token');
      if (token) {
        state.authenticate = true;
        state.token = token;        
      }
      let b = localStorage.getItem('routingServer');
      if(b === null) {
        //default
        localStorage.setItem('routingServer',true);
        state.routingServer = true; 
      } else {
        state.routingServer = b === "true";
      }

    },
    
    //se cierra la ventana
    unload(state){
      if (this.connection !== undefined) {
        this.connection.close();
      }
    },

    //conected
    connected(state, bool) { 
      state.isConnected = bool; },
  },
  //llamadas a servidor async y cia
  //si se tiene q modificar el state se usara commit("mutation")
  actions: {
    
    //connexion inicial websocket
    connect({ commit, dispatch }) {
      if (this.connection !== undefined) {
        this.connection.close();
      }
      this.host = document.location.host;
      this.host = "192.168.8.100";

      this.connection = new WebSocket('ws://' + this.host + '/ws');

      //delegamos los eventos a las acciones
      this.connection.onmessage = event => dispatch("onMessage", event);
      this.connection.onclose = event => dispatch("onClose", event);
      this.connection.onerror = event => dispatch("onError", event);
      this.connection.onopen = event => dispatch("onOpen", event);

    },
    //reiniciar esp32
    restart({ commit, dispatch }, obj) {
      dispatch("send", {
        system: { restart: true}
      });
    },
    // riego
    //abrir grifo
    openTap({ commit, dispatch }, obj) {
      dispatch("send", {
        tap: { id: obj.id, open: obj.value }
      });
    },
    //regar zona
    waterZone({ commit, dispatch }, obj) {
      dispatch("send", {
        zone: { id: obj.id, runing: obj.value }
      });
    },
    pauseZone({ commit, dispatch }, obj) {
      dispatch("send", {
        zone: { id: obj.id, pause: obj.value }
      });
    },

    //alarmas datos
    deleteAlarm({ commit, dispatch }, id) {  
      dispatch("send", { alarm: { delete: id } });
    },
    editAlarm({ commit, dispatch }, alarm) {  
      dispatch("send", { alarm: { edit: alarm } });
    },
    addAlarm({ commit, dispatch }, alarm) {
      dispatch("send", { alarm: { new: alarm} });
    },   

    //zone datos
    deleteZone({ commit, dispatch }, id) {
      dispatch("send", { zone: { delete: id } });
    }, 
    editZone({ commit, dispatch }, zone) {
      dispatch("send", { zone: {edit: zone} });
    },
    addZone({ commit, dispatch }, zone) {
      dispatch("send", { zone: {new: zone}});
    },

    // TODO config
    editConfig({ commit, dispatch }, obj) {
      dispatch("send", { config: obj});
    },

    // recibe wasaps del servidor de eventos/errores
    // hay q suscribirse a vuex para tratar los mensajes
    message({ commit, state }, message) {
      //is event
    },

    // para manejar el routing desde servidor y vuex
    // podemos abrir paginas al usuario
    // hay q suscribirse a vuex
    goTo({ commit, state }, path) {
      //is event
    },

    // intento de autentificacion
    setAuthentication({dispatch, commit, state }, user) {
      if (this.connection !== undefined) {
        this.connection.send(
          JSON.stringify({ login: user})
        );
      } else dispatch("connect");
    },

    //para recibir eventos autenticadas
    registerAuth({dispatch},bool){
      dispatch("send", { auth:bool });
    },

    //envia objectos/comandos en json al servidor
    //todo pasa por aqui salvo setAuthentication
    send({ commit, dispatch ,state}, obj) {
      // si no estamos identificado
      if(!state.authenticate){
        //dispatch("goTo","/login");
        dispatch("message",{type:2,content:"Requiere login!"});
        return;
      }

      //metemos el token
      obj.token = state.token;
        
      if (this.connection !== undefined) {
        this.connection.send(
          JSON.stringify(obj)
        );
      } else dispatch("connect");
    },

    //file

    downloadItem ({commit, dispatch,state },name) {
      
      axios.get(`http://${this.host}/file`,{          
        headers: { 'Authorization': `Basic ${state.token}`},
        params: {download:name},
        responseType: 'blob'
      }).then(response => {
          const blob = new Blob([response.data],{type: 'application/*' });
          const link = document.createElement('a');
          link.href = URL.createObjectURL(blob);
          link.download = name.split("/").pop();
          link.click()
          URL.revokeObjectURL(link.href)
        }).catch((error) => {
          console.error(error);
          dispatch("message",{type:1,content:`Error descarga
          ${name} 
          ${error.message}`});
      });
    },
    deleteItem ({commit, dispatch,state },name) {
      
      axios.get(`http://${this.host}/file`, {          
        headers: { 'Authorization': `Basic ${state.token}`},
        params: {delete:name},
      }).then(response => {
          dispatch("message",{type:0,content:"Borrado con exito"});
        }).catch((error) => {
          console.error(error);
          dispatch("message",{type:1,content:`Error borrando 
          ${name} ${error.message}`});
      });
    },
    uploadItem({commit, dispatch,state },{file,params}){
      let formData = new FormData();
      formData.append('file', file);
      let filename = file;
      axios.post( `http://${this.host}/file`,
        formData,{
          headers: {
            'Authorization': `Basic ${state.token}`,
            'Content-Type': 'multipart/form-data',
          },
          params: params,
        }
        ).then((res) => {
          dispatch("message",{type:0,content:"Subido con exito"});
        })
        .catch((error) => {
          console.log(error);
          dispatch("message",{type:1,content:`Error subiendo 
          ${file?file.name:'no difinido'} 
          ${error.message}`});
        });
    },
    

    //privado

    // eventos websocket
    //json recibido del servidor
    onMessage({ commit, dispatch }, event) {
      //convierte json en un objecto js
      let json = JSON.parse(event.data);

      // TODO mutations
      let mutations ={
        sensor:"updateSensor",sensors:"updateSensors",taps:"updateTaps",
        zones:"updateZones",zone:"updateZone",alarms:"updateAlarms",token:"authenticated",
        system:'updateSystem',root:'updateRootFiles',weather:'updateWeather',
        config:'updateConfig',
      }
      //actualizamos los datos por commit("mutation")
      for (const key in mutations) {
        if(json[key]!==undefined)
          commit(mutations[key], json[key]);
      }

      let actions ={
        message:"message", goTo:"goTo"
      }
      //ejecutamos acciones dispatch("action")
      for (const key in actions) {
        if(json[key]!==undefined)
          dispatch(actions[key], json[key]);
      }
      
    },
    onClose({ commit, dispatch }, event) {
      // ponemos state.isConnected a false
      commit("connected", false);
      //reconectamos al pasar 3sg
      setTimeout(() => {
        dispatch("connect");
      }, 3000)
    },
    onError({ commit, state }, event) {
      //is event
    },
    onOpen({ commit, state }, event) {
      commit("connected", true);
    }, 

  },
  // usado como acceso directo a datos de state
  getters: {
    getZoneById: (state) => (id) => {
      return state.zones.find(zone => zone.id === id);
    },
    getZoneByName: (state) => (name) => {
      return state.zones.find(zone => zone.name === name);
    },
    getTapById: (state) => (id) => {
      return state.taps.find(tap => tap.id === id);
    },
    getAllAlarmsSize: (state) => (id) => {
      return state.alarms.filter(alarm => alarm.zoneId === id).length;
    },
    getAllAlarms: (state) => (id) => {
      return state.alarms.filter(alarm => alarm.zoneId === id);
    },
    getAlarmById: (state) => (id) => {
      return state.alarms.find(alarm => alarm.id === id);
    },
    getAlarmIndex: (state,getters) => (id) => {
      let alarm = getters.getAlarmById(id);
      let alarms = getters.getAllAlarms(alarm.zoneId);
      return alarms.findIndex(alarm => alarm.id === id);
    },
    getLastAlarm: (state,getters) => (id) => {
      let alarms = getters.getAllAlarms(id);
      if(alarms.length<1) return undefined;
      let lastAlarm = alarms.reduce((a,c) => {
        return(a.time>c.time ? a :c);
      });
      return lastAlarm;
    },
  },
  strict: true,
})