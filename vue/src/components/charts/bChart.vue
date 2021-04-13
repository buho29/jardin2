<template>
  <b-card :title="title">
    <b-line-chart style="height: 300px;"
      :data="data" :title="title"
      :tags="tags" :labels="labels"
      :format="format"
    />
  </b-card>
</template>

<script>
import BCard from 'src/components/bCard.vue';
import bLineChart from "./bLineChart.vue";
export default {
  components: { bLineChart, BCard },
  name: "b-chart",
  props: ["data", "tags", "title","labels","format"],
};
</script>