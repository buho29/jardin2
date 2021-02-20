export default  {
    mixins: [mixinRequiresAuth],
    data: function () {
      return {
        icons:["icon-alarm" , "icon-alert-circle" , "icon-chevron-down" , 
          "icon-chevron-up" , "icon-cloud-check" , "icon-cog" , "icon-delete" , 
          "icon-menu" , "icon-menu-down" , "icon-moon" , "icon-pencil" , "icon-plus" ,
          "icon-stats-dots" , "icon-sun" , "icon-sun-cloud" , "icon-thermometer",
          "icon-watering-can" , "icon-water-pump","icon-eye-off", "icon-eye",
          "icon-cloud-upload", "icon-cloud-download" , "icon-file-outline",
          "icon-folder", "icon-folder-multiple","icon-help",
        ],
        meteo:["01","02","03","04","05","06","07","08","11","12","13","14","15","16","17",
          "18","19","20","21","22","23","24","25", "26","29","30","31","32","33","34","35",
          "36","37","38","39","40","41","42","43","44",
        ],
      }
    },
    computed: {
      //importamos datos
      ...Vuex.mapState(['system','authenticate',
            'routingServer','loaded','isConnected']),
      
    },
    methods: {
      // importamos acciones
      ...Vuex.mapActions(['restart','registerAuth']),
      ...Vuex.mapMutations(['setRoutingServer'])
    }, 
    mounted() {
      if(this.authenticate && this.isConnected)
        //nos registramos a eventos auth al servidor 
        this.registerAuth(true);
    },
    beforeDestroy() {
      this.registerAuth(false);
    },
    watch: {
      //si abren directamente la pagina opciones
      // y despues q se cargo to el tinglado
      loaded: function (newValue, oldValue) {
        //nos registramos a eventos auth al servidor
        this.registerAuth(true);
      },
    },//q-gutter-sm 
    template: /*html*/`
      <q-page>
      
        <b-config-tz/>
        <b-config-weather/>

        <b-container title="Navegador">
          <p>permite al servidor redireccionarte a una pagina <br/> 
            por ejemplo cuando se riega una zona</p>
          <q-toggle label= "routing server" :value="routingServer" 
            @input="setRoutingServer(!routingServer)">
          </q-toggle>
        </b-container>

        <b-config-admin/>
        <b-config-wifi/>
        <b-file/>
          
        <b-container title="System">
          
          <div  v-for="(value,key) in system" class="row q-mx-auto " :key="key" style="width:60%;" >
            <div class="col text-bold text-capitalize text-left"> {{key}}</div>
            <div class="col text-right"> {{value}}</div>
          </div>
          
          <q-btn color="primary" @click="restart()" class="q-ma-sm">restar Esp32</q-btn>
  
        </b-container>  
  
        <b-container title="Icons">
          <div class="text-purple q-pa-md row items-start">
            <div v-for="(item,id) in icons" :key="id"  style="width: 110px;">
              <q-icon :name="item" style="font-size: 2em;"></q-icon>  
              <p>{{item}}</p>    
            </div>
          </div>
        </b-container>
  
        <b-container title="Meteo">
          <div class="q-pa-md row items-start">
            <b-icon  v-for="(item) in meteo" :key="item"
              :icon="item"  style="font-size:80px;"/>
          </div>
        </b-container>
  
      </q-page>`
}

var mixinOption = {
  computed: {
    //importamos datos
    ...Vuex.mapState(['config']),
  },
  watch: {
    // los datos cambiaron 
    config: function (newValue, oldValue) {
        this.update();
    }
  },
  mounted() {
    this.update();
  },
}

//option components
Vue.component("b-file", {
    data: function () {
        return {
            file: null, //file data
            root: [],//select
            folder: [],//select
            indexRoot: 0,//usado para reselecionar el select
            indexFolder: 0,
        }
    },
    computed: {
        //importamos datos
        ...Vuex.mapState(['rootFiles']),
    },
    methods: {
        // importamos acciones
        ...Vuex.mapActions(['downloadItem', 'deleteItem', 'uploadItem']),
        submitFile() {
            this.uploadItem({
                file: this.file,
                params: { path: this.folder.path }
            });
        },
        rootClick() {
            //selecionar la primera carpeta
            this.folder = this.root.folders[0];
            this.indexFolder = 0;
            //guardar index
            this.indexRoot = this.getIndex(this.root.path, this.rootFiles);
        },
        folderClick() {
            //guardar index
            this.indexFolder = this.getIndex(this.folder.path, this.root.folders);
        },
        getIndex(path, array) {
            return array.findIndex(file => file.path === path);
        },
    },
    watch: {
        // los datos cambiaron 
        rootFiles: function (newValue, oldValue) {
            //volver a seleccionar los selects con los index
            this.root = this.rootFiles[this.indexRoot];
            this.folder = this.root.folders[this.indexFolder];
        }
    },
    template: /*html*/`
    <b-container title="File">

        <div class="row">
        <q-select v-model="root" :options="rootFiles" dense
        @input="rootClick" option-label="path" filled  class="col-4"></q-select>
    
        <q-select v-model="folder" :options="root.folders"  class="col" dense
        @input="indexFolder = getIndex(folder.path,root.folders)" filled 
        :option-label="(item) => !!item.path ? '/'+item.path.replace(root.path,''):null" />     
        </div>
    
        <q-list bordered>
    
        <transition-group name="list-complete">
            <q-item v-ripple v-for="file in folder.files" :key="file.name" class='list-complete-item' style="">
                <q-item-section>
                <q-item-label>{{file.name}}</q-item-label>
                <q-item-label caption>{{file.size}}byte</q-item-label>
                </q-item-section>
                <q-item-section side>
                <div class=" q-gutter-sm text-white" style="display:flex;">
                    <q-btn icon="icon-cloud-download" class="bg-primary" 
                    @click="downloadItem(file.name)"></q-btn>
                    <q-btn icon="icon-delete" class="bg-warning"
                    @click="deleteItem(file.name)"></q-btn>
                </div>
                </q-item-section>
            </q-item>
        </transition-group>
        </q-list>
    
        <q-form  @submit="submitFile()" class="q-mt-sm">
        <div style="display:flex;">
            <q-file dense filled v-model="file" label="File" style="flex-grow: 1;"
            lazy-rules :rules="[ val => !!val || 'Selectiona un fichero por favor']"/>
            <q-btn icon="icon-cloud-upload" color="primary" class="q-mb-lg q-ml-sm"
            type="submit"></q-btn>     
        </div>
        </q-form>
    
    </b-container>
`});

Vue.component("b-config-admin", {
    data: function () {
        return {
            name: null,
            pass: null,
            isPwd: true
        };
    },
    methods: {
        ...Vuex.mapActions(['editConfig']),
        onSubmit() {
            this.editConfig({www_user : this.name, www_pass: this.pass });
        },
        onReset() {
            this.name = null;
            this.pass = null;
        }
    },
    template: /*html*/`
      <b-container title="Administrador">
        <q-form @submit="onSubmit" @reset="onReset" class="q-gutter-sm" >
          <p>Cambiar login</p>
          <q-input filled v-model="name" label="Tu nombre *" dense
            lazy-rules :rules="[ val => val && val.length > 0 || 'Please type something']">
          </q-input>
          
          <q-input v-model="pass" filled dense
            lazy-rules :rules="[ val => val && val.length > 0 || 'Please type something']"
            :type="isPwd ? 'password' : 'text'" label="Tu password *">
            <template v-slot:append>
              <q-icon
                :name="isPwd ? 'icon-eye-off' : 'icon-eye'"
                class="cursor-pointer"
                @click="isPwd = !isPwd"
              />
            </template>
          </q-input>
  
          <div>
            <q-btn label="Enviar" type="submit" color="primary" />
            <q-btn label="Reset" type="reset" color="primary" flat class="q-ml-sm" />
          </div>
        </q-form>   
      </b-container>
`});

Vue.component("b-config-weather", {
    data: function () {
        return {
          cityName: null,
          cityID: null,
          accuURL: null
        };
    },
    mixins: [mixinOption],
    methods: {
        ...Vuex.mapActions(['editConfig']),
        onSubmit() {
          this.editConfig({ 
            cityName: this.cityName,
            accuURL:this.accuURL, 
            cityID: this.cityID 
          });
        },
        onReset() {
          this.cityName = null;
          this.cityID = null;
        },
        update(){
          this.cityName = this.config.cityName;
          this.cityID = this.config.cityID;
          this.accuURL = this.config.accuURL;
        }
    },
    template: /*html*/`
    <b-container title="Accuweather.com">
      <q-form @submit="onSubmit" @reset="onReset" class="q-gutter-sm" >
      
      <q-input filled v-model="cityName" label="Tu nombre ciudad *" dense
        lazy-rules :rules="[ val => val && val.length > 0 || 'Please type something']">
      </q-input>
      <q-input filled v-model="cityID" label="Tu cityID *" dense
        lazy-rules :rules="[ val => val && val.length > 0 || 'Please type something']">
      </q-input>
      <q-input filled v-model="accuURL" label="extra url *" dense 
      hint="key & lang & metric" 
        lazy-rules :rules="[ val => val && val.length > 0 || 'Please type something']">
      </q-input>
  
        <div>
          <q-btn label="Enviar" type="submit" color="primary"/>
          <q-btn label="Reset" type="reset" color="primary" flat class="q-ml-sm" />
        </div>
      </q-form>   
    </b-container>
`});

Vue.component("b-config-wifi", {
    data: function () {
        return {
            name: null,
            pass: null,
            isPwd: true
        };
    },
    mixins: [mixinOption],
    methods: {
        ...Vuex.mapActions(['editConfig']),
        onSubmit() {
            this.editConfig({ wifi_ssid: this.name, wifi_pass: this.pass });
        },
        onReset() {
            this.name = null;
            this.pass = null;
        },
        update(){
          this.name = this.config.wifi_ssid;
        }
    },
    template: /*html*/`
    <b-container title="Wifi">
      <q-form @submit="onSubmit" @reset="onReset" class="q-gutter-sm" >
      
        <q-input filled v-model="name" label="Tu SSID(wifi) *" dense
          lazy-rules :rules="[ val => val && val.length > 0 || 'Please type something']">
        </q-input>
        
        <q-input v-model="pass" filled dense
          lazy-rules :rules="[ val => val && val.length > 0 || 'Please type something']"
          :type="isPwd ? 'password' : 'text'" label="Tu password *">
          <template v-slot:append>
            <q-icon
              :name="isPwd ? 'icon-eye-off' : 'icon-eye'"
              class="cursor-pointer"
              @click="isPwd = !isPwd"
            />
          </template>
        </q-input>
  
        <div>
          <q-btn label="Enviar" type="submit" color="primary"/>
          <q-btn label="Reset" type="reset" color="primary" flat class="q-ml-sm" />
        </div>
      </q-form>   
    </b-container>
`});

Vue.component("b-config-tz", {
    data: function () {
        return {
            tz: 0,
            dst: "0, 0, 00, 0, 00, 0, 0 ",
        };
    },
    computed: {
      //importamos datos
      ...Vuex.mapState(['config']),
    },
    watch: {
      // los datos cambiaron 
      config: function (newValue, oldValue) {
          //volver a seleccionar los selects con los index
          this.tz = newValue.tz;
          this.dst = newValue.dst;
      }
    },
    methods: {
      ...Vuex.mapActions(['editConfig']),
      onSubmit() {
          this.editConfig({ tz: this.tz, dst: this.dst }); 
      },
      onReset() {
          this.tz = null;
          this.dst = null;
      },
      getTz(){
        let t = this.config.tz;
        console.log(t);
        return t;
      }
    },// class="q-gutter-md " 
    template: /*html*/`
    <b-container title="Config Time">
      <q-form class="q-gutter-sm row" @submit="onSubmit" @reset="onReset">

        <q-input :value="getTz()" label="Time zone" filled hint="Offset(hours)" style="max-width: 110px;" 
          v-model.number="tz" type="number" min="-12" max="12" lazy-rules  :rules="[
            val => val !== null && val !== '' || 'Esta mal...',
            val => val > -13 && val < 13 || 'Escribe entre -12 y +12',
          ]"></q-input>

        
        <q-input label="Daylight saving time"  filled style="flex-grow: 1;" hint="irregularity, startDay, startDay, endDay, endMonth, startHour, endHour" 
          v-model="dst" mask=" # , # , ## , # , ## , # , #">
          <template v-slot:append>
            <q-icon name="icon-help" @click="$window.open('https://github.com/kosme/flex_DST','_blank')"/>
          </template>
        </q-input>
        
        <div class="q-mt-xl" style="width: 100%">
          <q-btn label="Enviar" type="submit" color="primary"/>
          <q-btn label="Reset" type="reset" color="primary" flat class="q-ml-sm" />
        </div>
      </q-form>
    </b-container>
`});

Vue.component("b-select-flags", {
  data: function () {
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
  template: /*html*/ `
  <q-select filled v-model="model" :options="options" :label="label"
    multiple emit-value map-options>
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
`});