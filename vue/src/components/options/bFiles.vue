<template>
  <b-container :title="$t('config.files')">
    <div class="row">
      <q-select
        v-model="root"
        :options="rootFiles"
        dense filled
        @input="rootClick"
        option-label="path"
        
        class="col-4"
      ></q-select>

      <q-select
        v-model="folder"
        :options="root.folders"
        class="col"
        dense
        @input="indexFolder = getIndex(folder.path, root.folders)"
        filled
        :option-label="
          item => (!!item.path ? '/' + item.path.replace(root.path, '') : null)
        "
      />
    </div>

    <q-list bordered>
      <transition-group name="list-complete">
        <q-item
          v-ripple
          v-for="file in folder.files"
          :key="file.name"
          class="list-complete-item"
          style=""
        >
          <q-item-section>
            <q-item-label>{{ file.name }}</q-item-label>
            <q-item-label caption>{{ file.size }}byte</q-item-label>
          </q-item-section>
          <q-item-section side>
            <div class=" q-gutter-sm text-white" style="display:flex;">
              <q-btn
                icon="icon-cloud-download"
                class="bg-primary"
                @click="downloadItem(file.name)"
              ></q-btn>
              <q-btn
                icon="icon-delete"
                class="bg-warning"
                @click="deleteItem(file.name)"
              ></q-btn>
            </div>
          </q-item-section>
        </q-item>
      </transition-group>
    </q-list>

    <q-form @submit="submitFile()" class="q-mt-sm">
      <div style="display:flex;">
        <q-file
          dense
          filled
          v-model="file"
          :label="$t('config.up')"
          style="flex-grow: 1;"
          lazy-rules
          :rules="[val => !!val || 'Selectiona un fichero por favor']"
        />
        <q-btn
          icon="icon-cloud-upload"
          color="primary"
          class="q-mb-lg q-ml-sm"
          type="submit"
        ></q-btn>
      </div>
    </q-form>
  </b-container>
</template>
<script>
import { mapActions,mapState } from "vuex";
import bContainer from '../bContainer.vue';
export default {
  components: { bContainer },
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
    }
  },
  watch: {
    // los datos cambiaron
    rootFiles: function(newValue, oldValue) {
      //volver a seleccionar los selects con los index
      this.root = this.rootFiles[this.indexRoot];
      this.folder = this.root.folders[this.indexFolder];
    }
  }
};
</script>
