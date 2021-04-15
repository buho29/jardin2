<template>
  <div>
    <div class="row">
      <q-select
        dense filled option-label="path" class="col-4"
        @input="rootClick" v-model="root" :options="rootFiles"
      />

      <q-select
        dense filled class="col"
        v-model="folder" :options="root.folders"
        @input="indexFolder = getIndex(folder.path, root.folders)"
        :option-label="
          item => (!!item.path ? '/' + item.path.replace(root.path, '') : null)
        "
      />
    </div>

    <q-list bordered>
      <transition-group name="list-complete">
        <q-item v-ripple class="list-complete-item"
          v-for="file in folder.files" :key="file.name"
        >
          <q-item-section>
            <q-item-label>{{ file.name }}</q-item-label>
            <q-item-label caption>{{ formatSize(file.size) }}</q-item-label>
          </q-item-section>
          <q-item-section side>
            <div class=" q-gutter-sm text-white" style="display:flex;">
              <q-btn
                icon="icon-cloud-download" class="bg-primary"
                @click="downloadItem(file.name)"
              ></q-btn>
              <q-btn
                icon="icon-delete" class="bg-warning"
                @click="deleteItem(file.name)"
              />
            </div>
          </q-item-section>
        </q-item>
      </transition-group>
    </q-list>

    <q-form @submit="submitFile()" class="q-mt-sm">
      <div style="display:flex;">
        <q-file dense filled style="flex-grow: 1;"
          v-model="file"
          lazy-rules :label="$t('config.up')"
          :rules="[val => !!val || 'Selectiona un fichero por favor']"
        />
        <q-btn
          icon="icon-cloud-upload" color="primary" class="q-mb-lg q-ml-sm"
          type="submit"
        />
      </div>
    </q-form>
  </div>
</template>

<script>
import { mapActions, mapState } from "vuex";
// we import all of `format`
import { format } from 'quasar'
// destructuring to keep only what is needed
const { humanStorageSize } = format

export default {
  name: "b-files",
  data() {
    return {
      file: null, //file data
      root: [], //select
      folder: [], //select
      indexRoot: 0, //usado para reselecionar el select
      indexFolder: 0
    };
  },
  computed: {
    //importamos datos
    ...mapState(["rootFiles"])
  },
  methods: {
    // importamos acciones
    ...mapActions(["downloadItem", "deleteItem", "uploadItem"]),
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
    // los datos cambiaron
    update(){
      //volver a seleccionar los selects con los index
      this.root = this.rootFiles[this.indexRoot];
      this.folder = this.root.folders[this.indexFolder];      
    },
    formatSize(size){
      return humanStorageSize(size);
    }
  },
  mounted() {
    this.update();
  },
  watch: {
    rootFiles: function(newValue, oldValue) {
      this.update();
    }
  }
};
</script>

<style scoped>

.list-complete-item {
  transition: all .5s;
}
.list-complete-enter,
.list-complete-leave-to {
  opacity: 0;
  transform: translateY(30px);
}
.list-complete-leave-active {
   position: absolute; /**/
}
</style>
