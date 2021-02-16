Vue.component("b-select-flags", {
  data: function () {
    return {
      //seleccion en bin ej: 2|16 (18)
      mFlags: 0,
      //modelo se llena segun el select ej :[2,16]
      model: []
    };
  },
  props: ["options", "flags", "label"],
  watch: {
    //model a cambiado
    model: function (newValue, oldValue) {
      
      if(newValue.toString() === oldValue.toString()) 
        return;
      //recalculamos flag
      this.mFlags = 0;
      for (let i = 0; i < newValue.length; i++) {
        //"sumamos" el valor
        this.addF(newValue[i]);
      }
      
      this.$emit("input", this.mFlags);
      
    },
    
    //flags a cambiado
    flags: function (newValue, oldValue) {
      this.set(newValue);
    }
  },
  methods: {
    //inicio/set
    set(flag) {
      this.mFlags = flag;
      //vaciamos model
      this.model.splice(0, this.model.length);
      
      for (let i = 0; i < this.options.length; i++) 
      {
        //cuando contiene el valor
        if (this.checkF(this.options[i].value)) {
          //lo agregamos al model
          this.model.push(this.options[i].value);
        }
      }
    },
    // agrega un flags
    addF(flags) {
      this.mFlags |= flags;
    },
    // devuelve si cuando existe todos los flags
    checkF(flags) {
      return (this.mFlags & flags) == flags;
    }
  },
  mounted() {
    if(this.flags)
    this.set(this.flags);
  },
  template: /*html*/ `
  <q-select filled v-model="model" :options="options" :label="label"
    multiple emit-value map-options>
    <template v-slot:option="scope">
      <q-item v-bind="scope.itemProps" v-on="scope.itemEvents">
        <q-item-section>
          <q-item-label v-html="scope.opt.label" ></q-item-label>
        </q-item-section>
        <q-item-section side>
          <q-toggle v-model="model" :val="scope.opt.value" />
        </q-item-section>
      </q-item>
    </template>
  </q-select>
`});

const zone = {
    mixins: [mixinFormat, mixinRequiresAuth],
    data: function () {
        return {
            zoneId: -1,
            fab: false,
            showDialog: false,
            name: null,
            can_watering: true,
            lastAlarm: false,
            selectedAlarmId: -1,
            time: '10:15:00',
            duration: 10,
            tap: null,
            alarmsZone: [],
            modesFlags: 0,
            modesData: [
                { label: 'Desactivar 24h', value: 0b0000000001}, 
                { label: 'Meteorologia', value  : 0b0000000010},
                { label: 'Sensores', value      : 0b0000000100}, 
                { label: 'Lunes', value         : 0b0000001000},
                { label: 'Martes', value        : 0b0000010000},
                { label: 'Miercoles', value     : 0b0000100000},
                { label: 'Jueves', value        : 0b0001000000}, 
                { label: 'Viernes', value       : 0b0010000000},
                { label: 'Sabado', value        : 0b0100000000}, 
                { label: 'Domingo', value       : 0b1000000000}
            ],
        }
    },
    computed: {
        ...Vuex.mapState(
            ['taps', 'zones', 'isConnected', 'alarms']
        ),
        ...Vuex.mapGetters(
            ['getAllAlarms', 'getZoneById', 'getZoneByName',
                'getAlarmById', 'getLastAlarm', 'getTapById']
        ),
    },
    watch: {
        zones: function (newLoaded, oldLoaded) {
            this.update();
        },
        alarms: function (newLoaded, oldLoaded) {
            this.alarmsZone = this.getAllAlarms(this.zoneId);
        },
    },
    beforeRouteUpdate(to, from, next) {
        let id = parseInt(to.params.id);
        //nueva zona
        if (id < 0) {
            // borramos
            this.name = "";
            this.modesFlags = 0;
            this.alarmsZone = [];
        }
        next();
    },
    methods: {
        ...Vuex.mapActions(
            ['deleteAlarm', 'editZone', 'editAlarm', 'addZone', 'addAlarm',]
        ),
        change(flags){
          this.modesFlags = flags;
          console.log("change",this.modesFlags);
        },
        onSaveZone() {
            let error = false;
            if (this.zoneId >= 0) {
              console.log(this.modesFlags);
                this.editZone({
                    id: this.zoneId,
                    name: this.name,
                    modes: this.modesFlags
                });
            } else {
                this.addZone({
                    name: this.name,
                    modes: this.modesFlags
                });
            }
        },
        onSaveAlarm() {
            let time = this.getTimeFromFormat(this.time);
            console.log("time ", time, this.time);
            if (this.selectedAlarmId > -1) {
                this.editAlarm({
                    id: this.selectedAlarmId, zoneId: this.zoneId,
                    tapId: this.tap.id, time: time, duration: this.duration
                });
                this.showDialog = false;
            } else {
                this.addAlarm({
                    zoneId: this.zoneId, tapId: this.tap.id,
                    time: time, duration: this.duration
                });
                this.showDialog = false;
            }
        },
        showDialogAlarm(id) {
            this.selectedAlarmId = id;
            if (id >= 0) {
                const alarm = this.getAlarmById(id);
                this.lastAlarm = false;
                this.time = this.formatTime(alarm.time);
                this.duration = alarm.duration;
                this.tap = this.getTapById(alarm.tapId);
            } else {
                this.lastAlarm = true;
                let alarm = this.getLastAlarm(this.zoneId);
                if (alarm !== undefined)
                    this.time = this.formatTime(alarm.time + alarm.duration);
            }
            //mostramos el dialog alarm
            this.showDialog = true;
        },
        update() {
            this.zoneId = parseInt(this.$route.params.id);

            if (this.zoneId < 0 && this.name != null) {
                const zone = this.getZoneByName(this.name);
                if (zone !== undefined) {
                    this.$router.push({ path: `/zone/${zone.id}` });
                    this.zoneId = zone.id;
                }
            } else if (this.zoneId >= 0) {
                const zone = this.getZoneById(this.zoneId);
                if (zone !== undefined) {
                    this.name = zone.name;
                    this.modesFlags = zone.modes;
                    this.can_watering = zone.can_watering;
                }

            }
        },
        onLastAlarm(evt) {
            if (evt) {
                let alarm = this.getLastAlarm(this.zoneId);
                if (alarm !== undefined)
                    this.time = this.formatTime(alarm.time + alarm.duration);
            } else if (this.selectedAlarmId > -1) {
                let alarm = this.getAlarmById(this.selectedAlarmId);
                this.time = this.formatTime(alarm.time);
            }
        }

    },
    mounted() {
        this.update();
        this.alarmsZone = this.getAllAlarms(this.zoneId);
    },
    template: /*html*/`
      <q-page class="q-pa-none">
    
        <div  class="q-mt-lg q-mx-auto text-center bg-white" style="max-width: 400px">
          
          <div class="bg-primary text-white shadow-3 ">
              <div  class="text-h6 q-pa-sm">{{name}}</div>
          </div>
          
          <q-form @submit="onSaveZone" class="q-gutter-md q-pa-md" >
            
            <div  class="text-h6 text-blue-5 " v-if="can_watering">Hoy se riega</div>
            <div  class="text-h6 text-red-5" v-else>Hoy no se riega </div>
            <q-input dense filled v-model="name" label="Nombre *"
              lazy-rules :rules="[ val => val && val.length > 0 || 'Por favor escriba algo']">
            </q-input>
            <b-select-flags label="Modos" :options="modesData" :flags="modesFlags" @input="change"/>
              <div class="col-12 text-right">
                  <q-btn flat label="Cancelar" @click="$router.go(-1)" class="text-primary"></q-btn>
                  <q-btn label="Guardar" type="submit"  class="bg-primary text-white"></q-btn>
              </div>
          </q-form>
    
          <div class="bg-primary text-white shadow-3 row ">
              <div  class="text-h6 q-pa-sm col">Alarmas</div>
                    
              <q-fab v-model="fab"  v-if="zoneId>-1"
                flat dense class="col-auto" icon="icon-cog" active-icon="icon-cog"
                direction="left" padding="sm">
    
                <q-fab-action v-if="selectedAlarmId>-1" padding="5px" color="warning" icon="icon-delete" 
                external-label label-position="bottom" @click="deleteAlarm(selectedAlarmId)" label="Borrar" />
    
                <q-fab-action v-if="selectedAlarmId>-1" padding="5px" color="accent" label-position="bottom" 
                external-label @click="showDialogAlarm(selectedAlarmId)" icon="icon-pencil" label="Editar" />
    
                <q-fab-action padding="5px" external-label label-position="bottom" class="text-dark bg-white" 
                  @click="showDialogAlarm(-1)" icon="icon-plus" label="Nueva" />
                
              </q-fab>
          </div>
    
          <q-list bordered>
            <q-item clickable v-ripple v-for="alarm in alarmsZone" :key="alarm.id" 
              :active="selectedAlarmId === alarm.id" @click="selectedAlarmId=alarm.id" 
              active-class="my-menu-link">

              
    
              <q-item-section avatar>
                <q-icon color="primary" name="icon-alarm" />
              </q-item-section>
              
              <q-item-section>
                {{taps[alarm.tapId].name}} - 
                {{formatTime(alarm.time)}} - 
                {{alarm.duration}}sg
              </q-item-section>
            </q-item>
          </q-list>
        </div>
        <q-dialog v-model="showDialog">
            <q-card>
              <q-card-section class="q-pa-none">
                <div class="bg-primary text-h6 text-center text-white shadow-3 q-pa-md">Alarma</div>
              </q-card-section>
              <q-card-section>
                <q-form @submit="onSaveAlarm" class="q-pa-md q-gutter-sm row" >
                  <q-input :disable="lastAlarm" filled v-model="time" hint="Hora" mask="fulltime" 
                    :rules="['fulltime']" style="width: 120px">
                    <template v-slot:append>
                      <q-icon name="icon-alarm" class="cursor-pointer">
                        <q-popup-proxy transition-show="scale" transition-hide="scale">
                          <q-time v-model="time" format24h now-btn with-seconds>
                            <div class="row items-center justify-end">
                              <q-btn v-close-popup label="Aceptar" color="primary" flat></q-btn>
                            </div>
                          </q-time>
                        </q-popup-proxy>
                      </q-icon>
                    </template>
                  </q-input>
                  <q-input filled type="number" hint="Duracion (min)" 
                    lazy-rules :rules="[ val => val !== null && val !== '' || 'Esta mal...',
                      val => val > 0 && val < 180 || 'Use de 1 a 180min'
                    ]"   v-model="duration" style="width: 120px">
                  </q-input>
                  <q-select filled v-model="tap" :options="taps" option-label="name" hint="Grifo"  style="width: 120px" 
                    lazy-rules :rules="[val => val !== null && val !== '' || 'Selecciona un grifo']">
                  </q-select>
                  <div class="col-12">
                    <q-checkbox v-model="lastAlarm" label="Ultima Alarma" @input="onLastAlarm"></q-checkbox>
                    <q-btn flat label="Cancelar" v-close-popup class="text-primary"></q-btn>
                    <q-btn label="Guardar" type="submit"  class="bg-primary text-white"></q-btn>
                  </div>
            </q-form >
              </q-card-section>
            </q-card>
        </q-dialog>
      </q-page>`
}