<template>
  <q-page>
    <div class="col text-center">
      <b-chart :data="sensors" :title="$t('temp')" tag="te"></b-chart>
      <b-chart :data="sensors" :title="$t('press')" tag="pr"></b-chart>
      <b-chart :data="sensors" :title="$t('hum')" tag="hu"></b-chart>
    </div>
  </q-page>
</template>

<script>
import bChart from "src/components/charts/bChart.vue";

import { mapState } from "vuex";
export default {
  components: { bChart },
  name: "Charts",
  computed: {
    ...mapState(["sensors"]),
  },
};
</script>
