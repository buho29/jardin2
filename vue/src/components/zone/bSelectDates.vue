<template>
  <q-input
    readonly autogrow dense filled
    input-class="cursor-pointer"
    label="Intervalos de fechas de riego (Mes/Dia)"
    :value="printDates"
    @click="$refs.monthPicker.show()">

    <template v-slot:append>
      <q-icon name="icon-calendar-clock" class="cursor-pointer">
        <q-popup-proxy ref="monthPicker" @before-show="updateProxy" transition-show="scale" transition-hide="scale">
          <q-date v-model="proxyDate"  range multiple :locale="myLocale"
            :navigation-min-year-month="year+'/01'"
            :navigation-max-year-month="year+'/12'">
            <div class="row items-center justify-end q-gutter-sm">
              <q-btn label="Cancelar" color="primary" flat v-close-popup ></q-btn>
              <q-btn label="OK" color="primary" flat @click="save" v-close-popup ></q-btn>
            </div>
          </q-date>
        </q-popup-proxy>
      </q-icon>
    </template>
  </q-input>
</template>
<script>
export default {
  name: 'b-select-dates',
  data() {
    return {            
      year: new Date().getFullYear(),
      datesStr: "",
      proxyDate:[],
    };
  },
  props: ["options", "dates", "label"],
  computed:{
    printDates()
    {
      let result = "";
      let arr = this.datesStr.split('|');
      for(let i = 0; i<arr.length; i++){
        result += arr[i] + " \n" ;
      }
      return result;
    },
    myLocale(){
      return this.$i18n.getLocaleMessage(this.$i18n.locale).date;
    }
  },
  watch: {
    
    //dates a cambiado
    dates: function (newValue, oldValue) {
      if(newValue.toString() === oldValue.toString()) 
        return;
      this.datesStr = newValue;
      this.updateProxy();
    }
  },
  methods: {

    updateProxy () {
      this.proxyDate = [];
      let interval = this.datesStr.split('|');
      for(let i = 0; i<interval.length; i++){
        let dates = interval[i].split('-');
        this.proxyDate.push(
          {
            from: this.year+"/"+dates[0],
            to: this.year+"/"+dates[1]
          }
        );
      }
    },
    save () {
      this.datesStr = "";
      if(this.proxyDate !== null) {
        for(let i = 0; i<this.proxyDate.length; i++){
          this.datesStr += this.proxyDate[i].from.slice(5) + "-" + 
            this.proxyDate[i].to.slice(5);
          if(i<this.proxyDate.length-1)
            this.datesStr += '|'
        }
      }
      this.$emit("input", this.datesStr);
    },
    
  },
  
}
</script>