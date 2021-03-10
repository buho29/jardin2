<template>
  <q-page>
    <b-container :title="$t('titles.history')" class="text-left" >
      <p class="bg-white"
        v-for="(item, index) in history" :key="index">
        {{ formatDate(item.t) +' '+ formatTime(item.t) }} - 
        <span :class="getClass(item.v)"> {{getStringAction(item)}}</span>
      </p>
    </b-container>
  </q-page>
</template>

<script>
//{"t":1615040722,"v":1,"a":0,"idi":0}
import mixinFormat from 'src/components/mixin/mixinFormat';
import {mapState,mapGetters} from 'vuex';
import bContainer from 'src/components/bContainer.vue';
export default {
  components: { bContainer },
  name :"History",
  mixins: [mixinFormat],
  computed: {
    ...mapState(["history"]), //importamos de vuex el array taps
    ...mapGetters(["getZoneByAlarmId","getTapById"]),
  },
  methods: {
    getStringAction(item){
      let str= "";
      let tmp;

      // tap
      if(item.a > -1 && item.a < 2)
      {
        let tap = this.getTapById(item.idi);
        if(tap){
          tmp = item.v? 
            this.$t('stats.open',[tap.name]) :
            this.$t('stats.close',[tap.name]);
            str += this.$t('actions.'+item.a,[tmp]);
        }else str = "tap not found "+item.idi;
      }// zone
      else if(item.a > 1 && item.a < 4)
      {
        let z = this.getZoneByAlarmId(item.idi);
        if(z){
          tmp = this.$t('actions.'+item.a,[z.name]);
          str += item.v? 
            this.$t('stats.open',[tmp]) :
            this.$t('stats.close',[tmp]);
        }else str ="zone not found " + this.formatTime(item.idi) ;
      }// pause
      else if(item.a === 4)
      {
        tmp = this.$t('actions.'+item.a,['']);
        str += item.v? 
          this.$t('stats.pause',[tmp]) :
          this.$t('stats.unpause',[tmp]);
      }//first run sistem
      else if(item.a === 5)
      {
        str += this.$t('actions.'+item.a,['']);
      }else
        console.log("meecc")
      
      return str;
    },
    getClass(bool){
      if(bool) return 'text-primary';
      else return 'text-red';
    },
    formatDate(time){
      let date = new Date(time*1000);
      return date.toLocaleDateString(this.$t('localeTime'));
    }
  },
};
</script>