<template>
  <div class="q-pa-sm">
    <div class="text-primary text-h4">{{ currentDate }}</div>
    <div class="text-primary text-h6">{{ dataString }}</div>
    <div class="text-h5 q-ma-none">
      {{ localTime }}<span class="text-body1 text-blue-5"> Local</span>
    </div>
    <div class="text-h5 q-ma-none">
      {{ serverTime }}<span class="text-body1 text-blue-5"> Esp32</span>
    </div>
  </div>
</template>
<script>
export default {
  name: "b-timer",
  data() {
    return {
      date: new Date(),
      localTime: new Date().toLocaleTimeString(this.$t('localeTime')),
      local: this.$t('localeTime'),
    };
  },
  props: ["diff"],
  mounted() {
    this.id = setInterval(this.tick, 1000);
  },
  beforeDestroy() {
    clearInterval(this.id);
  },
  computed:{
    serverTime(){
      return this.date.toLocaleTimeString(this.local);
    },
    currentDate(){
      return this.date.toLocaleDateString(this.local);
    },
    dataString(){
      return this.$t('date.days.'+this.date.getDay())+", "+
        this.date.getDate()+ ' '+
        this.$t('date.months.'+this.date.getMonth());
    }
  },
  methods: {
    tick() {
      this.date = new Date(Date.now() - this.diff);
      this.localTime = new Date().toLocaleTimeString(this.local);
    },
  },
};
</script>