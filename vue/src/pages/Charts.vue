<template>
  <q-page>
      <q-card class="q-ma-lg q-mx-auto text-center items-center bg-white"
        style="min-width: 300px; max-width: 600px; width: 90%">
        
        <q-tabs
          class="bg-primary text-white shadow-1" dense
          indicator-color="accent" align="justify" narrow-indicator
          v-model="tab"
        >
          <q-tab name="1H" label="1H"/>
          <q-tab name="24H" label="24H"/>
        </q-tabs>

        <q-separator/>

        
        <q-tab-panels v-model="tab" animated keep-alive>
          
          <q-tab-panel name="1H" class="q-gutter-y-md">
            <b-chart :data="sensors" :title="$t('temp')" 
            :labels="[$t('temp')]" :tags="['te']" format="HH:mm:ss"/>
            <b-chart :data="sensors" :title="$t('press')"  format="HH:mm:ss"
            :labels="[$t('press')]" :tags="['pr']"/>
            <b-chart :data="sensors" :title="$t('hum')"  format="HH:mm:ss" 
            :labels="[$t('hum')]" :tags="['hu']"/>            
          </q-tab-panel>
          <q-tab-panel name="24H" class="q-gutter-y-md">
            <b-chart :data="sensors24" :title="$t('temp')"  format="DD-MM"
            :labels="['min','avg','max']" :tags="['mite', 'avte', 'mate']"/>
            <b-chart :data="sensors24" :title="$t('hum')" format="DD-MM"
            :labels="['min','avg','max']"  :tags="['mihu','avhu','mahu']"/>         
          </q-tab-panel>

        </q-tab-panels>

      </q-card>
  </q-page>
</template>

<script>
import bChart from "src/components/charts/bChart.vue";

import { mapState } from "vuex";
export default {
  components: { bChart },
  name: "Charts",
  data() {
    return {
      tab:"1H",
    }
  },
  computed: {
    ...mapState(["sensors","sensors24"]),
  },
};
</script>
