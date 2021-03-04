<template>
  <q-select filled label="Search cityID" dense hide-dropdown-icon
    fill-input use-input hide-selected input-debounce="500"
    v-model="model" :options="options" @filter="filterFn"
  >
    <template v-slot:append>
      <q-icon name="icon-magnify" @click="model = null" />
    </template>

    <template v-slot:option="scope">
      <q-item v-bind="scope.itemProps" v-on="scope.itemEvents">
        <q-item-section>
          <q-item-label>{{ scope.opt.label }}</q-item-label>
          <q-item-label caption>{{ scope.opt.value }}</q-item-label>
        </q-item-section>
      </q-item>
    </template>

    <template v-slot:no-option>
      <q-item>
        <q-item-section class="text-grey">
          No results
        </q-item-section>
      </q-item>
    </template>
  </q-select>
</template>
<script>
export default {
  name:"b-Search-city",
  data() {
    return {
      model: null,
      options: null,
      dataLoaded: null
    };
  },
  methods: {
    async filterFn(val, update, abort) {
      if (val.length > 2) {
        await this.$axios.get("https://dataservice.accuweather.com/locations/v1/cities/search",
            {
              params: {
                apikey: "q329xaaTojo0koLv6A3uFgh3dQLgp6em ",
                q: val.toLocaleLowerCase()
              }
            }
          )
          .then(response => {
            this.dataLoaded = response.data.map(obj => {
              var o = {};
              o.label = obj.LocalizedName;
              o.value =
                obj.Country.LocalizedName + " - " + obj.Region.LocalizedName;
              o.cityID = obj.Key;
              o.region = obj.Region.LocalizedName;
              o.country = obj.Country.LocalizedName;
              return o;
            });
          })
          .catch(error => {
            console.log(error);
          });

        update(() => {
          this.options = this.dataLoaded;
        });
      } else {
        update(() => {
          this.options = [];
        });
      }
    }
  },
  watch:{
      model(newValue,oldValue){
          this.$emit("input",this.model);
      }
  }
};
</script>
