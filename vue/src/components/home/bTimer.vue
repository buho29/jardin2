<template>
  <div class="text-center q-pa-sm">
    <div class="text-primary text-h5">{{ currentDate }}</div>
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
      localTime: new Date().toLocaleTimeString("es-ES"),
      currentDate: new Date().toLocaleDateString("es-ES"),
      diff: 0,
      serverTime: 0,
    };
  },
  props: ["weather", "local"],
  mounted() {
    this.id = setInterval(this.tick, 1000);
  },
  beforeDestroy() {
    clearInterval(this.id);
  },
  methods: {
    tick() {
      let date = new Date(Date.now() - this.weather.diff);
      this.localTime = new Date().toLocaleTimeString(this.local);
      this.serverTime = date.toLocaleTimeString(this.local);
      this.currentDate = date.toLocaleDateString(this.local);
    },
  },
};
</script>