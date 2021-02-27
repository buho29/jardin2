<template>
  <q-card class="text-center my-card">
    <q-card-section class="q-pa-none q-ma-none">
      <div class="bg-primary row text-white shadow-3">
        <div class="col text-h6 q-pa-sm">{{ data.name }}</div>

        <q-fab
          v-model="fab"
          v-if="authenticate"
          flat
          dense
          class="col-auto"
          icon="icon-cog"
          active-icon="icon-cog"
          direction="down"
          padding="xs"
        >
          <q-fab-action
            padding="5px"
            external-label
            label-position="left"
            color="warning"
            icon="icon-delete"
            @click="onDelete"
            :label="$t('delete')"
          />

          <q-fab-action
            padding="5px"
            icon="icon-pencil"
            :label="$t('edit')"
            :to="{ name: 'zone', params: { id: data.id } }"
            external-label
            label-position="left"
            color="accent"
          />
        </q-fab>
      </div>
    </q-card-section>

    <q-card-section style="height: 180px">
      <div class="text-h6 q-pa-sm text-blue-5" v-if="data.can_watering">
        {{$t('zones.todayWatering')}}
      </div>
      <div class="text-h6 q-pa-sm text-red-5" v-else>
        {{$t('zones.todayNotWatering')}}
      </div>
      <!-- parado  -->
      <div v-if="!data.runing" class="text-center">
        <div class="text-h3 q-ma-none">
          {{ formatTime(data.time) }}
        </div>
        <div class="text-h6 text-blue-5">
          {{ getAllAlarmsSize(data.id) }} - {{ formatTime(data.duration) }}
        </div>
      </div>
      <!-- rulando -->
      <div v-else class="text-center text-h6 q-ma-none q-pa-none">
        <div class="text-h3">
          {{ formatTime(data.elapsed) }}
        </div>
        <div class="text-blue-5">
          {{ getAlarmIndex(data.alarmId) + 1 }} /
          {{ getAllAlarmsSize(data.id) }} -
          {{ formatTime(getAlarmById(data.alarmId).duration - data.elapsed) }}
        </div>
        <div class="text-blue-5 text-h6">
          {{ formatTime(data.duration) }}
        </div>
      </div>
    </q-card-section>

    <q-card-actions align="around">
      <b-toggle
        :checked="data.paused"
        @change="onPause"
        :disable="!data.runing"
        :label-off="$t('zones.resume')"
        :label-on="$t('zones.pause')"
      ></b-toggle>
      <b-toggle
        :checked="data.runing"
        @change="onWater"
        :label-off="$t('cancel')"
        :label-on="$t('zones.water')"
      ></b-toggle>
    </q-card-actions>
  </q-card>
</template>
<script>
import { mapActions, mapMutations, mapState, mapGetters } from "vuex";
import mixinFormat from "../mixin/mixinFormat";
import bToggle  from "../bToggle";

export default {
  name: "b-zone",
  components: { bToggle },
  mixins: [mixinFormat],
  data() {
    return {
      fab: false
    };
  },

  props: { data: Object },
  computed: {
    ...mapState(["zones", "authenticate"]),
    ...mapGetters(["getAllAlarmsSize", "getAlarmIndex", "getAlarmById"])
  },
  methods: {
    ...mapMutations(["addZone", "editZone"]),
    ...mapActions(["waterZone", "pauseZone", "deleteZone"]),
    onDelete() {
      this.deleteZone(this.data.id);
    },
    onWater() {
      this.waterZone({ id: this.data.id, value: !this.data.runing });
    },
    onPause() {
      this.pauseZone({ id: this.data.id, value: !this.data.paused });
    }
  }
};
</script>
