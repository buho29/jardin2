<template>
  <q-page>
    <q-carousel swipeable animated infinite vertical
      v-model="slide" ref="carousel" @input="rewritePath"
      class="text-center bg-amber-1" style="position: relative"
      height="94vh"
      transition-prev="slide-down" transition-next="slide-up"
    >
      <q-carousel-slide v-for="item in zones" :key="item.id" :name="item.id">
        <b-zone :data="item"></b-zone>
      </q-carousel-slide>

      <template v-slot:control v-if="zones.length > 0">
        <q-carousel-control position="bottom" :offset="[0, 30]">
          <q-btn flat round dense icon="icon-chevron-down"
            text-color="black"
            @click="$refs.carousel.previous()"
          />
        </q-carousel-control>

        <q-carousel-control position="top" :offset="[0, 30]">
          <q-btn flat round dense icon="icon-chevron-up"
            text-color="black"
            @click="$refs.carousel.next()"
          />
        </q-carousel-control>
      </template>
    </q-carousel>  
      <q-page-sticky position="bottom-right" :offset="[18, 18]">
        <q-btn round icon="icon-plus" color="accent" class="text-primary" 
          :to="{name:'zone',params:{id:-1}}"
        />
      </q-page-sticky>
  </q-page>
</template>

<script>
import BZone from 'src/components/zones/bZone.vue';

import {mapState,mapGetters} from 'vuex'
export default {
  components: { BZone },
    name :"Zones",
    data() {
        return {
            slide: false
        }
    },
    computed: {
        ...mapState(['zones']),
        ...mapGetters(['getZoneById']),
    },
    watch: {
        zones: function (newLoaded, oldLoaded) {
            this.update();
        }
    },
    beforeRouteUpdate(to, from, next) {
        let id = parseInt(to.params.id);
        //nueva zona
        if (id < 0 || this.getZoneById(id) === undefined) {
            next(false);
        }
        next(true);
    },
    methods: {
        rewritePath(newVal, oldVal) {
            let id = parseInt(this.$route.params.id);
  
            if (id !== newVal) {
                this.$router.replace({ path: `/zones/${newVal}` });
            }
        },
        update() {
            let id = parseInt(this.$route.params.id);
            let zone = this.getZoneById(id);
            if (id >= 0 && zone !== undefined)
                this.$refs.carousel.goTo(id);
            else this.$refs.carousel.next();
        }
    },
    mounted() {
        this.update();
    },
}
</script>