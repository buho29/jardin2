export default {
    computed: {
        ...Vuex.mapState(['taps'])//importamos de vuex el array taps
    },
    methods: {
        ...Vuex.mapActions(['openTap'])// importamos openTap
    },
    template: /*html*/`
    <q-page>
      <q-list bordered>
        <q-item class="bg-white" clickable v-ripple 
            v-for="(item,index) in taps" :key="index">
          <q-item-section avatar>
            <q-icon color="primary" name="icon-water-pump"></q-icon>
          </q-item-section>
          <q-item-section> {{item.name}}</q-item-section>
          <q-item-section side >
            <q-toggle :value="item.open" 
              @input="openTap({id:item.id,value:!item.open})"></q-toggle>
          </q-item-section>
        </q-item>
      </q-list>
    </q-page>`
}