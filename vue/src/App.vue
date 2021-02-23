<template>
  <div id="q-app">
    <router-view />
  </div>
</template>
<script>
export default {
  name: 'App',
  mounted() {
    this.$q.iconSet.field.error = 'icon-alert-circle';
    this.$q.iconSet.arrow.dropdown = 'icon-menu-down';
    this.$q.iconSet.datetime.now = 'icon-alarm';
    console.log("teata");
  },
}
</script>
