
<template>
  <q-card class="q-ma-md">
    <q-card-section class="q-pa-none q-ma-none">
      <div class="bg-primary text-white shadow-3">
        <div class="text-h6 q-pa-sm">{{ title }}</div>
      </div>
    </q-card-section>
    <q-card-section>
      <b-line-chart style="height: 300px;"
        :data="data"
        :title="title"
        :tag="tag"
      ></b-line-chart>
    </q-card-section>
  </q-card>
</template>

<script>
import bLineChart from "./bLineChart.vue";
export default {
  components: { bLineChart },
  name: "b-chart",
  props: ["data", "tag", "title"],
};
</script>