<template>
  <q-page>
    <b-container :title="$t('titles.taps')">
      <q-list>
        <q-item class="bg-white" clickable v-ripple
          v-for="(item, index) in taps" :key="index"
        >
          <q-item-section avatar>
            <q-icon color="primary" name="icon-water-pump"></q-icon>
          </q-item-section>
          <q-item-section> {{ item.name }}</q-item-section>
          <q-item-section side>
            <q-toggle
              :value="item.open"
              @input="openTap({ id: item.id, value: !item.open })"
            ></q-toggle>
          </q-item-section>
        </q-item>
      </q-list>
    </b-container>
  </q-page>
</template>

<script>
import bContainer from 'src/components/bContainer.vue';
import { mapActions, mapState } from "vuex";
export default {
  components: { bContainer },
  name :"Taps",
  computed: {
    ...mapState(["taps"]), //importamos de vuex el array taps
  },
  methods: {
    ...mapActions(["openTap"]), // importamos openTap
  },
};
</script>