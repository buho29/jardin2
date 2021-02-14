const zone = {
    mixins: [mixinFormat, mixinRequiresAuth],
    data: function () {
        return {
            zoneId: -1,
            fab: false,
            showDialog: false,
            name: null,
            lastAlarm: false,
            selectedAlarmId: -1,
            time: '10:15:00',
            duration: 10,
            tap: null,
            alarmsZone: [],
            modesSelecteds: [],
            modesData: [
                { label: 'Desactivar', value: 1 }, { label: 'Meteorologia', value: 2 },
                { label: 'Sensores', value: 3 }, { label: 'Lunes', value: 4 },
                { label: 'Martes', value: 5 }, { label: 'Miercoles', value: 6 },
                { label: 'Jueves', value: 7 }, { label: 'Viernes', value: 8 },
                { label: 'Sabado', value: 9 }, { label: 'Domingo', value: 10 }
            ]
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
            console.log(newLoaded);
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
            this.modesSelecteds = 0;
            this.alarmsZone = [];
        }
        next();
    },
    methods: {
        ...Vuex.mapActions(
            ['deleteAlarm', 'editZone', 'editAlarm', 'addZone', 'addAlarm',]
        ),
        onSaveZone() {
            let error = false;
            if (this.zoneId >= 0) {
                this.editZone({
                    id: this.zoneId,
                    name: this.name,
                    modes: 0//this.modesSelecteds
                });
            } else {
                this.addZone({
                    name: this.name,
                    modes: 0//this.modesSelecteds
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
                    this.modesSelecteds = zone.modes;
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
              <div  class="text-h6 q-pa-sm">Nombre</div>
          </div>
          
          <q-form @submit="onSaveZone" class="q-gutter-md q-pa-md" >
    
              <q-input dense filled v-model="name" label="Nombre *"
                lazy-rules :rules="[ val => val && val.length > 0 || 'Por favor escriba algo']">
              </q-input>
              
                <q-select dense filled v-model="modesSelecteds" label="Modos *" required
                  :options="modesData" multiple emit-value map-options>
                  <template v-slot:option="scope">
                    <q-item v-bind="scope.itemProps" v-on="scope.itemEvents">
                      <q-item-section>
                        <q-item-label v-html="scope.opt.label" ></q-item-label>
                      </q-item-section>
                      <q-item-section side>
                        <q-toggle v-model="modesSelecteds" :val="scope.opt.value" />
                      </q-item-section>
                    </q-item>
                  </template>
                </q-select>
    
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