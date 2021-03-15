<template>
  <q-scroll-area horizontal visible style="height: 320px;">
    <g-gantt-chart
      :chart-start="start"
      :chart-end="end"
      :row-height="30"
      :locale="$t('localeMoment')"
      style="width: 1200px;"
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

const tapsBarConfig = { color: 'white', backgroundColor: '#00ffff', immobile: true }
const zoneBarConfig = { color: 'white', backgroundColor: '#ff00ff', immobile: true }
const pauseBarConfig = { color: 'white', backgroundColor: '#ffff00', immobile: true }
const sistemBarConfig = { color: 'white', backgroundColor: '#ffff00', immobile: true }

import moment from 'moment'
import { GGanttChart, GGanttRow } from 'vue-ganttastic'
import {mapState,mapGetters} from 'vuex';


export default {
  components: { GGanttChart, GGanttRow },
  name: 'b-time-line',
  props: ['data'],
  computed: {
    ...mapState(["history","taps"]), //importamos de vuex el array taps
    ...mapGetters(["getZoneByAlarmId","getTapById","getAlarmById","getZoneById"]),
  },
  watch: {
    history(newValue, oldValue) {
      this.update();
    }
  },
  mounted() {
    this.update();
  },
  methods: {
    update() {
      if(this.history.length > 1){
        this.start = this.getMoment(this.history[0].t);
        this.end = this.getMoment(this.history[this.history.length-1].t);
      }
      //create zone
      this.rows = [];
      for(let i = 0;i<this.taps.length;i++){
        this.rows.push({
          label: this.taps[i].name,
          bars: []
        })
      }
      this.rows.push({
        label: this.$t('titles.zone'),
        bars: []
      })
      this.rows.push({
        label: this.$t('history.paused'),
        bars: []
      })
      this.rows.push({
        label: this.$t('history.system'),
        bars: []
      })

      let st = null;
      let z = null;
      for(let i = 0;i<this.history.length;i++){

        let item = this.history[i];
        //tap
        if(item.a > -1 && item.a < 2)
        {
          let tap = this.getTapById(item.idi);
          if(tap){
            if(item.v) st = item;
            else if(st && st.idi === item.idi)
            {
              this.pushBars(item.idi, st.t,item.t,
                tap.name,tapsBarConfig);
            }
          }
        }// zone
        else if(item.a > 1 && item.a < 4)
        {
          let a = this.getAlarmById(item.idi)
          if(a){
            let tmp = this.getZoneByAlarmId(item.idi);
            if(item.v) {
              z = tmp;
              st = item;
            }
            else if( z  && z.id === tmp.id && st)
            {
              this.pushBars(this.taps.length, st.t,item.t,
                  z.name,zoneBarConfig);
                  
              console.log('zone',  tmp.name, z.id , tmp.id);
            }

          }//else str ="zone not found " + this.formatTime(item.idi) ;
        }// pause
        else if(item.a === 4)
        {
          z = this.getZoneById(item.idi);
          if(z){
            if(item.v) st = item;
            else if(st && st.idi === item.idi)
            {
              this.pushBars(this.taps.length+1, st.t,item.t,
                "pause",pauseBarConfig);
            }
            console.log('pause');
          }
        }//first run sistem
        else if(item.a === 5)
        {
          this.pushBars(this.taps.length+2, item.t,item.t+1800,
            "system",sistemBarConfig);
          console.log('system');
        }else
          console.log("meecc")
      }
    },
    getMoment(time){
      return moment(time*1000).utc().format('YYYY-MM-DD HH:mm:ss') ;
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
