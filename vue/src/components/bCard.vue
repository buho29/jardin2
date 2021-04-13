<template>
  <q-card
    class="text-center items-center bg-white" flat bordered
  >
    <q-item class="bg-primary">
      <q-item-section>
        <q-item-label class="text-white text-h6 ">{{ title }}</q-item-label>
      </q-item-section>
    </q-item>

    <q-separator/>

    <q-card-section >
      <slot></slot>
    </q-card-section>
  </q-card>
</template>
<script>
export default {
  name: "b-card",
  props: ["title"]
};
</script>
