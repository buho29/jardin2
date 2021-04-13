<template>
    <q-form class="q-gutter-sm row" @submit="onSubmit" @reset="onReset">
      <q-input 
        type="number" filled label="Time zone" style="max-width: 110px;"
        min="-12" max="12"
        hint="Offset(hours)"
        :value="getTz()" v-model.number="tz"
        lazy-rules :rules="[
          val => (val !== null && val !== '') || 'Esta mal...',
          val => (val > -13 && val < 13) || 'Escribe entre -12 y +12'
        ]"
      ></q-input>

      <q-input filled label="Daylight saving time" style="flex-grow: 1;"
        hint="irregularity, startDay, startDay, endDay, endMonth, startHour, endHour"
        v-model="dst"
        mask=" # , # , ## , # , ## , # , #"
      >
        <template v-slot:append>
          <q-icon
            name="icon-help"
            @click="$window.open('https://github.com/kosme/flex_DST', '_blank')"
          />
        </template>
      </q-input>

      <b-btn-send />

    </q-form>
</template>

<script>

import { mapState, mapActions } from "vuex";
import BBtnSend from "../bBtnSend.vue";
import bContainer from "../bContainer.vue";
import mixinOption from '../mixin/mixinOption';

export default {
  components: { bContainer, BBtnSend },
  mixins: [mixinOption],
  name: "b-tz",
  data() {
    return {
      tz: 0,
      dst: "0, 0, 00, 0, 00, 0, 0 "
    };
  },
  computed: {
    //importamos datos
    ...mapState(["config"])
  },
  methods: {
    ...mapActions(["editConfig"]),
    onSubmit() {
      this.editConfig({ tz: this.tz, dst: this.dst });
    },
    onReset() {
      this.tz = null;
      this.dst = null;
    },
    getTz() {
      let t = this.config.tz;
      return t;
    },
    update(){
    // los datos cambiaron
      this.tz = this.config.tz;
      this.dst = this.config.dst;
    }

  }
};
</script>
