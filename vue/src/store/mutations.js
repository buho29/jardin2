
//updates state
export function updateSensors(state, array) { 
    state.sensors = array; 
}

export function updateSensor(state, obj) { 
    state.sensor = obj;
}

export function updateConfig(state, obj) { 
  state.config = obj;
}

export function updateWeather(state, obj) {
  obj.diff = Date.now() - obj.time*1000;
  state.weather = obj;
}

export function updateTaps(state, array) { state.taps = array; }

export function updateZones(state, array) { state.zones = array;}
//para actualizar solo una zona
//cuando se rega una zona y tal
export function updateZone(state, obj) { 
  let zone = this.getters.getZoneById(obj.id);
  for (const key in obj) {
    zone[key] = obj[key];
  }
}

export function updateAlarms(state, array) { 
  state.alarms = array; 
  //alarmas es el ultimo array q se carga al inicio
  // usado para esconder cosas cuando aun no hay datos
  state.loaded = true;
}

//login
export function authenticated(state,token){
  state.authenticate = true;
  state.token = token;
  localStorage.setItem('token',token);
}

//system 
export function updateSystem(state,obj){
  state.system = obj;
}
//rootFiles
export function updateRootFiles(state,obj){
  state.rootFiles = obj
}

//logout
export function logout(state){
  state.authenticate = false;
  state.token = undefined;
  localStorage.removeItem('token');
}

export function setRoutingServer(state,bool){
  state.routingServer = bool;
  localStorage.setItem('routingServer',bool);
}

//loadLocal
export function loadLocal(state){
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

  let lang = localStorage.getItem('lang');
  if(lang){
    this.$i18n.locale = lang;
  }

}

//se cierra la ventana
export function unload(state){
  if (this.connection !== undefined) {
    this.connection.close();
  }
}

//conected
export function connected(state, bool) { 
  state.isConnected = bool; 
}
  