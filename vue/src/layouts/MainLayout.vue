<template>
  <q-layout view="hHh LpR ffr">
    <q-header elevated>
      <q-toolbar>
        <q-btn
          flat
          dense
          round
          @click="leftDrawerOpen = !leftDrawerOpen"
          aria-label="Menu"
          icon="icon-menu"
        ></q-btn>
        <q-toolbar-title>{{ $route.meta.title }}</q-toolbar-title>
        <q-btn v-if="authenticate" flat round @click="logout()">logout</q-btn>
      </q-toolbar>
    </q-header>

    <q-drawer
      v-model="leftDrawerOpen"
      show-if-above
      bordered
      content-class="bg-grey-1"
    >
      <q-list>
        <q-item-label header>Navegacion</q-item-label>
        <q-item
          v-for="(item, index) in menu"
          :key="index"
          :to="item.path"
          exact
        >
          <q-item-section avatar>
            <q-icon :name="item.icon"></q-icon>
          </q-item-section>
          <q-item-section>
            <q-item-label>{{ item.title }}</q-item-label>
          </q-item-section>
        </q-item>
      </q-list>
    </q-drawer>

    <q-page-container>
      <transition :name="transitionName">
        <router-view class="child-view"></router-view>
      </transition>
    </q-page-container>
  </q-layout>
</template>

<script>
//menu leftdata
const menu = [
  { title: "Home", icon: "icon-sun-cloud", path: "/" },
  { title: "Gráfico", icon: "icon-stats-dots", path: "/chart" },
  { title: "Zonas", icon: "icon-watering-can", path: "/zones/0" },
  { title: "Grifos", icon: "icon-water-pump", path: "/taps" },
  { title: "Login", icon: "icon-cloud-check", path: "/login" },
  { title: "Opciones", icon: "icon-cog", path: "/options" }
];

import { mapActions, mapMutations, mapState, mapGetters } from "vuex";

export default {
  name: "MainLayout",
  data() {
    return {
      leftDrawerOpen: false,
      transitionName: "slide-right",
      menu: menu
    };
  },
  computed: {
    ...mapState(["authenticate"])
  },
  watch: {
    //cambiamos el estilo de las transiciones
    // cada vez q se cambia de pagina
    $route(to, from) {
      this.transitionName =
        this.transitionName === "slide-left" ? "slide-right" : "slide-left";
    },
    authenticate: function(newValue, oldValue) {
      this.buttonLogin.show = !newValue;
    }
  }
};
</script>
