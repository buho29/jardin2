<template>
  <q-layout view="hHh LpR ffr">
    <q-header elevated>
      <q-toolbar>
        <q-btn flat dense round
          @click="leftDrawerOpen = !leftDrawerOpen"
          aria-label="Menu"  icon="icon-menu"
        />
        <q-toolbar-title>{{ $t(`titles.${$route.name}`) }}</q-toolbar-title>
        <q-btn v-if="authenticate" flat round @click="logout()">logout</q-btn>
        <q-btn v-else flat round to="/login">login</q-btn>
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
        <q-item exact
          v-for="(item, index) in menu" :key="index" :to="item.path"
          >
          <q-item-section avatar>
            <q-icon :name="item.icon"/>
          </q-item-section>
          <q-item-section>
            <q-item-label>{{ $t(`titles.${item.title}`) }}</q-item-label>
          </q-item-section>
        </q-item>
      </q-list>
    </q-drawer>

    <q-page-container>
      <transition :name="transitionName">
        <router-view class="child-view"/>
      </transition>
    </q-page-container>
  </q-layout>
</template>

<script>
//menu leftdata
const menu = [
  { title: "home", icon: "icon-sun-cloud", path: "/" },
  { title: "charts", icon: "icon-stats-dots", path: "/chart" },
  { title: "zones", icon: "icon-watering-can", path: "/zones/0" },
  { title: "taps", icon: "icon-water-pump", path: "/taps" },
  { title: "options", icon: "icon-cog", path: "/options" }
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
  methods:{
    ...mapMutations(["logout"])
  },
  watch: {
    //cambiamos el estilo de las transiciones
    // cada vez q se cambia de pagina
    $route(to, from) {
      this.transitionName =
        this.transitionName === "slide-left" ? "slide-right" : "slide-left";
    },
  }
};
</script>
<style>

/* router animation */
.child-view {
  position: absolute;
  width: 100%;
  left: 0;
  transition: all 0.75s cubic-bezier(0.55, 0, 0.1, 1); /**/
}

.slide-left-enter,
.slide-right-leave-active {
  opacity: 0;
  -webkit-transform: translate(30%, 0);
  transform: translate(30%, 0);
}
.slide-left-leave-active,
.slide-right-enter {
  opacity: 0;
  -webkit-transform: translate(-30%, 0);
  transform: translate(-30%, 0);
}

</style>
