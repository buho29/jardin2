<template>
  <q-card
    class="q-ma-lg q-mx-auto text-center items-center bg-white page"
  >
    <q-card-section class="q-pa-none q-ma-none">
      <div class="bg-primary text-white shadow-3 round-top">
        <div class="q-pa-sm text-h6 ">{{ title }}</div>
      </div>
    </q-card-section>
    <q-card-section >
      <slot></slot>
    </q-card-section>
  </q-card>
</template>
<script>
export default {
  name: "b-container",
  props: ["title"],
};
</script>