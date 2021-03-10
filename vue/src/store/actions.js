
// connexion inicial websocket
export function connect({ commit, dispatch }) {
  if (this.connection !== undefined) {
    this.connection.close();
  }

  this.connection = new WebSocket('ws://' + this.host + '/ws');

  //delegamos los eventos a las acciones
  this.connection.onmessage = event => dispatch("onMessage", event);
  this.connection.onclose = event => dispatch("onClose", event);
  this.connection.onerror = event => dispatch("onError", event);
  this.connection.onopen = event => dispatch("onOpen", event);

}
//reiniciar esp32
export function restart({dispatch }) {
  dispatch("send", {
    system: { restart: true}
  });
}
// riego
//abrir grifo
export function openTap({ commit, dispatch }, obj) {
  dispatch("send", {
    tap: { id: obj.id, open: obj.value }
  });
}
//regar zona
export function waterZone({ commit, dispatch }, obj) {
  dispatch("send", {
    zone: { id: obj.id, runing: obj.value }
  });
}
export function pauseZone({ commit, dispatch }, obj) {
  dispatch("send", {
    zone: { id: obj.id, pause: obj.value }
  });
}

//alarmas datos
export function deleteAlarm({ commit, dispatch }, id) {  
  dispatch("send", { alarm: { delete: id } });
}
export function editAlarm({ commit, dispatch }, alarm) {  
  dispatch("send", { alarm: { edit: alarm } });
}
export function addAlarm({ commit, dispatch }, alarm) {
  dispatch("send", { alarm: { new: alarm} });
}

//zone datos
export function deleteZone({ commit, dispatch }, id) {
  dispatch("send", { zone: { delete: id } });
}
export function editZone({ commit, dispatch }, zone) {
  dispatch("send", { zone: {edit: zone} });
}
export function addZone({ commit, dispatch }, zone) {
  dispatch("send", { zone: {new: zone}});
}

export function editConfig({ commit, dispatch }, obj) {
  dispatch("send", { config: obj});
}

// recibe wasaps del servidor de eventos/errores
// hay q suscribirse a vuex para tratar los mensajes
export function message({ commit, state }, message) {
  //is event
}

// para manejar el routing desde servidor y vuex
// podemos abrir paginas al usuario
// hay q suscribirse a vuex
export function goTo({ commit, state }, path) {
  //is event
}

// intento de autentificacion
export function setAuthentication({dispatch, commit, state }, user) {
  if (this.connection !== undefined) {
    this.connection.send(
      JSON.stringify({ login: user})
    );
  } else dispatch("connect");
}

//para recibir eventos autenticadas
export function registerAuth({dispatch},bool){
  dispatch("send", { auth:bool });
}

//envia objectos/comandos en json al servidor
//todo pasa por aqui salvo setAuthentication
export function send({ commit, dispatch ,state}, obj) {
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
}

//file
export function downloadItem ({commit, dispatch,state },name) {
  
  this.$axios.get(`http://${this.host}/file`,{          
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
}
export function deleteItem ({commit, dispatch,state },name) {
  
  this.$axios.get(`http://${this.host}/file`, {          
    headers: { 'Authorization': `Basic ${state.token}`},
    params: {delete:name},
  }).then(response => {
      dispatch("message",{type:0,content:"Borrado con exito"});
    }).catch((error) => {
      console.error(error);
      dispatch("message",{type:1,content:`Error borrando 
      ${name} ${error.message}`});
  });
}
export function uploadItem({commit, dispatch,state },{file,params}){
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
}

//privado
// eventos websocket
//json recibido del servidor
export function onMessage({ commit, dispatch }, event) {
  //convierte json en un objecto js
  let json = JSON.parse(event.data);

  // TODO mutations
  let mutations ={
    sensor:"updateSensor",sensors:"updateSensors",sensors24:"updateSensors24",taps:"updateTaps",
    zones:"updateZones",zone:"updateZone",alarms:"updateAlarms",token:"authenticated",
    system:'updateSystem',root:'updateRootFiles',weather:'updateWeather',
    config:'updateConfig',history: 'updatehHistory'
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
  
}
export function onClose({ commit, dispatch }, event) {
  // ponemos state.isConnected a false
  commit("connected", false);
  //reconectamos al pasar 3sg
  setTimeout(() => {
    dispatch("connect");
  }, 3000)
}
export function onError({ commit, state }, event) {
  //is event
}
export function onOpen({ commit, state }, event) {
  commit("connected", true);
}
