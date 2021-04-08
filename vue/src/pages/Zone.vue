<template>
  <q-page class="q-pa-none">
    <div class="q-mt-lg q-mx-auto text-center bg-white"
      style="max-width: 400px">
      <div class="bg-primary text-white shadow-3">
        <div class="text-h6 q-pa-sm" v-if="zoneId > -1" >{{ name }}</div>
        <div class="text-h6 q-pa-sm" v-else >{{ $t('new') }}</div>
      </div>

      <q-form @submit="onSaveZone" class="q-gutter-md q-pa-md">
        <div class="text-h6 text-blue-5" v-if="can_watering && zoneId > -1">
          {{$t('zones.todayWatering')}}
        </div>
        <div class="text-h6 text-red-5" v-else>
           {{$t('zones.todayNotWatering')}}
        </div>
        <q-input dense filled
          v-model="name" :label="$t('name')"
          lazy-rules :rules="[
            (val) => (val && val.length > 0) || 'Por favor escriba algo',
          ]"
        >
        </q-input>
        <b-select-flags
          :label="$t('zone.modes')"
          :options="modesData"
          :flags="modesFlags"
          @input="change"
        />
        <div class="col-12 text-right">
          <q-btn flat class="text-primary"
            :label="$t('cancel')"
            @click="$router.go(-1)"
          ></q-btn>
          <q-btn
            :label="$t('save')"
            type="submit"
            class="bg-primary text-white"
          ></q-btn>
        </div>
      </q-form>

      <div class="bg-primary text-white shadow-3 row">
        <div class="text-h6 q-pa-sm col">{{$t('zone.alarms')}}</div>

        <q-fab flat dense class="col-auto" icon="icon-cog"
          active-icon="icon-cog" direction="left" padding="sm"
          v-model="fab" v-if="zoneId > -1"
        >
          <q-fab-action
            padding="5px"
            color="warning"
            icon="icon-delete"
            external-label
            label-position="bottom"
            v-if="selectedAlarmId > -1"
            @click="deleteAlarm(selectedAlarmId)"
            :label="$t('delete')"
          />

          <q-fab-action
            v-if="selectedAlarmId > -1"
            padding="5px"
            color="accent"
            label-position="bottom"
            external-label
            @click="showDialogAlarm(selectedAlarmId)"
            icon="icon-pencil"
            :label="$t('edit')"
          />

          <q-fab-action
            padding="5px"
            external-label
            label-position="bottom"
            class="text-dark bg-white"
            @click="showDialogAlarm(-1)"
            icon="icon-plus"
            :label="$t('new')"
          />
        </q-fab>
      </div>

      <q-list bordered>
        <q-item
          clickable
          v-ripple
          v-for="alarm in alarmsZone"
          :key="alarm.id"
          :active="selectedAlarmId === alarm.id"
          @click="selectedAlarmId = alarm.id"
          active-class="my-menu-link"
        >
          <q-item-section avatar>
            <q-icon color="primary" name="icon-alarm" />
          </q-item-section>

          <q-item-section>
            {{ taps[alarm.tapId].name }} - {{ formatTime(alarm.time) }} -
            {{ alarm.duration /60 }} min
          </q-item-section>
        </q-item>
      </q-list>
    </div>
    <q-dialog v-model="showDialog">
      <q-card>
        <q-card-section class="q-pa-none">
          <div
            class="bg-primary text-h6 text-center text-white shadow-3 q-pa-md">
            {{$t('zone.alarm')}}
          </div>
        </q-card-section>
        <q-card-section>
          <q-form @submit="onSaveAlarm" class="q-pa-md q-gutter-sm row">
            <q-input filled style="width: 120px" 
              v-model="time" :disable="lastAlarm"
              mask="fulltime" :rules="['fulltime']"
              :hint="$t('zone.time')">

              <template v-slot:append>
                <q-icon name="icon-alarm" class="cursor-pointer">
                  <q-popup-proxy
                    transition-show="scale"
                    transition-hide="scale" >
                    <q-time v-model="time" format24h now-btn with-seconds>
                      <div class="row items-center justify-end">
                        <q-btn color="primary" flat
                          v-close-popup :label="$t('accept')"
                        ></q-btn>
                      </div>
                    </q-time>
                  </q-popup-proxy>
                </q-icon>
              </template>
            </q-input>

            <q-input filled type="number" style="width: 120px"
              :hint="$t('zone.duration')"
              lazy-rules :rules="[
                (val) => (val !== null && val !== '') || $t('zone.durationErrors[0]'),
                (val) => (val > 0 && val < 180) ||  $t('zone.durationErrors[1]'),
              ]"
              v-model="duration">
            </q-input>

            <q-select filled style="width: 120px"
              v-model="tap" :options="taps" option-label="name"
              :hint="$t('zone.tap')"
              lazy-rules :rules="[
                (val) => (val !== null && val !== '') || $t('zone.tapErrors'),
              ]">
            </q-select>

            <div class="col-12">
              <q-checkbox
                v-model="lastAlarm"
                :label="$t('zone.lastAlarm')"
                @input="onLastAlarm"
              ></q-checkbox>
              <q-btn flat class="text-primary"
                :label="$t('cancel')"
                v-close-popup
              ></q-btn>
              <q-btn class="bg-primary text-white"
                :label="$t('save')"
                type="submit"
              ></q-btn>
            </div>
          </q-form>
        </q-card-section>
      </q-card>
    </q-dialog>
  </q-page>
</template>

<script>

import {mapActions,mapState,mapGetters} from 'vuex'

import mixinFormat from 'src/components/mixin/mixinFormat';
import mixinRequiresAuth from 'src/components/mixin/mixinRequiresAuth';
import bSelectFlags from 'src/components/zone/bSelectFlags.vue';

export default {
  components: { bSelectFlags },
    name :"Zone",
    mixins: [mixinFormat, mixinRequiresAuth],
    data() {
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
                { label: this.$t('modes[0]'), value: 0b0000000001},
                { label: this.$t('modes[1]'), value: 0b0000000010},
                { label: this.$t('modes[2]'), value: 0b0000000100},
                { label: this.$t('modes[3]'), value: 0b0000001000},
                { label: this.$t('modes[4]'), value: 0b0000010000},
                { label: this.$t('modes[5]'), value: 0b0000100000},
                { label: this.$t('modes[6]'), value: 0b0001000000},
                { label: this.$t('modes[7]'), value: 0b0010000000},
                { label: this.$t('modes[8]'), value: 0b0100000000},
                { label: this.$t('modes[9]'), value: 0b1000000000}
            ],
        }
    },
    computed: {
        ...mapState(['taps', 'zones', 'isConnected', 'alarms'] ),
        ...mapGetters( ['getAllAlarms', 'getZoneById', 'getZoneByName',
          'getAlarmById', 'getLastAlarm', 'getTapById']
        ),
    },
    methods: {
        change(flags){
          this.modesFlags = flags;
          console.log("change",this.modesFlags);
        },
        ...mapActions(['deleteAlarm', 'editZone', 'editAlarm', 'addZone', 'addAlarm']),
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
                    tapId: this.tap.id, time: time, duration: this.duration*60
                });
                this.showDialog = false;
            } else {
                this.addAlarm({
                    zoneId: this.zoneId, tapId: this.tap.id,
                    time: time, duration: this.duration*60
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
    mounted() {
        this.update();
        this.alarmsZone = this.getAllAlarms(this.zoneId);
    },
}
</script>
