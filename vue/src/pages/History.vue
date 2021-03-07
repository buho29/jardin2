<template>
  <q-page>
    <div >
      <p class="bg-white"
        v-for="(item, index) in history"
        :key="index"
      >
      {{ formatTime(item.t) }} - {{getStringAction(item)}}
      </p>
    </div>
  </q-page>
</template>

<script>
//{"t":1615040722,"v":1,"a":0,"idi":0}
import mixinFormat from 'src/components/mixin/mixinFormat';
import {mapActions, mapMutations,mapState,mapGetters} from 'vuex';
export default {
  name :"History",
  mixins: [mixinFormat],
  computed: {
    ...mapState(["history"]), //importamos de vuex el array taps
    ...mapGetters(["getZoneByAlarmId","getTapById"]),
  },
  methods: {
    getStringAction(item){
      let str= "";
      let z;
      let tmp;

      switch (item.a) {
        case 0:
          let tap = this.getTapById(item.idi);
          if(tap){
            str += item.v? 
              this.$t('stats.open',[tap.name]) :
              this.$t('stats.close',[tap.name])
          }
          break;
        case 1:
          str += this.$t('actions.'+item.a);
          break;
        case 2:
          z = this.getZoneByAlarmId(item.idi);
          if(z){
            tmp = this.$t('actions.'+item.a,[z.name]);
            str += item.v? 
              this.$t('stats.open',[tmp]) :
              this.$t('stats.close',[tmp]);
          }
          break;
        case 3:
          z = this.getZoneByAlarmId(item.idi);
          if(z){
            str += this.$t('actions.'+item.a,[z.name]);
          }
          break;
        case 4:
          str += this.$t('stats.pause',[item.v]);
          break;
      
      }
      
      
      return str;
    }
  },
};
</script>