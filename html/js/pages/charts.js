
const charts = {
    computed: {
        ...Vuex.mapState(['sensors'])
    },
    template: /*html*/`
    <q-page>
      <div class="col text-center">
        <chart :data="sensors" title="Temperatura" tag="te"></chart>
        <chart :data="sensors" title="Pression" tag="pr"></chart>
        <chart :data="sensors" title="Humedad" tag="hu"></chart>
      </div>
    </q-page>
`
}
