<template>
  <q-scroll-area horizontal visible 
    class="q-pa-ml shadow-2"
    style="height: 280px;border: 1px solid gray;">
    <g-gantt-chart
      :chart-start="start"
      :chart-end="end"
      :row-height="30"
      grid
      :locale="$t('localeMoment')"
      style="width: 1200px;height: 280px;"
    >
      <g-gantt-row
        v-for="row in rows"
        :key="row.label"
        :label="row.label"
        :bars="row.bars"
        bar-start="start"
        bar-end="end"
      >
        <template #bar-label="{bar}">
          <span>{{bar.label}}</span>
        </template>
      </g-gantt-row>
    </g-gantt-chart>
  </q-scroll-area>
</template>

<script>

const tapsBarConfig = { color: 'white', backgroundColor: '#00ffff', immobile: true,borderRadius: 0 }
const zoneBarConfig = { color: 'white', backgroundColor: '#ff00ff', immobile: true,borderRadius: 0 }

import moment from 'moment'
import {mapState,mapGetters} from 'vuex';
import GGanttChart from '../ganttastic/GGanttChart.vue';
import GGanttRow from '../ganttastic/GGanttRow.vue';


export default {
  components: {GGanttChart, GGanttRow },
  name: 'b-time-line',
  props: ['data'],
  computed: {
    ...mapState(["alarms","taps"]), //importamos de vuex el array taps
    ...mapGetters(["getZoneByAlarmId","getTapById","getAlarmById","getZoneById"]),
  },
  watch: {
    alarms(newValue, oldValue) {
      this.update();
    }
  },
  mounted() {
    this.update();
  },
  methods: {
    update() {
      
      if(this.alarms.length > 1){
        this.start = this.getMoment(this.alarms[0].time);
        let last = this.alarms[this.alarms.length-1];
        this.end = this.getMoment(last.time+last.duration+3600);
        console.log(this.end+"sss");
      }
      //create taps
      this.rows = [];
      for(let i = 0;i<this.taps.length;i++){
        this.rows.push({
          label: this.taps[i].name,
          bars: []
        })
      }
      //create zone
      this.rows.push({
        label: this.$t('titles.zone'),
        bars: []
      })

      //taps
      let st = undefined;
      for(let i = 0;i<this.alarms.length;i++){

        let item = this.alarms[i];
        //tap
        let tap = this.getTapById(item.tapId);
        if(tap){

          this.pushBars(item.tapId, item.time,item.time + item.duration,
              tap.name,tapsBarConfig);
        }
        
        // zone
        let z = item.zoneId;
        let next = i<this.alarms.length ? this.alarms[i+1] :undefined;
        console.log(next);
        if(st===undefined){
            st = item;
        }
        
        if(next !== undefined && z !== next.zoneId){
            let tmp = this.getZoneByAlarmId(item.id);
            console.log('zone',  tmp.name, z);
            this.pushBars(this.taps.length, st.time,item.time+item.duration,
              tmp.name,zoneBarConfig);
            st = undefined;
        }else if(next === undefined){
            let tmp = this.getZoneByAlarmId(item.id);
            console.log('zone',  tmp.name, z);
            this.pushBars(this.taps.length, st.time,item.time+item.duration,
              tmp.name,zoneBarConfig);
        }
      }
    },
    getMoment(time){
      let ti = this.makeDate(time);
      return moment({ hour: ti.h, minute: ti.m, seconds: ti.s }).format('YYYY-MM-DD HH:mm:ss') ;
    },
    makeDate(utc) {
      let s = utc % 60;
      utc = Math.floor(utc / 60);
      let m = utc % 60;
      utc = Math.floor(utc / 60);
      let h = utc % 24;

      return {h:h,m:m,s: s};
    },
    pushBars(id,st,en,name,config){
      this.rows[id].bars.push({
        start: this.getMoment(st),
        end: this.getMoment(en),
        label: name,
        ganttBarConfig: config
      })
    }
  },
  data () {
    return {
      start: '2020-03-01 00:00',
      end: '2020-03-03 00:00',
      rows: []
    }
  }
}
</script>
