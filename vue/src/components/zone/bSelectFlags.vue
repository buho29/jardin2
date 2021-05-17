<template>
  <q-select filled v-model="model" :options="options" 
    :label="label" multiple emit-value map-options>
    <template v-slot:option="scope">
      <q-item v-bind="scope.itemProps" v-on="scope.itemEvents">
        <q-item-section>
          <q-item-label v-html="scope.opt.label" ></q-item-label>
        </q-item-section>
        <q-item-section side>
          <q-toggle v-model="model" :val="scope.opt.value" />
        </q-item-section>
      </q-item>
    </template>
  </q-select>  
</template>
<script>
export default {
  name: 'b-select-flags',
  data() {
    return {
        //seleccion en bin ej: 2|16 (18)
        mFlags: 0,
        //modelo se llena segun el select ej :[2,16]
        model: []
    };
  },
  props: ["options", "flags", "label"],
  watch: {
    //model a cambiado
    model: function (newValue, oldValue) {
      
      if(newValue.toString() === oldValue.toString()) 
        return;
      //recalculamos flag
      this.mFlags = 0;
      for (let i = 0; i < newValue.length; i++) {
        //"sumamos" el valor
        this.addF(newValue[i]);
      }
      
      this.$emit("input", this.mFlags);
      
    },
    
    //flags a cambiado
    flags: function (newValue, oldValue) {
      this.set(newValue);
    }
  },
  methods: {
    //inicio/set
    set(flag) {
      this.mFlags = flag;
      //vaciamos model
      this.model.splice(0, this.model.length);
      
      for (let i = 0; i < this.options.length; i++) 
      {
        //cuando contiene el valor
        if (this.checkF(this.options[i].value)) {
          //lo agregamos al model
          this.model.push(this.options[i].value);
        }
      }
    },
    // agrega un flags
    addF(flags) {
      this.mFlags |= flags;
    },
    // devuelve si cuando existe todos los flags
    checkF(flags) {
      return (this.mFlags & flags) == flags;
    }
  },
  mounted() {
    if(this.flags)
    this.set(this.flags);
  },
  
}
</script>