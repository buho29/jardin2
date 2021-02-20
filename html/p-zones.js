
export default {
    data: function () {
        return {
            slide: false
        }
    },
    computed: {
        ...Vuex.mapState(['zones']),
        ...Vuex.mapGetters(['getZoneById']),
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
    //
    template: /*html */`
    <q-page>
        <q-carousel swipeable animated v-model="slide" ref="carousel" infinite 
          @input="rewritePath"
          vertical class="text-center bg-amber-1" height="94vh" style="position: relative;"  
          transition-prev="slide-up" transition-next="slide-down">
    
          <q-carousel-slide v-for="item in zones" :key="item.id" :name="item.id">
            <b-zone :data="item"></b-zone>
          </q-carousel-slide>
    
          <template v-slot:control v-if="zones.length>0">
            <q-carousel-control position="bottom" :offset="[0, 30]" >
              <q-btn flat round dense icon="icon-chevron-down" text-color="black" 
                @click="$refs.carousel.next()"></q-btn>
            </q-carousel-control>
    
            <q-carousel-control position="top" :offset="[0, 30]">
              <q-btn flat round dense icon="icon-chevron-up" text-color="black" 
                @click="$refs.carousel.previous()"></q-btn>
            </q-carousel-control>
          </template>
  
        </q-carousel>
    </q-page>`
  }
  
  Vue.component('b-zone', {
    data: function () {
        return {
            fab: false,
        }
    },
    mixins: [mixinFormat],
    props: { data: Object },
    computed: {
        ...Vuex.mapState(['zones', 'authenticate']),
        ...Vuex.mapGetters(['getAllAlarmsSize', 'getAlarmIndex', 'getAlarmById']),
    },
    methods: {
        ...Vuex.mapMutations(['addZone', 'editZone']),
        ...Vuex.mapActions(['waterZone', 'pauseZone', 'deleteZone']),
        onDelete() {
            this.deleteZone(this.data.id);
        },
        onWater() {
            this.waterZone({ id: this.data.id, value: !this.data.runing });
        },
        onPause() {
            this.pauseZone({ id: this.data.id, value: !this.data.paused });
        }
    },
    template: /*html*/`
    
    <q-card class=" text-center my-card">  
      
      <q-card-section class="q-pa-none q-ma-none">
        <div class="bg-primary row text-white shadow-3">
        
          <div  class="col text-h6 q-pa-sm">{{data.name}}</div>
          
          <q-fab v-model="fab" v-if="authenticate"
            flat dense class="col-auto" icon="icon-cog" active-icon="icon-cog"
            direction="down" padding="xs">
    
            <q-fab-action padding="5px" external-label label-position="left" color="warning" 
              icon="icon-delete" @click="onDelete" label="Borrar" />
    
            <q-fab-action padding="5px" icon="icon-pencil" label="Editar" 
              :to="{name:'zone',params:{id:data.id}}" external-label label-position="left" color="accent" 
            />
            
          </q-fab>
  
        </div>
      </q-card-section>
      
      <q-card-section style="height: 180px;">
        
        <div  class="text-h6 q-pa-sm text-blue-5 " v-if="data.can_watering">Hoy se riega</div>
        <div  class="text-h6 q-pa-sm text-red-5" v-else>Hoy no se riega </div>
        <!-- parado  -->
        <div v-if="!data.runing" class="text-center">
          <div class="text-h3 q-ma-none">
            {{formatTime(data.time)}}
          </div> 
          <div class="text-h6 text-blue-5">
            {{getAllAlarmsSize(data.id)}} - {{formatTime(data.duration)}}
          </div>
        </div>
        <!-- rulando -->
        <div v-else="data.runing" class="text-center text-h6 q-ma-none q-pa-none">
          <div class="text-h3">
            {{formatTime(data.elapsed)}}
          </div> 
          <div class="text-blue-5">
            {{ (getAlarmIndex(data.alarmId) + 1) }} / {{getAllAlarmsSize(data.id)}} - 
            {{formatTime(getAlarmById(data.alarmId).duration - data.elapsed)}}
          </div>
          <div class="text-blue-5 text-h6">
            {{formatTime(data.duration)}}
          </div>
        </div>
      </q-card-section>
    
      <q-card-actions align="around">
        <b-toggle :checked="data.paused"  @change="onPause" 
          :disable="!data.runing" label-off="Reanudar" label-on="Pausar"></b-toggle>
        <b-toggle :checked="data.runing" @change="onWater" 
          label-off="Cancelar" label-on="Regar"></b-toggle>
      </q-card-actions>
      
    </q-card>
    `
  })