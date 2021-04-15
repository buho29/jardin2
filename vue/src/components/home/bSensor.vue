<template>
  <q-card class="text-subtitle1">
    <q-card-section class="q-pa-none text-primary" style="min-width:100px;">
      <div class="q-pa-sm">{{ prop }}</div>
      <q-separator />
    </q-card-section>
    <q-card-section>
      <div class="text-dark">{{ value }}</div>
    </q-card-section>
  </q-card>
</template>

<script>
export default {
  name: "b-sensor",
  props: ["prop", "value"],
};
</script>
