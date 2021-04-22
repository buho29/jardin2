<template>
  <div class="q-pa-sm">
    <div class="text-primary text-h4">{{ currentDate }}</div>
    <div class="text-primary text-h6">{{ dataString }}</div>
    <div class="q-mx-auto">
    <div class="text-h5 q-ma-none">
      {{ localTime }}<span class="text-body1 text-blue-5"> Local</span>
    </div>
    <div class="text-h5 q-ma-none">
      {{ serverTime }}<span class="text-body1 text-blue-5"> Esp32</span>
    </div></div>
  </div>
</template>
<script>

import moment from 'moment'
export default {
  name: "b-timer",
  data() {
    return {
      local: this.$t('localeMoment'),
      date: moment().locale(this.$t('localeMoment')),
      localTime: moment().locale(this.$t('localeMoment')).format('LTS'),
    };
  },
  props: ["time"],
  mounted() {
    this.id = setInterval(this.tick, 1000);
  },
  beforeDestroy() {
    clearInterval(this.id);
  },
  computed:{
    serverTime(){
      return this.date.format('LTS')//this.date.toLocaleTimeString(this.local);
    },
    currentDate(){
      return this.date.format('L');
    },
    dataString(){
      return this.date.format('dddd, DD MMMM');
    }
  },
  methods: {
    tick() {
      this.date = moment().utc(this.time*1000).locale(this.local);
      this.localTime = moment().locale(this.local).format('LTS');
    },
  },
};
</script>